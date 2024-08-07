cmake_minimum_required(VERSION 3.16)

project(external)

include(FetchContent)

# set(FETCHCONTENT_QUIET OFF)

# Do the big scary one first in case it fails...
#
##### target: lib-dawn #####

message("### Configuring Dawn...")

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
	set(DAWN_BUILD_MONOLITHIC_LIBRARY OFF)
	set(DAWN_FETCH_DEPENDENCIES ON)
	set(DAWN_BUILD_SAMPLES OFF)
	set(DAWN_USE_GLFW OFF)

	set(TINT_BUILD_CMD_TOOLS OFF)
	set(TINT_BUILD_TESTS OFF)

	FetchContent_MakeAvailable(dawn)

	target_link_libraries(lib-dawn INTERFACE
		#webgpu_dawn
		webgpu_cpp
		dawn::dawn_proc
		dawn::dawn_native
	)

	target_include_directories(lib-dawn INTERFACE ${DAWN_INCLUDE_DIR})

endif ()

# Less scary but takes ages to config
#
##### target: libcurl #####

message("### Configuring Curl...")

if (NOT SGD_OS_EMSCRIPTEN)

	FetchContent_Declare(libcurl
		GIT_REPOSITORY https://github.com/blitz-research/curl.git
		GIT_TAG master #ea7322ed14e26eb9ea715f4637b28fdecbefe2a1
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

	FetchContent_MakeAvailable(libcurl)

endif ()

##### miniaudio #####

add_library(miniaudio INTERFACE)

FetchContent_Declare(miniaudio
	GIT_REPOSITORY https://github.com/blitz-research/miniaudio.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(miniaudio)

target_include_directories(miniaudio INTERFACE ${miniaudio_SOURCE_DIR})

##### soloud #####

FetchContent_Declare(soloud
	GIT_REPOSITORY https://github.com/blitz-research/soloud.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(soloud)

set(CORE ${soloud_SOURCE_DIR}/src/core)
set(BACKEND ${soloud_SOURCE_DIR}/src/backend)
set(ASRC ${soloud_SOURCE_DIR}/src/audiosource)

set(SRCS
	${CORE}/soloud.cpp
	${CORE}/soloud_core_basicops.cpp
	${CORE}/soloud_core_setters.cpp
	${CORE}/soloud_fft.cpp
	${CORE}/soloud_misc.cpp
	${CORE}/soloud_audiosource.cpp
	${CORE}/soloud_core_faderops.cpp
	${CORE}/soloud_core_voicegroup.cpp
	${CORE}/soloud_fft_lut.cpp
	${CORE}/soloud_queue.cpp
	${CORE}/soloud_bus.cpp
	${CORE}/soloud_core_filterops.cpp
	${CORE}/soloud_core_voiceops.cpp
	${CORE}/soloud_file.cpp
	${CORE}/soloud_thread.cpp
	${CORE}/soloud_core_3d.cpp
	${CORE}/soloud_core_getters.cpp
	${CORE}/soloud_fader.cpp
	${CORE}/soloud_filter.cpp
	${ASRC}/wav/soloud_wav.cpp
	${ASRC}/wav/soloud_wavstream.cpp
	${ASRC}/wav/stb_vorbis.c
	${ASRC}/wav/dr_impl.cpp
)

if (SGD_OS_WINDOWS)
	set(SRCS ${SRCS}
		${BACKEND}/xaudio2/soloud_xaudio2.cpp
		${BACKEND}/winmm/soloud_winmm.cpp
	)
	set(WITH_BACKENDS WITH_XAUDIO2=1 WITH_WINMM=1)
elseif (SGD_OS_LINUX)
	set(SRCS ${SRCS}
		${BACKEND}/alsa/soloud_alsa.cpp
		${BACKEND}/oss/soloud_oss.cpp
	)
	set(LIBS ${LIBS} -lasound)
	set(WITH_BACKENDS WITH_ALSA=1 WITH_OSS=1)
elseif (SGD_OS_MACOS)
	set(SRCS ${SRCS}
		${BACKEND}/coreaudio/soloud_coreaudio.cpp
	)
	set(LIBS ${LIBS} "-framework AudioToolBox" "-framework CoreAudio")
	set(WITH_BACKENDS WITH_COREAUDIO=1)
elseif (SGD_OS_EMSCRIPTEN)
	set(SRCS ${SRCS}
		${BACKEND}/soloud_openal.cpp
	)
	set(WITH_BACKENDS WITH_OPENAL=1)
endif ()

add_library(soloud ${SRCS})

target_compile_definitions(soloud PUBLIC ${WITH_BACKENDS})

target_include_directories(soloud PUBLIC ${soloud_SOURCE_DIR}/include)

target_link_libraries(soloud ${LIBS})

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

target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

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

##### tinygltf #####

FetchContent_Declare(tinygltf
	GIT_REPOSITORY https://github.com/blitz-research/tinygltf.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

set(TINYGLTF_HEADER_ONLY ON)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)

FetchContent_MakeAvailable(tinygltf)

##### tcpp #####

add_library(tcpp INTERFACE)

FetchContent_Declare(tcpp
	GIT_REPOSITORY https://github.com/blitz-research/tcpp.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

block()
set(IS_PLUGIN_BUILDING_ENABLED OFF)
set(IS_SAMPLES_BUILDING_ENABLED OFF)
set(IS_TESTING_ENABLED OFF)
FetchContent_MakeAvailable(tcpp)
endblock()

target_include_directories(tcpp INTERFACE ${tcpp_SOURCE_DIR}/source)

##### imgui #####

FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/blitz-research/imgui.git
	GIT_TAG master
	GIT_SUBMODULES ""
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(imgui)

# imgui-core used by imgui test only
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
