cmake_minimum_required(VERSION 3.16)

##### scene #####

project(scene)

add_library(scene
	animation.cpp
	animation.h
	camera.cpp
	camera.h
	collider.cpp
	collider.h
	collisionspace.cpp
	collisionspace.h
	collisionutil.cpp
	collisionutil.h
	contact.cpp
	contact.h
	ellipsoidcollider.cpp
	ellipsoidcollider.h
	entity.cpp
	entity.h
	entityutil.cpp
	entityutil.h
	gltfloader.cpp
	gltfloader.h
	intersect.cpp
	intersect.h
	light.cpp
	light.h
	meshcollider.cpp
	meshcollider.h
	meshcolliderdata.cpp
	meshcolliderdata.h
	model.cpp
	model.h
	modelrenderer.cpp
	modelrenderer.h
	overlay.cpp
	overlay.h
	scene.cpp
	scene.h
	skinnedmodelrenderer.cpp
	skinnedmodelrenderer.h
	skybox.cpp
	skybox.h
	spherecollider.cpp
	spherecollider.h
	sprite.cpp
	sprite.h
	spriterenderer.cpp
	spriterenderer.h

	exports.h
)

target_link_libraries(scene graphics)
 
add_subdirectory(tests)
