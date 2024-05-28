cmake_minimum_required(VERSION 3.16)

##### graphics #####

project(graphics)

add_library(graphics
	bindgroup.cpp
	bindgroup.h
	buffer.cpp
	buffer.h
	drawlist.cpp
	drawlist.h
	font.cpp
	font.h
	graphics.cpp
	graphics.h
	image.cpp
	image.h
	imageutil.cpp
	imageutil.h
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
	scenebindings.cpp
	scenebindings.h
	shadowmaterial.cpp
	shadowmaterial.h
	skinnedmeshrenderer.cpp
	skinnedmeshrenderer.h
	skyboxrenderer.cpp
	skyboxrenderer.h
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

target_link_libraries(graphics window dawn tinygltf stb tcpp)

add_subdirectory(tests)
