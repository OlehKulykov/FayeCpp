#
#   Copyright (c) 2014 - 2017 Kulykov Oleh <info@resident.name>
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8)


project(fayecpp)


set(PACKAGE "fayecpp")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "info@resident.name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "1.2.0")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(VERSION "${CPACK_PACKAGE_VERSION}")


include(CheckLibraryExists)
include(CheckFunctionExists)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)

include(CheckTypeSize)
include(CheckSymbolExists)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)


# C with -fPIC
check_c_compiler_flag("-fPIC" WITH_FPIC_C)
if(WITH_FPIC_C)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(WITH_FPIC_C)


# CXX with -fPIC
check_cxx_compiler_flag("-fPIC" WITH_FPIC_CXX)
if(WITH_FPIC_CXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(WITH_FPIC_CXX)


if(MSVC)
	# Suppress warning about "deprecated" functions
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)


if(WIN32)
	# Add windows definitions, cause this can be missed during compiling
	# and libwebsockets checks only one of this defines(so add both).
	add_definitions(-DWIN32)
	add_definitions(-D_WIN32)
	set(__RE_OS_WINDOWS__ 1)
endif(WIN32)


if(MINGW)
	set(__RE_COMPILER_MINGW__ 1)
endif(MINGW)


add_definitions(-DCMAKE_BUILD)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


message("Looking for CoreFoundation...")
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
message("Finding library CoreFoundation: ${COREFOUNDATION_FRAMEWORK}")
if(COREFOUNDATION_FRAMEWORK)
	set(RE_HAVE_COREFOUNDATION_FRAMEWORK 1)
endif()
if(RE_HAVE_COREFOUNDATION_FRAMEWORK)
	message("CoreFoundation found")
endif()


message("Looking for libwebsockets...")
find_package(LIBWEBSOCKETS QUIET)
if(NOT LIBWEBSOCKETS_FOUND)
	message("libwebsockets not found, using bundled copy.")
	add_subdirectory(libwebsockets)
	list(APPEND LIBWEBSOCKETS_INCLUDE_DIRS
				${CMAKE_CURRENT_SOURCE_DIR}/libwebsockets/lib
				${CMAKE_BINARY_DIR}/libwebsockets/lib
				${CMAKE_BINARY_DIR}/libwebsockets)
	set(LIBWEBSOCKETS_FOUND 1)
	set(LIBWEBSOCKETS_BUNDLED_COPY 1)
endif()
message("libwebsockets include dirs: ${LIBWEBSOCKETS_INCLUDE_DIRS}")
message("libwebsockets libraries: ${LIBWEBSOCKETS_LIBRARIES}")
include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
set(RE_HAVE_LIBWEBSOCKETS_H 1)


message("Looking for jansson...")
find_package(JANSSON QUIET)
if(NOT JANSSON_FOUND)
	message("jansson not found, using bundled copy.")
	add_subdirectory(jansson)
	list(APPEND JANSSON_INCLUDE_DIRS
				${CMAKE_CURRENT_SOURCE_DIR}/jansson/src
				${CMAKE_BINARY_DIR}/jansson/include)
	set(JANSSON_FOUND 1)
endif()
message("jansson include dirs: ${JANSSON_INCLUDE_DIRS}")
message("jansson libraries: ${JANSSON_LIBRARIES}")
include_directories(${JANSSON_INCLUDE_DIRS})
set(RE_HAVE_JANSSON_H 1)


check_include_file("stdlib.h" RE_HAVE_STDLIB_H)
if(RE_HAVE_STDLIB_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES stdlib.h)
endif(RE_HAVE_STDLIB_H)

check_include_file("inttypes.h" RE_HAVE_INTTYPES_H)
if(RE_HAVE_INTTYPES_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES inttypes.h)
endif(RE_HAVE_INTTYPES_H)

check_include_file("sys/types.h" RE_HAVE_SYS_TYPES_H)
if(RE_HAVE_SYS_TYPES_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/types.h)
endif(RE_HAVE_SYS_TYPES_H)

check_include_file("stddef.h" RE_HAVE_STDDEF_H)
if(RE_HAVE_STDDEF_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES stddef.h)
endif(RE_HAVE_STDDEF_H)

check_include_file("stdbool.h" RE_HAVE_STDBOOL_H)
if(RE_HAVE_STDBOOL_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES stdbool.h)
endif(RE_HAVE_STDBOOL_H)

check_include_file("stdint.h" RE_HAVE_STDINT_H)
if(RE_HAVE_STDINT_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES stdint.h)
endif(RE_HAVE_STDINT_H)

check_include_file("stdarg.h" RE_HAVE_STDARG_H)
if(RE_HAVE_STDARG_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES stdarg.h)
endif(RE_HAVE_STDARG_H)

check_include_file("limits.h" RE_HAVE_LIMITS_H)
if(RE_HAVE_LIMITS_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES limits.h)
endif(RE_HAVE_LIMITS_H)

check_include_file("float.h" RE_HAVE_FLOAT_H)
if(RE_HAVE_FLOAT_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES float.h)
endif(RE_HAVE_FLOAT_H)

check_include_file("math.h" RE_HAVE_MATH_H)
if(RE_HAVE_MATH_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES math.h)
endif(RE_HAVE_MATH_H)

check_include_file("wchar.h" RE_HAVE_WCHAR_H)
if(RE_HAVE_WCHAR_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES wchar.h)
endif(RE_HAVE_WCHAR_H)

check_include_file("malloc.h" RE_HAVE_MALLOC_H)
if(RE_HAVE_MALLOC_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES malloc.h)
endif(RE_HAVE_MALLOC_H)

check_include_file("memory.h" RE_HAVE_MEMORY_H)
if(RE_HAVE_MEMORY_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES memory.h)
endif(RE_HAVE_MEMORY_H)

check_include_file("string.h" RE_HAVE_STRING_H)
if(RE_HAVE_STRING_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES string.h)
endif(RE_HAVE_STRING_H)


check_include_file("mach/mach_time.h" RE_HAVE_MACH_MACH_TIME_H)
check_include_file("sys/time.h" RE_HAVE_SYS_TIME_H)
check_include_file("time.h" RE_HAVE_TIME_H)
check_include_file("locale.h" RE_HAVE_LOCALE_H)
check_include_file("assert.h" RE_HAVE_ASSERT_H)

check_include_file("sys/stat.h" RE_HAVE_SYS_STAT_H)
if(RE_HAVE_SYS_STAT_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
endif(RE_HAVE_SYS_STAT_H)

#	check_include_file("fcntl.h" RE_HAVE_FCNTL_H)
check_include_file("pthread.h" RE_HAVE_PTHREAD_H)
#	check_include_file("sys/resource.h" RE_HAVE_SYS_RESOURCE_H)

check_include_file("direct.h" RE_HAVE_DIRECT_H)
if(RE_HAVE_DIRECT_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES direct.h)
endif(RE_HAVE_DIRECT_H)

#	check_include_file("byteswap.h" RE_HAVE_BYTESWAP_H)

check_include_file("errno.h" RE_HAVE_ERRNO_H)
if(RE_HAVE_ERRNO_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES errno.h)
endif(RE_HAVE_ERRNO_H)

check_include_file("sys/errno.h" RE_HAVE_SYS_ERRNO_H)
if(RE_HAVE_SYS_ERRNO_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/errno.h)
endif(RE_HAVE_SYS_ERRNO_H)

#	check_include_file("io.h" RE_HAVE_IO_H)
#	if(RE_HAVE_IO_H)
#		list(APPEND CMAKE_EXTRA_INCLUDE_FILES io.h)
#	endif(RE_HAVE_IO_H)

#	check_include_file("signal.h" RE_HAVE_SIGNAL_H)
#	check_include_file("setjmp.h" RE_HAVE_SETJMP_H)

check_include_file("unistd.h" RE_HAVE_UNISTD_H)
if(RE_HAVE_UNISTD_H)
	list(APPEND CMAKE_EXTRA_INCLUDE_FILES unistd.h)
endif(RE_HAVE_UNISTD_H)

check_include_file("dispatch/dispatch.h" RE_HAVE_DISPATCH_DISPATCH_H)

message(CMAKE_EXTRA_INCLUDE_FILES ": " ${CMAKE_EXTRA_INCLUDE_FILES})

# To check the size of a primitive type:
check_type_size("void*" RE_SIZEOF_VOIDP_T)
if(RE_SIZEOF_VOIDP_T AND (NOT RE_VOIDP_TYPE))
	set(RE_VOIDP_TYPE void*)
endif()

check_type_size("uintptr_t" RE_SIZEOF_UINTPTR_T)
if(RE_SIZEOF_UINTPTR_T AND (NOT RE_UINTPTR_TYPE))
	set(RE_UINTPTR_TYPE uintptr_t)
endif()

check_type_size("float_t" RE_SIZEOF_FLOAT_T)
if(RE_SIZEOF_FLOAT_T AND (NOT RE_FLOAT32_TYPE) AND (RE_SIZEOF_FLOAT_T EQUAL 4))
	set(FLOAT32_TYPE float_t)
endif()

check_type_size("double_t" RE_SIZEOF_DOUBLE_T)
if(RE_SIZEOF_DOUBLE_T AND (NOT RE_FLOAT64_TYPE) AND (RE_SIZEOF_DOUBLE_T EQUAL 8))
	set(RE_FLOAT32_TYPE double_t)
endif()

check_type_size("long double_t" RE_SIZEOF_LONG_DOUBLE_T)
if(RE_SIZEOF_LONG_DOUBLE_T AND (NOT RE_FLOAT64_TYPE) AND (RE_SIZEOF_LONG_DOUBLE_T EQUAL 8))
	set(RE_FLOAT64_TYPE "long double_t")
endif()
if(RE_SIZEOF_LONG_DOUBLE_T AND (NOT RE_FLOAT128_TYPE) AND (RE_SIZEOF_LONG_DOUBLE_T EQUAL 16))
	set(RE_FLOAT128_TYPE "long double_t")
	set(RE_HAVE_FLOAT128 1)
endif()

check_type_size("float" RE_SIZEOF_FLOAT)
if(RE_SIZEOF_FLOAT AND (NOT RE_FLOAT32_TYPE) AND (RE_SIZEOF_FLOAT EQUAL 4))
	set(RE_FLOAT32_TYPE float)
endif()

check_type_size("double" RE_SIZEOF_DOUBLE)
if(RE_SIZEOF_DOUBLE AND (NOT RE_FLOAT64_TYPE) AND (RE_SIZEOF_DOUBLE EQUAL 8))
	set(RE_FLOAT32_TYPE double)
endif()

check_type_size("long double" RE_SIZEOF_LONG_DOUBLE)
if(RE_SIZEOF_LONG_DOUBLE AND (NOT RE_FLOAT64_TYPE) AND (RE_SIZEOF_LONG_DOUBLE EQUAL 8))
	set(RE_FLOAT64_TYPE "long double")
endif()
if(RE_SIZEOF_LONG_DOUBLE AND (NOT RE_FLOAT128_TYPE) AND (RE_SIZEOF_LONG_DOUBLE EQUAL 16))
	set(RE_FLOAT128_TYPE "long double")
	set(RE_HAVE_FLOAT128 1)
endif()

check_type_size("bool" RE_SIZEOF_BOOL)
if(RE_SIZEOF_BOOL)
	set(RE_BOOL_TYPE bool)
endif()


if(RE_SIZEOF_VOIDP_T EQUAL 4)
	set(__RE_32BIT_PLATFORM__ 1)
endif()
if(RE_SIZEOF_VOIDP_T EQUAL 8)
	set(__RE_64BIT_PLATFORM__ 1)
endif()


# functions
check_function_exists(posix_memalign RE_HAVE_FUNCTION_POSIX_MEMALIGN)
check_function_exists(usleep RE_HAVE_FUNCTION_USLEEP)
check_function_exists(getrlimit RE_HAVE_FUNCTION_GETRLIMIT)

if(RE_HAVE_PTHREAD_H)
check_function_exists(pthread_attr_setscope RE_HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
check_function_exists(pthread_attr_setstacksize RE_HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE)
check_function_exists(pthread_cancel RE_HAVE_FUNCTION_PTHREAD_CANCEL)
check_function_exists(pthread_setname_np RE_HAVE_FUNCTION_PTHREAD_SETNAME_NP)
check_function_exists(pthread_set_name_np RE_HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
endif(RE_HAVE_PTHREAD_H)


check_function_exists(clock_gettime RE_HAVE_FUNCTION_CLOCK_GETTIME)
check_function_exists(sscanf_s RE_HAVE_FUNCTION_SSCANF_S)
check_function_exists(sprintf_s RE_HAVE_FUNCTION_SPRINTF_S)
check_function_exists(vsnprintf RE_HAVE_FUNCTION_VSNPRINTF)
check_function_exists(vsprintf_s RE_HAVE_FUNCTION_VSPRINTF_S)

check_function_exists(clock_gettime RE_HAVE_FUNCTION_CLOCK_GETTIME)
check_function_exists(difftime RE_HAVE_FUNCTION_DIFFTIME)
check_function_exists(time RE_HAVE_FUNCTION_TIME)
check_function_exists(localtime RE_HAVE_FUNCTION_LOCALTIME)
check_function_exists(gmtime RE_HAVE_FUNCTION_GMTIME)
check_function_exists(strftime RE_HAVE_FUNCTION_STRFTIME)

if(RE_HAVE_DISPATCH_DISPATCH_H)
check_function_exists(dispatch_sync RE_HAVE_FUNCTION_DISPATCH_SYNC)
check_function_exists(dispatch_async RE_HAVE_FUNCTION_DISPATCH_ASYNC)
check_function_exists(dispatch_get_global_queue RE_HAVE_FUNCTION_DISPATCH_GET_GLOBAL_QUEUE)
check_function_exists(dispatch_after RE_HAVE_FUNCTION_DISPATCH_AFTER)
endif(RE_HAVE_DISPATCH_DISPATCH_H)

set(__FAYECPP_HAVE_CONFIG__ 1)


if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)


if(UNIX)
# Generate and install pkgconfig.
# (This is not indented, because the tabs will be part of the output)
file(WRITE "${PROJECT_BINARY_DIR}/fayecpp.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include

Name: FayeCpp
Description: FayeCpp (C++) client library for desktop & mobile platforms.
Version: 0.2

Libs: -L\${libdir} -lfayecpp
Cflags: -I\${includedir}"
)

install(FILES "${PROJECT_BINARY_DIR}/fayecpp.pc"
		DESTINATION lib${LIB_SUFFIX}/pkgconfig)
endif(UNIX)


configure_file("${PROJECT_SOURCE_DIR}/fayecpp_config.h.in" 
				"${PROJECT_BINARY_DIR}/fayecpp_config.h")

# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})


set(FAYECPP_SOURCES	src/FCAdvice.cpp
		src/FCClient.cpp
		src/FCError.cpp
		src/FCJsonUtils.cpp
		src/FCMessage.cpp
		src/REBuffer.cpp
		src/REBufferNoCopy.cpp
		src/RELog.cpp
		src/REMutableString.cpp
		src/REStaticString.cpp
		src/REString.cpp
		src/REStringBase.cpp
		src/REStringList.cpp
		src/REStringUtilsPrivate.cpp
		src/REThreadingPrivate.cpp
		src/REVariant.cpp
		src/REVariantList.cpp
		src/REVariantMap.cpp
		src/REWideString.cpp
		src/FCTransport.cpp
		src/FCWebSocket.cpp
				)
				
if(MINGW)
	list(APPEND FAYECPP_SOURCES builds/windows-mingw/inet_ntop.c)
endif(MINGW)
				

set(FAYECPP_HEADERS fayecpp.h
				fayecpp_config.h)


add_definitions(-DHAVE_FAYECPP_CONFIG_H)

add_library(fayecpp_static STATIC ${FAYECPP_SOURCES} ${FAYECPP_HEADERS})
set_property(TARGET fayecpp_static APPEND PROPERTY COMPILE_FLAGS -DFAYECPP_STATIC)

add_library(fayecpp SHARED ${FAYECPP_SOURCES} ${FAYECPP_HEADERS})


add_dependencies(fayecpp websockets jansson)


target_link_libraries(fayecpp websockets)
target_link_libraries(fayecpp jansson)


if(RE_HAVE_PTHREAD_H)
	target_link_libraries(fayecpp pthread)
endif(RE_HAVE_PTHREAD_H)


if(RE_HAVE_COREFOUNDATION_FRAMEWORK)
	target_link_libraries(fayecpp ${COREFOUNDATION_FRAMEWORK})
endif(RE_HAVE_COREFOUNDATION_FRAMEWORK)


if(MINGW)
	target_link_libraries(fayecpp ws2_32)
endif(MINGW)


install(TARGETS fayecpp
		DESTINATION lib)

install(TARGETS fayecpp_static
		DESTINATION lib)

install(FILES fayecpp.h 
		DESTINATION include)


enable_testing()
add_subdirectory(test)

# This must always be last!
include(CPack)
