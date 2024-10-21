cmake_minimum_required(VERSION 3.16)

##### graphics #####

project(graphics)

add_library(graphics

    exports.h

	core/bindgroup.cpp
	core/bindgroup.h
	core/buffer.cpp
	core/buffer.h
	core/float16.cpp
	core/float16.h
	core/graphics.cpp
	core/graphics.h
	core/texture.cpp
	core/texture.h
	core/texturedata.cpp
	core/texturedata.h
	core/textureutil.cpp
	core/textureutil.h
    
	drawlist/drawlist.cpp
	drawlist/drawlist.h
	drawlist/drawlistrenderer.wgsl
	drawlist/font.cpp
	drawlist/font.h
    drawlist/fontmaterial.wgsl
    
	image/image.cpp
    image/image.h
	image/imagebindgroup.cpp
	image/imagebindgroup.h
	image/imagematerial.cpp
	image/imagematerial.h
	image/imagematerial.wgsl
	image/imagerenderer.cpp
	image/imagerenderer.h
	image/imagerenderer.wgsl
	image/imageutil.cpp
	image/imageutil.h

	material/emissivematerial.cpp
	material/emissivematerial.h
	material/emissivematerial.wgsl
	material/material.cpp
	material/material.h
	material/materialutil.cpp
	material/materialutil.h
	material/nullmaterial.cpp
	material/nullmaterial.h
	material/nullmaterial.wgsl
	material/pbrmaterial.cpp
	material/pbrmaterial.h
	material/pbrmaterial.wgsl
	material/prelitmaterial.cpp
	material/prelitmaterial.h
	material/prelitmaterial.wgsl
    
	mesh/mesh.cpp
	mesh/mesh.h
	mesh/meshprimitives.cpp
	mesh/meshprimitives.h
	mesh/meshrenderer.cpp
	mesh/meshrenderer.h
    mesh/meshrenderer.wgsl
	mesh/meshutil.cpp
	mesh/meshutil.h
	mesh/skinnedmeshrenderer.cpp
	mesh/skinnedmeshrenderer.h
    mesh/skinnedmeshrenderer.wgsl
	mesh/surface.cpp
	mesh/surface.h
	mesh/vertex.cpp
	mesh/vertex.h

	plane/planebindgroup.cpp
	plane/planebindgroup.h
	plane/planebindings.cpp
	plane/planebindings.h
	plane/planerenderer.wgsl

	render/rendercontext.cpp
	render/rendercontext.h
	render/renderop.cpp
	render/renderop.h
	render/renderpipeline.cpp
	render/renderpipeline.h
	render/renderqueue.cpp
	render/renderqueue.h
    render/scene.wgsl
	render/scenebindgroup.cpp
	render/scenebindgroup.h
	render/scenebindings.cpp
	render/scenebindings.h

	skybox/skyboxbindgroup.cpp
	skybox/skyboxbindgroup.h
	skybox/skyboxbindings.cpp
	skybox/skyboxbindings.h
	skybox/skyboxrenderer.wgsl

	terrain/terrainbindgroup.cpp
	terrain/terrainbindgroup.h
	terrain/terrainbindings.cpp
	terrain/terrainbindings.h
	terrain/terrainrenderer.wgsl
)

target_link_libraries(graphics window dawn tinyexr stb tcpp)

add_subdirectory(tests)
