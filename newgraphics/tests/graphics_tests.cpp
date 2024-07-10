#include <graphics/exports.h>

#include <thread>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
SceneBindingsPtr sceneBindings;
RenderContextPtr renderContext;

SkyboxRendererPtr skyboxGeometry;

MeshPtr meshGeometry;
MeshRendererPtr meshRenderer;

void render() {

	auto inst = meshRenderer->lockInstances(1);

	inst->worldMatrix = AffineMat4f::TRS({0, 0, 2});
	inst->color = Vec4f(1,.5,0,1);

	meshRenderer->unlockInstances();

	runOnMainThread([]{

		GraphicsResource::validateAll();

		renderContext->beginRender();
		{
			renderContext->beginRenderPass(RenderPassType::opaque, gc->colorBuffer(), gc->depthBuffer(), {}, 1);
			renderContext->wgpuRenderPassEncoder().SetBindGroup(0, sceneBindings->bindGroup()->wgpuBindGroup());
			{ //
				skyboxGeometry->render(renderContext);
				meshRenderer->render(renderContext);
			}
			renderContext->endRenderPass();
		}
		renderContext->endRender();
	});

	gc->present(gc->colorBuffer());
}

int main() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) { render(); });

	gc = new GraphicsContext(window, wgpu::BackendType::Undefined);

	sceneBindings = new SceneBindings();
	{

		CameraUniforms uniforms;
		float near = .1f, far = 100.0f;
		uniforms.projectionMatrix = Mat4f::frustum(-near, near, -near, near, near, far);
		uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
		uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
		uniforms.clipNear = near;
		uniforms.clipFar = far;
		sceneBindings->setCameraUniforms(uniforms);
	}
	
	{
		LightingUniforms uniforms;
		uniforms.directionalLightCount=1;
		sceneBindings->setLightingUniforms(uniforms);
	};

	renderContext = new RenderContext();

	auto skyTexture = loadTexture(Path("sgd://envmaps/grimmnight-cube.jpg"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();

	skyboxGeometry = new SkyboxRenderer();
	skyboxGeometry->skyTexture = skyTexture;

	MaterialPtr material = loadPBRMaterial(Path("sgd://materials/Fabric048_1K-JPG")).result();

	meshGeometry = createSphereMesh(.5f, 48, 24, material);

	meshRenderer = new MeshRenderer(meshGeometry);

	for (;;) {
		pollEvents();
		render();
	}
}
