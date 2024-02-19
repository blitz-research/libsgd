#include <graphics/exports.h>

#include <graphics/shaders/uniforms.h>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;

TexturePtr colorBuffer;
TexturePtr depthBuffer;
MeshRendererPtr meshRenderer;

CameraUniforms camera;
LightingUniforms lighting;

#if 0
MaterialPtr loadMaterial(Path path) {

	String prefix = path.path() + "/" + path.path();

	MaterialPtr material = new Material(&pbrMaterialDescriptor);

	material->setTexture("albedoTexture",
						 loadTexture(Path(prefix + "_Color.jpg"), TextureFormat::srgba8, TextureFlags::filter).result());

	if (pathExists(Path(prefix + "_Metalness.jpg"))) {
		material->setTexture("metallicTexture",
							 loadTexture(Path(prefix + "_Metalness.jpg"), TextureFormat::rgba8, TextureFlags::filter).result());
		material->setFloat("metallicFactor1f", 1);
	}

	if (pathExists(Path(prefix + "_Roughness.jpg"))) {
		material->setTexture("roughnessTexture",
							 loadTexture(Path(prefix + "_Roughness.jpg"), TextureFormat::rgba8, TextureFlags::filter).result());
		material->setFloat("roughnessFactor1f", 1);
	}

	if (pathExists(Path(prefix + "_AmbientOcclusion.jpg"))) {
		material->setTexture(
			"occlusionTexture",
			loadTexture(Path(prefix + "_AmbientOcclusion.jpg"), TextureFormat::rgba8, TextureFlags::filter).result());
	}

	if (pathExists(Path(prefix + "_NormalGL.jpg"))) {
		material->setTexture("normalTexture",
							 loadTexture(Path(prefix + "_NormalGL.jpg"), TextureFormat::rgba8, TextureFlags::filter).result());
	}

	return material;
}
#endif

void render() {

	static float camRot;
	camera.worldMatrix = Mat4f(AffineMat4f::TRS({0, 0, -3}));

	camera.viewMatrix = inverse(camera.worldMatrix);
	camera.projectionMatrix = Mat4f::perspective(45, (float)colorBuffer->size().x / (float)colorBuffer->size().y, .1, 100);
	camera.inverseProjectionMatrix = inverse(camera.projectionMatrix);
	camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
	gc->bindGroup0()->getBuffer(0)->update(&camera, 0, sizeof(camera));

	gc->bindGroup0()->getBuffer(1)->update(&lighting, 0, sizeof(lighting));

	{
		static float meshRot;
		auto insts = meshRenderer->lockInstances(1);
		insts[0].matrix = Mat4f(AffineMat4f::TRS({0, 0, 0}, {0, meshRot += .0002, 0}));
		insts[0].color = {1, 1, 1, 1};
		meshRenderer->unlockInstances();
	}

	gc->beginRender(colorBuffer, depthBuffer, {0, 0, 0, 1}, 1);
	{
		gc->beginRenderPass(RenderPass::clear);
		gc->endRenderPass();

		gc->beginRenderPass(RenderPass::opaque);
		meshRenderer->render(gc);
		gc->endRenderPass();
	}
	gc->endRender();

	gc->present(colorBuffer);
}

void rrender() {
	if (!window->pollEvents()) std::exit(0);

	requestRender(rrender);

	render();
}

void start() {

	{
		window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

		gc = new GraphicsContext(window);

		colorBuffer = new Texture(gc->swapChainSize(), 1, sgd::TextureFormat::rgba16f, sgd::TextureFlags::renderTarget);
		depthBuffer = new Texture(gc->swapChainSize(), 1, sgd::TextureFormat::depth32f, sgd::TextureFlags::renderTarget);

		gc->swapChainSizeChanged.connect(gc, [=](CVec2u size) {
			colorBuffer->resize(size);
			depthBuffer->resize(size);
			render();
		});
	}

	{
		TexturePtr envTexture =
			loadTexture(Path("sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::filter).result();

		gc->bindGroup0()->setTexture(2, envTexture);

		lighting.pointLightCount = 1;
		lighting.ambientLightColor = {1, 1, 1, 0};
		lighting.pointLights[0].position = {2.5, 2.5, -2.5, 1};
		lighting.pointLights[0].color = {1, 1, 1, 1};
		lighting.pointLights[0].falloff = 1;
		lighting.pointLights[0].range = 25;
#if 0
		lighting.pointLights[1].position = {0, 2, -2.5, 1};
		lighting.pointLights[1].color = {0, 1, 0, 1};
		lighting.pointLights[1].falloff = 1;
		lighting.pointLights[1].range = 25;
		lighting.pointLights[2].position = {2, 0, -2.5, 1};
		lighting.pointLights[2].color = {0, 0, 1, 1};
		lighting.pointLights[2].falloff = 1;
		lighting.pointLights[2].range = 25;
#endif
	}

	{
		//MaterialPtr material = loadMaterial(Path("Bricks076C_1K-JPG"));
		MaterialPtr material = loadMaterial(Path("Marble008_1K-JPG"));
		// MaterialPtr material = loadMaterial(Path("Facade001_1K-JPG"));
		//MaterialPtr material = loadMaterial(Path("Facade018A_1K-JPG"));
		//MaterialPtr material = loadMaterial(Path("PavingStones131_1K-JPG"));

		MeshPtr mesh = createSphereMesh(1, 64, 32, material);
		//MeshPtr mesh = createBoxMesh(Boxf(-1, 1), material);

		transformTexCoords(mesh, {4,2}, {0,0});

		meshRenderer = new MeshRenderer(mesh);
		auto insts = meshRenderer->lockInstances(1);
		insts[0].matrix = {};
		insts[0].color = {1, 1, 1, 1};
		meshRenderer->unlockInstances();
	}

	requestRender(rrender);
}

int main() {

	requestWGPUDevice({}, [](const wgpu::Device&) { start(); });

	beginAppEventLoop();
}
