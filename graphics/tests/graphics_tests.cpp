#include <graphics/exports.h>

#include <thread>

#define SKYBOX 1
#define MESH 1
#define DRAWLIST 1

using namespace sgd;

WindowPtr window;
SceneBindingsPtr sceneBindings;
RenderContextPtr renderContext;
RenderQueuePtr renderQueue;

SkyboxGeometryPtr skyboxGeometry;

MeshPtr meshGeometry;
MeshRendererPtr meshRenderer;

DrawListPtr drawList;

void render() {

#if MESH
	auto inst = meshRenderer->lockInstances(1);
	inst->worldMatrix = AffineMat4f::TRS({0, 0, 2});
	inst->color = Vec4f(1,.5,0,1);
	meshRenderer->unlockInstances();
#endif

#if DRAWLIST
	drawList->clear();
	drawList->projectionMatrix = Mat4f::ortho(0, 1280, 960, 0, 0, 1);
	drawList->addText("Hello World!",{0,0});
	drawList->fillColor = Vec4f(1,.5,0,1);
	drawList->addRect({0,16,1920,32});
	drawList->flush();
#endif

	runOnMainThread([] {

		auto gc = currentGC();

		GraphicsResource::validateAll();

		renderQueue->clear();

#if SKYBOX
		skyboxGeometry->render(renderQueue);
#endif

#if MESH
		meshRenderer->render(renderQueue);
#endif

#if DRAWLIST
		drawList->render(renderQueue);
#endif

		renderContext->beginRender();
		{
			renderContext->beginRenderPass(RenderPassType::opaque, gc->colorBuffer(), gc->depthBuffer(), Vec4f(1,.25,0,1), 1,
										   sceneBindings->bindGroup()->wgpuBindGroup());

			renderContext->render(renderQueue->renderOps(RenderPassType::opaque));

			renderContext->endRenderPass();
		}
		{
			renderContext->beginRenderPass(RenderPassType::blend, gc->colorBuffer(), gc->depthBuffer(), {}, 1,
										   sceneBindings->bindGroup()->wgpuBindGroup());

			renderContext->render(renderQueue->renderOps(RenderPassType::blend));

			renderContext->endRenderPass();
		}
		renderContext->endRender();

		gc->present(gc->colorBuffer());
	});
}

int main() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });
	window->sizeChanged.connect(nullptr, [](CVec2u) { render(); });

	createGC(window);

	sceneBindings = new SceneBindings();

	{
		float near = .125f, far = 1024.0f;
		auto& uniforms = sceneBindings->lockCameraUniforms();
		uniforms.projectionMatrix = Mat4f::frustum(-near, near, -near, near, near, far);
		uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
		uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
		uniforms.clipNear = near;
		uniforms.clipFar = far;
		sceneBindings->unlockCameraUniforms();
	}

	{
		auto& uniforms = sceneBindings->lockLightingUniforms();
		uniforms.numDirectionalLights = 1;
		sceneBindings->unlockLightingUniforms();
	}

	renderContext = new RenderContext();

	renderQueue = new RenderQueue();

#if SKYBOX
	auto skyTexture = loadTexture(Path("sgd://envmaps/grimmnight-cube.jpg"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();
	skyboxGeometry = new SkyboxGeometry();
	skyboxGeometry->skyTexture = skyTexture;
#endif

#if MESH
	MaterialPtr material = loadPBRMaterial(Path("sgd://materials/Fabric048_1K-JPG")).result();
	meshGeometry = createSphereMesh(.5f, 48, 24, material);
	meshRenderer = new MeshRenderer(meshGeometry);
#endif

#if DRAWLIST
	drawList = new DrawList();
#endif

	for (;;) {
		pollEvents();
		render();
	}
}
