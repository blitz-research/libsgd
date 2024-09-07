cmake_minimum_required(VERSION 3.16)

##### skirmish #####

project(skirmish)

add_executable(skirmish
	actor.cpp
	actor.h
	bullet.cpp
	bullet.h
	game.cpp
	game.h
	ground.cpp
	ground.h
	main.cpp
	player.cpp
	player.h
	saucer.cpp
	saucer.h
	std.cpp
	std.h
	tower.cpp
	tower.h
	world.cpp
	world.h
)

target_link_libraries(skirmish libsgd)
