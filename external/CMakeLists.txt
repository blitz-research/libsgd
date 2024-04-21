cmake_minimum_required(VERSION 3.16)

project(external)

include(FetchContent)

# set(FETCHCONTENT_QUIET OFF)

# Do the scary one first in case it fails...
#
##### target: lib-dawn #####

add_library(lib-dawn INTERFACE)

if (SGD_OS_EMSCRIPTEN)

	target_link_options(lib-dawn INTERFACE -sUSE_WEBGPU=1)

else ()

	FetchContent_Declare(dawn
		GIT_REPOSITORY https://github.com/blitz-research/dawn.git
		GIT_TAG openxr-dev
		GIT_SUBMODULES ""
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)

	set(DAWN_FETCH_DEPENDENCIES ON)
	set(DAWN_BUILD_SAMPLES OFF)
	set(DAWN_USE_GLFW OFF)

	set(TINT_BUILD_CMD_TOOLS OFF)
	set(TINT_BUILD_TESTS OFF)

    message("### Configuring Dawn...")
	FetchContent_MakeAvailable(dawn)

	target_link_libraries(lib-dawn INTERFACE webgpu_dawn webgpu_cpp)

	target_include_directories(lib-dawn INTERFACE ${DAWN_INCLUDE_DIR})

endif ()

# Less scary but takes ages to config
#
##### target: libcurl #####

if (NOT SGD_OS_EMSCRIPTEN)

	FetchContent_Declare(libcurl
		GIT_REPOSITORY https://github.com/blitz-research/curl.git
		GIT_TAG master	#ea7322ed14e26eb9ea715f4637b28fdecbefe2a1
		GIT_SUBMODULES ""
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)

	set(BUILD_CURL_EXE OFF)
	set(BUILD_SHARED_LIBS OFF)
	set(CURL_DISABLE_TESTS ON)

	if (SGD_OS_WINDOWS)
		set(CURL_USE_SCHANNEL ON)
	else ()
		set(CURL_USE_OPENSSL ON)
		set(CURL_DISABLE_ALTSVC ON) # Missing header?
	endif ()

	message("### Configuring Curl...")
	FetchContent_MakeAvailable(libcurl)

endif ()

##### json11 #####

FetchContent_Declare(json11
	GIT_REPOSITORY https://github.com/blitz-research/json11.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(json11)

##### stb #####

add_library(stb INTERFACE)

FetchContent_Declare(stb
	GIT_REPOSITORY https://github.com/blitz-research/stb.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(stb)

##### glfw #####

if (SGD_OS_EMSCRIPTEN)

	add_library(glfw INTERFACE)

	target_link_options(glfw INTERFACE -sUSE_GLFW=3)

else ()

	FetchContent_Declare(glfw
		GIT_REPOSITORY https://github.com/blitz-research/glfw.git
		GIT_TAG master
		GIT_SUBMODULES ""
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)

	FetchContent_MakeAvailable(glfw)

endif ()

target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

##### tinygltf #####

FetchContent_Declare(tinygltf
	GIT_REPOSITORY https://github.com/blitz-research/tinygltf.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

#set(TINYGLTF_HEADER_ONLY ON)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)

FetchContent_MakeAvailable(tinygltf)

##### imgui #####

FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/blitz-research/imgui.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(imgui)

# imgui-core used by demo only
add_library(imgui-core
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui-core PUBLIC ${imgui_SOURCE_DIR})

target_link_libraries(imgui-core lib-dawn glfw)

# imgui-backends used by dynamic lib builds
add_library(imgui-backends
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp
)
target_include_directories(imgui-backends PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

target_link_libraries(imgui-backends lib-dawn glfw)
