cmake_minimum_required(VERSION 2.8.9)
project (cherry C)

# HARD COMPILE
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	#droid-opengl
	set(OPENGL_LINK GLESv3 EGL z)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if (${CMAKE_GENERATOR} MATCHES Xcode)
		#ios vendor library fixes
		set(VENDOR_LIBRARY[variant=Debug] "${VENDOR_DIRECTORY}/build/ios/debug")
		set(VENDOR_LIBRARY[variant=Release] "${VENDOR_DIRECTORY}/build/ios/release")
		set(VENDOR_LIBRARY[variant=MinSizeRel] "${VENDOR_DIRECTORY}/build/ios/release")
		set(VENDOR_LIBRARY[variant=RelWithDebInfo] "${VENDOR_DIRECTORY}/build/ios/release")

		#ios-opengl
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIRS})
		set(OPENGL_LINK ${OPENGL_LIBRARY} z bz2)
	else()
		#osx-opengl
		include_directories(/System/Library/Frameworks)
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIRS})
		set(OPENGL_LINK ${OPENGL_LIBRARY} z bz2)
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(CMAKE_C_COMPILER "emcc")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_ZLIB=1")
	#emscripten-opengl
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})
	set(OPENGL_LINK ${OPENGL_LIBRARY})
else()
	#linux-opengl
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})
	set(OPENGL_LINK ${OPENGL_LIBRARY} z)
endif()

# ADD VENDOR LIBRARIES
add_library(jpeg STATIC IMPORTED)
set_target_properties(
	jpeg
	PROPERTIES
	IMPORTED_LOCATION "${VENDOR_LIBRARY}/libjpeg.a"
)

add_library(png STATIC IMPORTED)
set_target_properties(
	png
	PROPERTIES
	IMPORTED_LOCATION "${VENDOR_LIBRARY}/libpng.a"
)

add_library(zip STATIC IMPORTED)
set_target_properties(
        zip
        PROPERTIES
        IMPORTED_LOCATION "${VENDOR_LIBRARY}/libzip.a"
)

add_library(freetype STATIC IMPORTED)
set_target_properties(
        freetype
        PROPERTIES
        IMPORTED_LOCATION "${VENDOR_LIBRARY}/freetype/lib/libfreetype.a"
)

include_directories(src)
include_directories(${VENDOR_DIRECTORY}/png/include)
include_directories(${VENDOR_DIRECTORY}/jpeg/include)
include_directories(${VENDOR_DIRECTORY}/zip/include)
include_directories(${VENDOR_DIRECTORY}/freetype/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(cherry STATIC ${SOURCES})
target_link_libraries(cherry png jpeg zip freetype m pthread ssl crypto)

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
	add_custom_command(
		TARGET cherry
		POST_BUILD
		COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libcherry.a
	)
endif()

install (TARGETS cherry
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
    add_dependencies(test cherry ${OPENGL_APPLICATION})
	target_link_libraries(test cherry ${OPENGL_APPLICATION})
endif()