cmake_minimum_required(VERSION 2.8.9)
project (game C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_ZLIB=1")
else()
	if (${CMAKE_GENERATOR} MATCHES Xcode)
		# set(LIBRARY_INPUT[variant=Debug] "${IOS_LIBRARY_PREIX}/debug")
		# set(LIBRARY_INPUT[variant=Release] "${IOS_LIBRARY_PREIX}/release")
		# set(LIBRARY_INPUT[variant=MinSizeRel] "${IOS_LIBRARY_PREIX}/release")
		# set(LIBRARY_INPUT[variant=RelWithDebInfo] "${IOS_LIBRARY_PREIX}/release")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
	endif()
endif()

include_directories(src)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	set(OPENGL_LINK GLESv3 EGL z)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if (${CMAKE_GENERATOR} MATCHES Xcode)
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIRS})
		set(OPENGL_LINK ${OPENGL_LIBRARY} z bz2)
	else()
		include_directories(/System/Library/Frameworks)
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIRS})
		set(OPENGL_LINK ${OPENGL_LIBRARY} z bz2)
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIRS})
        set(OPENGL_LINK ${OPENGL_LIBRARY})
else()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIRS})
		set(OPENGL_LINK ${OPENGL_LIBRARY} z)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(game STATIC ${SOURCES})
target_link_libraries(game m pthread ssl crypto)

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
	add_custom_command(
		TARGET game
		POST_BUILD
		COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libgame.a
	)
endif()

install (TARGETS game
	ARCHIVE DESTINATION .)

if(NOT ${CMAKE_GENERATOR} MATCHES Xcode
	AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten"
	AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")

	if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 --use-preload-plugins --preload-file ../../../data@")
		set(OPENGL_APPLICATION )
    else()
            set(OPENGL_APPLICATION SDL2)
    endif()

    add_executable(test "test/main.c")
    add_dependencies(test game ${OPENGL_APPLICATION})
	target_link_libraries(test game ${OPENGL_APPLICATION})
endif()