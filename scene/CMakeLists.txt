cmake_minimum_required(VERSION 3.16)

##### scene #####

project(scene)

add_library(scene

	audio/audioemitter.cpp
	audio/audioemitter.h
	audio/audiolistener.cpp
	audio/audiolistener.h
	audio/audiospace.cpp
	audio/audiospace.h

	model/animation.cpp
	model/animation.h
	model/gltfloader.cpp
	model/gltfloader.h
	model/model.cpp
	model/model.h
	model/modelrenderer.cpp
	model/modelrenderer.h
	model/skinnedmodelrenderer.cpp
	model/skinnedmodelrenderer.h

	overlay/overlay.cpp
	overlay/overlay.h
	overlay/overlayrenderer.cpp
	overlay/overlayrenderer.h

	physics/bvhtree.cpp
	physics/bvhtree.h
	physics/collider.cpp
	physics/collider.h
	physics/collisionspace.cpp
	physics/collisionspace.h
	physics/collisionutil.cpp
	physics/collisionutil.h
	physics/contact.cpp
	physics/contact.h
	physics/ellipsoidcollider.cpp
	physics/ellipsoidcollider.h
	physics/intersect.cpp
	physics/intersect.h
	physics/meshcollider.cpp
	physics/meshcollider.h
	physics/meshcolliderdata.cpp
	physics/meshcolliderdata.h
	physics/spherecollider.cpp
	physics/spherecollider.h

	rendereffect/bloomeffect.cpp
	rendereffect/bloomeffect.h
	rendereffect/bloomeffect.wgsl
	rendereffect/blureffect.cpp
	rendereffect/blureffect.h
	rendereffect/blureffect.wgsl
	rendereffect/fogeffect.cpp
	rendereffect/fogeffect.h
	rendereffect/fogeffect.wgsl
	rendereffect/gaussianfilter.cpp
	rendereffect/gaussianfilter.h
	rendereffect/rendereffect.cpp
	rendereffect/rendereffect.h
	rendereffect/monocoloreffect.cpp
	rendereffect/monocoloreffect.h
	rendereffect/monocoloreffect.wgsl

	scene/camera.cpp
	scene/camera.h
	scene/entity.cpp
	scene/entity.h
	scene/entityutil.cpp
	scene/entityutil.h
	scene/light.cpp
	scene/light.h
	scene/scene.cpp
	scene/scene.h
	scene/scenerenderer.cpp
	scene/scenerenderer.h

	serialize/serialize.cpp
	serialize/serialize.h

	skybox/skybox.cpp
	skybox/skybox.h
	skybox/skyboxrenderer.cpp
	skybox/skyboxrenderer.h

	sprite/sprite.cpp
	sprite/sprite.h
	sprite/spriterenderer.cpp
	sprite/spriterenderer.h

	terrain/terrain.cpp
	terrain/terrain.h
	terrain/terrainrenderer.cpp
	terrain/terrainrenderer.h

	exports.h
)

target_link_libraries(scene graphics audio tinygltf)

add_subdirectory(tests)
