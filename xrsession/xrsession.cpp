#define _CRT_SECURE_NO_WARNINGS

#include "xrsession.h"

#include <dawn/exports.h>

#include <openxr/openxr.h>

#include <dawn/native/D3D12Backend.h>
#include <dawn/native/VulkanBackend.h>

#include <thread>

// clang-format off
#define XR_ASSERT(X) { \
	auto r=(X);if(XR_FAILED(r)) { \
		SGD_PANIC("!!! XR_ASSERT FAILED: " #X " " + std::to_string(r)); \
	} \
}

#define XR_TRY(X) { \
	auto r=(X);if(XR_FAILED(r)) { \
		log() << "### XR_TRY FAILED: " #X << r; \
		return r; \
	} \
}

#define XR_PROC(XRINST, FUNCID) \
	PFN_##FUNCID FUNCID{}; \
	xrGetInstanceProcAddr((XRINST), #FUNCID, (PFN_xrVoidFunction*)(&FUNCID));
// clang-format on

namespace sgd {

namespace {

Mat4f projectionMatrix(float angleLeft, float angleRight, float angleUp, float angleDown, float zNear, float zFar) {

	float left = std::tan(angleLeft), right = std::tan(angleRight), up = std::tan(angleUp), down = std::tan(angleDown);

	float w = right - left, h = up - down, d = zFar - zNear;

	// clang-format off
	return {{2 / w,					0,						0,							0},
			{0,						2 / h,					0,							0},
			{-(right + left) / w,	-(up + down) / h,		zFar  / d, 					1},
			{0,						0,						-zFar * zNear  / d,			0}};
	// clang-format on
}

AffineMat4f poseMatrix(XrQuaternionf orientation, XrVector3f position) {

	AffineMat4f m{transpose(Mat3f::rotation((Quatf&)orientation)), (Vec3f&)position};

	m.r.i.z = -m.r.i.z;
	m.r.j.z = -m.r.j.z;
	m.r.k.x = -m.r.k.x;
	m.r.k.y = -m.r.k.y;
	m.t.z = -m.t.z;

	return m;
}

AffineMat4f poseMatrix(const XrPosef& pose) {

	AffineMat4f m{transpose(Mat3f::rotation((CQuatf)pose.orientation)), (CVec3f)pose.position};

	m.r.i.z = -m.r.i.z;
	m.r.j.z = -m.r.j.z;
	m.r.k.x = -m.r.k.x;
	m.r.k.y = -m.r.k.y;
	m.t.z = -m.t.z;

	return m;
}

void sleep(uint32_t ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

XrInstance g_instance;
XrSystemId g_systemId;

XrResult createXrInstance() {

	if (g_instance) return XR_SUCCESS;

	if (0) {
		uint32_t nprops;
		xrEnumerateInstanceExtensionProperties(nullptr, 0, &nprops, nullptr);
		Vector<XrExtensionProperties> props(nprops, {XR_TYPE_EXTENSION_PROPERTIES});

		xrEnumerateInstanceExtensionProperties(nullptr, nprops, &nprops, props.data());
		SGD_ASSERT(nprops == props.size());

		log() << "OpenXR extensions:";
		for (uint32_t i = 0; i < nprops; ++i) log() << props[i].extensionName;
	}

	{
		std::vector<const char*> enabledExts;
#ifdef XR_USE_GRAPHICS_API_D3D12
		enabledExts.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
#endif
#ifdef XR_USE_GRAPHICS_API_VULKAN
		enabledExts.push_back(XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME);
#endif
		// enabledExts.push_back(XR_HTC_PASSTHROUGH_EXTENSION_NAME);

		XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};

		std::strncpy(createInfo.applicationInfo.applicationName, "WasmBasic Application", XR_MAX_APPLICATION_NAME_SIZE);
		std::strncpy(createInfo.applicationInfo.engineName, "WasmBasic Engine", XR_MAX_ENGINE_NAME_SIZE);

		createInfo.applicationInfo.applicationVersion = XR_VERSION_1_0;	  // 0x10000;
		createInfo.applicationInfo.engineVersion = XR_VERSION_1_0;		  // 0x10000;
		createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0); // XR_VERSION_1_0;

		createInfo.enabledExtensionCount = enabledExts.size();
		createInfo.enabledExtensionNames = enabledExts.data();

		XR_TRY(xrCreateInstance(&createInfo, &g_instance));

		XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO};
		systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XR_TRY(xrGetSystem(g_instance, &systemGetInfo, &g_systemId));
	}

	{
		XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
		XR_TRY(xrGetSystemProperties(g_instance, g_systemId, &systemProperties));
		log() << "XrInstance created - " << systemProperties.systemName;
		//		log() << "Swap chain image size:" << systemProperties.graphicsProperties.maxSwapchainImageWidth << ","
		//			  << systemProperties.graphicsProperties.maxSwapchainImageHeight;
		//		log() << "Layers:" << systemProperties.graphicsProperties.maxLayerCount;
	}

	return XR_SUCCESS;
}

XrInstance getXrInstance() {
	XR_ASSERT(createXrInstance());
	return g_instance;
}

XrSystemId getXrSystemId() {
	XR_ASSERT(createXrInstance());
	return g_systemId;
}

} // namespace

struct XRSessionImpl : XRSession {

	XRSessionImpl(wgpu::Device device, XrInstance instance, XrSystemId systemId)
		: m_device(device), m_instance(instance), m_systemId(systemId) {
		XR_ASSERT(createSession());
		XR_ASSERT(createSwapchain());
		XR_ASSERT(createActions());
	}

	const wgpu::Device& wgpuDevice() const override {
		return m_device;
	}

	wgpu::TextureFormat swapChainFormat() const override {
		return (wgpu::TextureFormat)m_swapchainTextureFormat;
	}

	Vec2u swapChainSize() const override {
		return m_swapchainTextureSize;
	}

	const XRFrame& beginFrame() override {

		//		log() << "Polling...";

		// Poll until ready, give up after 10 seconds - TODO: then what?
		for (int i = 0; i < 100; ++i) {
			pollEvents();
			if (m_ready) break;
			sleep(100);
		}
		if (!m_ready) SGD_ABORT();

		//		log() << "Waiting...";

		waitFrame();

		//		log() << "Ready";

		updatePoses();

		return m_frame;
	}

	void endFrame() override {

		//		log() << "End";

		XR_ASSERT(xrReleaseSwapchainImage(m_swapchain, nullptr));

		m_frameEndInfo.displayTime = m_frameState.predictedDisplayTime;
		XR_ASSERT(xrEndFrame(m_session, &m_frameEndInfo));
	}

private:
	XrResult createSession() {

		// Create openxr session
		{
			dawnxr::GraphicsBindingDawn binding{XR_TYPE_GRAPHICS_BINDING_DAWN_EXT};
			binding.device = m_device;

			XrSessionCreateInfo info{XR_TYPE_SESSION_CREATE_INFO};
			info.next = &binding;
			info.systemId = m_systemId;

			XR_TRY(dawnxr::createSession(m_instance, &info, &m_session));
		}

		// Create local reference space
		{
			XrReferenceSpaceCreateInfo info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};

			info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
			info.poseInReferenceSpace.orientation.w = 1;

			XR_ASSERT(xrCreateReferenceSpace(m_session, &info, &m_localSpace));
		}

		return XR_SUCCESS;
	}

	XrResult createSwapchain() {

		// Get view configuration views
		{
			XrViewConfigurationView viewConfigurationViews[2]{};

			viewConfigurationViews[0].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
			viewConfigurationViews[1].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

			uint32_t n;
			XR_TRY(xrEnumerateViewConfigurationViews(m_instance, m_systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2, &n,
													 viewConfigurationViews));

			SGD_ASSERT(n == 2);
#if 0
		SGD_ASSERT(viewConfigurationViews[0].recommendedImageRectWidth ==
			   viewConfigurationViews[1].recommendedImageRectWidth);
		SGD_ASSERT(viewConfigurationViews[0].recommendedImageRectHeight ==
			   viewConfigurationViews[1].recommendedImageRectHeight);
#endif
			SGD_ASSERT(viewConfigurationViews[0].recommendedSwapchainSampleCount ==
					   viewConfigurationViews[1].recommendedSwapchainSampleCount);

			Vec2u size0{viewConfigurationViews[0].recommendedImageRectWidth,
						viewConfigurationViews[0].recommendedImageRectHeight};

			Vec2u size1{viewConfigurationViews[1].recommendedImageRectWidth,
						viewConfigurationViews[1].recommendedImageRectHeight};

			m_swapchainTextureSize = {size0.x + size1.x, std::max(size0.y, size1.y)};
			m_swapchainSampleCount = viewConfigurationViews[0].recommendedSwapchainSampleCount;

			m_viewports[0] = {0, 0, (int)size0.x, (int)size0.y};
			m_viewports[1] = {(int)size0.x, 0, (int)size0.x + (int)size1.x, (int)size1.y};
		}

		// Create swapchain and framebuffer
		{
			uint32_t n;
			XR_TRY(dawnxr::enumerateSwapchainFormats(m_session, 0, &n, nullptr));

			Vector<int64_t> swapchainFormats(n);
			XR_TRY(dawnxr::enumerateSwapchainFormats(m_session, n, &n, swapchainFormats.data()));

			if (!n) SGD_PANIC("Can't find suitable swapchain texture format");

			m_swapchainTextureFormat = swapchainFormats[0];
			//log() << "### SwapChainTextureFormat:" << m_swapchainTextureFormat;

			XrSwapchainCreateInfo swapchainInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
			swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
			swapchainInfo.format = m_swapchainTextureFormat;
			swapchainInfo.sampleCount = m_swapchainSampleCount;
			swapchainInfo.width = m_swapchainTextureSize.x;
			swapchainInfo.height = m_swapchainTextureSize.y;
			swapchainInfo.faceCount = 1;
			swapchainInfo.arraySize = 1;
			swapchainInfo.mipCount = 1;
			XR_TRY(dawnxr::createSwapchain(m_session, &swapchainInfo, &m_swapchain));

			XR_TRY(dawnxr::enumerateSwapchainImages(m_swapchain, 0, &n, nullptr));

			Vector<dawnxr::SwapchainImageDawn> swapchainImages;
			swapchainImages.insert(swapchainImages.end(), n, {XR_TYPE_SWAPCHAIN_IMAGE_DAWN_EXT});

			XR_TRY(dawnxr::enumerateSwapchainImages(m_swapchain, n, &n, (XrSwapchainImageBaseHeader*)swapchainImages.data()));

			for (size_t i = 0; i < n; ++i) m_swapchainTextures.push_back(swapchainImages[i].texture);
		}

		// Initialize layers
		{
			m_viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
			m_viewLocateInfo.space = m_localSpace;

			for (size_t eye = 0; eye < 2; ++eye) {
				auto& projView = m_projViews[eye];
				projView.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
				projView.subImage.swapchain = m_swapchain;
				projView.subImage.imageRect.offset = {m_viewports[eye].x(), m_viewports[eye].y()};
				projView.subImage.imageRect.extent = {m_viewports[eye].width(), m_viewports[eye].height()};
			}

			m_projLayer.space = m_localSpace;
			m_projLayer.viewCount = 2;
			m_projLayer.views = m_projViews;

			m_layers[0] = (XrCompositionLayerBaseHeader*)&m_projLayer;
			// m_layers[1] = (XrCompositionLayerBaseHeader*)&m_passthroughLayer;

			m_frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
			m_frameEndInfo.layerCount = 1;
			m_frameEndInfo.layers = m_layers;
		}

		return XR_SUCCESS;
	}

	XrResult createActions() {

		// Create action set
		{
			XrActionSetCreateInfo createInfo{XR_TYPE_ACTION_SET_CREATE_INFO};

			std::strncpy(createInfo.actionSetName, "action_set", XR_MAX_ACTION_SET_NAME_SIZE);
			std::strncpy(createInfo.localizedActionSetName, "Action set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);

			XR_TRY(xrCreateActionSet(m_instance, &createInfo, &m_actionSet));

			m_activeActionSets.actionSet = m_actionSet;
		}

		XrPath handPaths[2]{};
		XR_TRY(xrStringToPath(m_instance, "/user/hand/left", &handPaths[0]));
		XR_TRY(xrStringToPath(m_instance, "/user/hand/right", &handPaths[1]));

		// Create aim pose and grip pose actions
		{
			XrActionCreateInfo createInfo{XR_TYPE_ACTION_CREATE_INFO};
			createInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
			createInfo.countSubactionPaths = 2;
			createInfo.subactionPaths = handPaths;

			std::strncpy(createInfo.actionName, "aim_poses", XR_MAX_ACTION_NAME_SIZE);
			std::strncpy(createInfo.localizedActionName, "Aim poses", XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
			XR_TRY(xrCreateAction(m_actionSet, &createInfo, &m_aimPoseAction));

			std::strncpy(createInfo.actionName, "grip_poses", XR_MAX_ACTION_NAME_SIZE);
			std::strncpy(createInfo.localizedActionName, "Grip poses", XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
			XR_TRY(xrCreateAction(m_actionSet, &createInfo, &m_gripPoseAction));
		}

		// Create aim pose spaces and grip pose spaces
		{
			for (size_t hand = 0; hand < 2; ++hand) {

				XrActionSpaceCreateInfo spaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};

				spaceInfo.subactionPath = handPaths[hand];
				spaceInfo.poseInActionSpace.orientation.w = 1;

				spaceInfo.action = m_aimPoseAction;
				XR_TRY(xrCreateActionSpace(m_session, &spaceInfo, &m_aimPoseSpaces[hand]));

				spaceInfo.action = m_gripPoseAction;
				XR_TRY(xrCreateActionSpace(m_session, &spaceInfo, &m_gripPoseSpaces[hand]));
			}
		}

		// Associate actions with poses
		{
			XrPath profilePath;
			XR_TRY(xrStringToPath(m_instance, "/interaction_profiles/khr/simple_controller", &profilePath));

			XrPath bindingPaths[4];
			XR_TRY(xrStringToPath(m_instance, "/user/hand/left/input/aim/pose", &bindingPaths[0]));
			XR_TRY(xrStringToPath(m_instance, "/user/hand/right/input/aim/pose", &bindingPaths[1]));
			XR_TRY(xrStringToPath(m_instance, "/user/hand/left/input/grip/pose", &bindingPaths[2]));
			XR_TRY(xrStringToPath(m_instance, "/user/hand/right/input/grip/pose", &bindingPaths[3]));

			XrActionSuggestedBinding suggestedBindings[] = {{m_aimPoseAction, bindingPaths[0]},
															{m_aimPoseAction, bindingPaths[1]},
															{m_gripPoseAction, bindingPaths[2]},
															{m_gripPoseAction, bindingPaths[3]}};

			XrInteractionProfileSuggestedBinding profileBindings = {XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
			profileBindings.interactionProfile = profilePath;
			profileBindings.countSuggestedBindings = std::size(suggestedBindings);
			profileBindings.suggestedBindings = suggestedBindings;

			XR_TRY(xrSuggestInteractionProfileBindings(m_instance, &profileBindings));
		}

		// Attach action sets to session
		{
			XrSessionActionSetsAttachInfo attachInfo = {XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO, nullptr, 1, &m_actionSet};
			XR_TRY(xrAttachSessionActionSets(m_session, &attachInfo));
		}

		return XR_SUCCESS;
	}

	XrResult pollEvents() {

		XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};

		if (xrPollEvent(m_instance, &event) == 0) {
			switch (event.type) {
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
				auto ev = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
				if (ev->state == XR_SESSION_STATE_READY) {
					if (m_ready) SGD_PANIC("OOPS");
					XrSessionBeginInfo info{XR_TYPE_SESSION_BEGIN_INFO};
					info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
					XR_ASSERT(xrBeginSession(m_session, &info));
					m_ready = true;
				} else if (ev->state == XR_SESSION_STATE_SYNCHRONIZED) {
				} else if (ev->state == XR_SESSION_STATE_VISIBLE) {
				} else if (ev->state == XR_SESSION_STATE_FOCUSED) {
				}
			} break;
			default:
				break;
			}
		}

		return XR_SUCCESS;
	}

	XrResult waitFrame() {

		// Wait for next frame
		XR_ASSERT(xrWaitFrame(m_session, nullptr, &m_frameState));
		if (!m_frameState.shouldRender) return XR_ERROR_RUNTIME_FAILURE;

		XR_ASSERT(xrBeginFrame(m_session, nullptr));

		uint32_t swapchainImage;
		XR_ASSERT(xrAcquireSwapchainImage(m_swapchain, nullptr, &swapchainImage));
		m_frame.swapChainImage = m_swapchainTextures[swapchainImage];

		XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO, nullptr, XR_INFINITE_DURATION};
		XR_ASSERT(xrWaitSwapchainImage(m_swapchain, &waitInfo));

		return XR_SUCCESS;
	}

	XrResult updatePoses() {

		XrViewState viewState{XR_TYPE_VIEW_STATE};
		XrView views[2]{{XR_TYPE_VIEW}, {XR_TYPE_VIEW}};
		uint32_t n;
		m_viewLocateInfo.displayTime = m_frameState.predictedDisplayTime;
		XR_ASSERT(xrLocateViews(m_session, &m_viewLocateInfo, &viewState, 2, &n, views));
		SGD_ASSERT(n == 2);

		// Update eye states
		//
		for (int eye = 0; eye < 2; ++eye) {

			auto& view = views[eye];
			auto& projView = m_projViews[eye];
			projView.fov = view.fov;

			auto flags = viewState.viewStateFlags;
			if (flags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) projView.pose.orientation = view.pose.orientation;
			if (flags & XR_VIEW_STATE_POSITION_VALID_BIT) projView.pose.position = view.pose.position;

			float zNear = .01f;
			float zFar = 100.0f;

			auto& fov = projView.fov;

			m_frame.viewerPose.views[eye].projection =
				projectionMatrix(fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, zNear, zFar);
			m_frame.viewerPose.views[eye].transform = poseMatrix(projView.pose);
			m_frame.viewerPose.views[eye].viewport = m_viewports[eye];
		}

		// Update controller states
		//
		XR_ASSERT(xrSyncActions(m_session, &m_actionsSyncInfo));

		for (int hand = 0; hand < 2; ++hand) {

			XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
			XR_ASSERT(xrLocateSpace(m_aimPoseSpaces[hand], m_localSpace, m_frameState.predictedDisplayTime, &location));

			auto flags = location.locationFlags;

			if (flags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) m_aimPoses[hand].orientation = location.pose.orientation;
			if (flags & XR_SPACE_LOCATION_POSITION_VALID_BIT) m_aimPoses[hand].position = location.pose.position;

			m_frame.handPoses[hand].transform = poseMatrix(m_aimPoses[hand]);
		}

		return XR_SUCCESS;
	}

	wgpu::Device m_device{};
	XrInstance m_instance{};
	XrSystemId m_systemId{};

	XrSession m_session{};
	XrSpace m_localSpace{};
	XRFrame m_frame{};

	Vec2u m_swapchainTextureSize{};
	uint32_t m_swapchainSampleCount{};
	int64_t m_swapchainTextureFormat{};
	XrSwapchain m_swapchain{};
	Vector<wgpu::Texture> m_swapchainTextures{};

	XrCompositionLayerProjectionView m_projViews[2]{};
	Recti m_viewports[2];
	XrViewLocateInfo m_viewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
	XrCompositionLayerProjection m_projLayer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
	XrCompositionLayerBaseHeader* m_layers[2]{};
	XrFrameEndInfo m_frameEndInfo{XR_TYPE_FRAME_END_INFO};

	XrPassthroughHTC m_passthrough{};
	bool m_passthroughEnabled{};
	XrCompositionLayerPassthroughHTC m_passthroughLayer{};

	bool m_ready = false;
	bool m_rendering = false;
	XrFrameState m_frameState{XR_TYPE_FRAME_STATE};
	XRViewerPose m_viewerPose{};
	Array<XRHandPose, 2> m_handPoses{};

	XrActionSet m_actionSet{};
	XrAction m_aimPoseAction{};
	XrAction m_gripPoseAction{};
	XrSpace m_aimPoseSpaces[2]{};
	XrSpace m_gripPoseSpaces[2]{};
	XrPosef m_aimPoses[2]{};
	XrPosef m_gripPoses[2]{};
	XrActiveActionSet m_activeActionSets = {{}, XR_NULL_PATH};
	XrActionsSyncInfo m_actionsSyncInfo = {XR_TYPE_ACTIONS_SYNC_INFO, nullptr, 1, &m_activeActionSets};
};

wgpu::Device createXRCompatibleWGPUDevice() {

	if (XR_FAILED(createXrInstance())) return {};

	wgpu::RequestAdapterOptions adapterOpts{};
	adapterOpts.backendType = defaultBackendType;

	wgpu::ChainedStruct* opts;
	dawnxr::createRequestAdapterOptions(getXrInstance(), getXrSystemId(), adapterOpts.backendType, &opts);

	adapterOpts.nextInChain = opts;

	return createWGPUDevice(adapterOpts);
}

XRSession* createXRSession(const wgpu::Device& device) {

	XR_ASSERT(createXrInstance());

	return new XRSessionImpl(device, getXrInstance(), getXrSystemId());
}

} // namespace sgd
