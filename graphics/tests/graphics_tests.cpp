#include <graphics/exports.h>

#include <thread>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
SceneBindingsPtr sceneBindings;
RenderContextPtr renderContext;
SkyboxRendererPtr skyboxRenderer;

void render() {

	GraphicsResource::validateAll(gc);

	renderContext->beginRender();
	{
		renderContext->beginRenderPass(RenderPassType::clear, gc->colorBuffer(), gc->depthBuffer(), {}, 1);
		renderContext->wgpuRenderPassEncoder().SetBindGroup(0, sceneBindings->bindGroup()->wgpuBindGroup());
		{ //
			skyboxRenderer->render(renderContext);
		}
		renderContext->endRenderPass();
	}
	renderContext->endRender();

	gc->present(gc->colorBuffer());
}

int main() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) { render(); });

	gc = new GraphicsContext(window, wgpu::BackendType::Undefined);

	sceneBindings = new SceneBindings();

	CameraUniforms cameraUniforms;
	cameraUniforms.projectionMatrix = Mat4f::frustum(-.1f,.1f,-.1f,.1f,.1f,100);
	cameraUniforms.inverseProjectionMatrix = inverse(cameraUniforms.projectionMatrix);
	cameraUniforms.viewProjectionMatrix = cameraUniforms.projectionMatrix * cameraUniforms.viewMatrix;
	cameraUniforms.clipNear=.1f;
	cameraUniforms.clipFar=.100;
	sceneBindings->updateCameraUniforms(cameraUniforms);

	renderContext = new RenderContext(gc);

	auto skyTexture = loadTexture(Path("sgd://envmaps/grimmnight-cube.jpg"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();

	skyboxRenderer = new SkyboxRenderer();
	skyboxRenderer->skyTexture = skyTexture;

	for (;;) {
		pollEvents();
		render();
	}
}
