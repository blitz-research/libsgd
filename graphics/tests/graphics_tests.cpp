#include <graphics/exports.h>

#include <thread>

#define SKYBOX 1
#define TERRAIN 1
#define MESH 1
#define IMAGE 1
#define DRAWLIST 1

using namespace sgd;

WindowPtr window;

TexturePtr renderTarget;
TexturePtr depthBuffer;

SceneBindingsPtr sceneBindings;
RenderContextPtr renderContext;
RenderQueuePtr renderQueue;

#if SKYBOX
SkyboxBindingsPtr skyboxBindings;
#endif

#if TERRAIN
TerrainBindingsPtr terrainBindings;
#endif

#if MESH
MeshPtr mesh;
MeshRendererPtr meshRenderer;
#endif

#if DRAWLIST
DrawListPtr drawList;
#endif

#if IMAGE
ImagePtr image;
ImageRendererPtr imageRenderer;
#endif

void render() {

#if MESH
	{
		auto inst = meshRenderer->lockInstances(1);
		inst->worldMatrix = AffineMat4f::TRS({0, 0, 2});
		inst->color = Vec4f(1, .5, 0, 1);
		meshRenderer->unlockInstances();
	}
#endif
#if IMAGE
	{
		auto instp = imageRenderer->lockInstances(1);
		instp->worldMatrix = AffineMat4f::TRS({0, 0, 1});
		instp->color = Vec4f(1);
		instp->frame = 0;
		imageRenderer->unlockInstances();
	}
#endif
#if DRAWLIST
	{
		drawList->clear();
		drawList->projectionMatrix = Mat4f::ortho(0, 1280, 960, 0, 0, 1);
		drawList->addText("Hello World!", {0, 0});
		drawList->fillColor = Vec4f(1, .5, 0, 1);
		drawList->addRect({0, 16, 1920, 32});
		drawList->flush();
	}
#endif

	runOnMainThread([] {
		auto gc = currentGC();

		GraphicsResource::validateAll();

		renderQueue->clear();

#if SKYBOX
		skyboxBindings->render(renderQueue);
#endif
#if TERRAIN
		terrainBindings->render(renderQueue);
#endif
#if MESH
		meshRenderer->render(renderQueue, mesh, 1, 0);
#endif
#if IMAGE
		imageRenderer->render(renderQueue, image, 0, 1);
#endif
#if DRAWLIST
		drawList->render(renderQueue);
#endif

		renderContext->beginRender();
		{
			renderContext->beginRenderPass(RenderPassType::opaque, renderTarget, depthBuffer, Vec4f(1, .25, 0, 1), 1,
										   sceneBindings->bindGroup());

			renderContext->render(renderQueue->renderOps(RenderPassType::opaque));

			renderContext->endRenderPass();
		}
		{
			renderContext->beginRenderPass(RenderPassType::blend, renderTarget, depthBuffer, {}, 1, sceneBindings->bindGroup());

			renderContext->render(renderQueue->renderOps(RenderPassType::blend));

			renderContext->endRenderPass();
		}
		renderContext->endRender();

		gc->present(renderTarget);
	});
}

int main() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	auto resize = [](CVec2u size) {
		if (renderTarget && renderTarget->size() == size) return;
		renderTarget = new Texture(size, 1, TextureFormat::rgba16f,
								   TextureFlags::renderTarget | TextureFlags::filter | TextureFlags::clamp);
		depthBuffer = new Texture(size, 1, TextureFormat::depth32f, TextureFlags::renderTarget);
	};

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [=](CVec2u size) {
		resize(size);
		render();
	});
	resize(window->size());

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
	auto skyTexture = loadCubeTexture(Path("sgd://envmaps/stormy-cube.jpg"), TextureFormat::srgba8, TextureFlags::mipmap).result();
	skyboxBindings = new SkyboxBindings();
	skyboxBindings->skyTexture = skyTexture;
#endif

#if TERRAIN
	terrainBindings = new TerrainBindings();
#endif

#if MESH
	MaterialPtr material = loadPBRMaterial(Path("sgd://materials/Fabric048_1K-JPG")).result();
	mesh = createSphereMesh(.5f, 48, 24, material);
	meshRenderer = new MeshRenderer();
#endif

#if DRAWLIST
	drawList = new DrawList();
#endif

#if IMAGE
	image = loadImage(Path("sgd://misc/light.png")).result();
	imageRenderer = new ImageRenderer();
#endif

	for (;;) {
		pollEvents();
		render();
	}
}
