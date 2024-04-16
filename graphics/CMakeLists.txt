cmake_minimum_required(VERSION 3.16)

##### graphics #####

project(graphics)

add_library(graphics
	animation.cpp
	animation.h
	bindgroup.cpp
	bindgroup.h
	buffer.cpp
	buffer.h
	camera.cpp
	camera.h
	drawlist.cpp
	drawlist.h
	entity.cpp
	entity.h
	entityutil.cpp
	entityutil.h
	font.cpp
	font.h
	gltfloader.cpp
	gltfloader.h
	graphics.cpp
	graphics.h
	light.cpp
	light.h
	material.cpp
	material.h
	materialutil.cpp
	materialutil.h
	mesh.cpp
	mesh.h
	meshprimitives.cpp
	meshprimitives.h
	meshrenderer.cpp
	meshrenderer.h
	meshutil.cpp
	meshutil.h
	model.cpp
	model.h
	modelrenderer.cpp
	modelrenderer.h
	overlay.cpp
	overlay.h
	pbrmaterial.cpp
	pbrmaterial.h
	prelitmaterial.cpp
	prelitmaterial.h
	rendercontext.cpp
	rendercontext.h
	renderer.cpp
	renderer.h
	renderop.cpp
	renderop.h
	renderpipeline.cpp
	renderpipeline.h
	scene.cpp
	scene.h
	scenebindings.cpp
	scenebindings.h
	shadowmaterial.cpp
	shadowmaterial.h
	skinnedmeshrenderer.cpp
	skinnedmeshrenderer.h
	skinnedmodelrenderer.cpp
	skinnedmodelrenderer.h
	skybox.cpp
	skybox.h
	skyboxrenderer.cpp
	skyboxrenderer.h
	sprite.cpp
	sprite.h
	spriterenderer.cpp
	spriterenderer.h
	texture.cpp
	texture.h
	textureutil.cpp
	textureutil.h

	shaders/drawlistrenderer.wgsl
	shaders/meshrenderer.wgsl
	shaders/pbrmaterial.wgsl
	shaders/prelitmaterial.wgsl
	shaders/scene.wgsl
	shaders/skinnedmeshrenderer.wgsl
	shaders/skyboxrenderer.wgsl
	shaders/spriterenderer.wgsl
	shaders/uniforms.h

	exports.h
)

target_link_libraries(graphics window dawn tinygltf stb)

add_subdirectory(tests)
