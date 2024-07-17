cmake_minimum_required(VERSION 3.16)

##### scene #####

project(scene)

add_library(scene

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

	rendereffect/rendereffect.cpp
	rendereffect/rendereffect.h
	rendereffect/grayscaleeffect.cpp
	rendereffect/grayscaleeffect.h
	rendereffect/grayscaleeffect.wgsl

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

	skybox/skybox.cpp
	skybox/skybox.h
	skybox/skyboxrenderer.cpp
	skybox/skyboxrenderer.h

	sprite/sprite.cpp
	sprite/sprite.h
	sprite/spriterenderer.cpp
	sprite/spriterenderer.h

	exports.h
)

target_link_libraries(scene graphics tinygltf)
 
add_subdirectory(tests)
