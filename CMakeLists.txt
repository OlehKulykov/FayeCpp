#
#   Copyright 2012 - 2014 Kulykov Oleh
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

cmake_minimum_required(VERSION 2.8)


project(fayecpp)


set(PACKAGE "fayecpp")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "info@resident.name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "0.1.5")
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


add_definitions(-DCMAKE_BUILD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message("Looking for libwebsockets...")
find_package(LIBWEBSOCKETS QUIET)
if(NOT LIBWEBSOCKETS_FOUND)
message("libwebsockets not found, using bundled copy.")
add_subdirectory(libwebsockets)
list(APPEND LIBWEBSOCKETS_INCLUDE_DIRS
			${CMAKE_CURRENT_SOURCE_DIR}/libwebsockets/lib
			${CMAKE_BINARY_DIR}/libwebsockets/lib
			${CMAKE_BINARY_DIR}/libwebsockets
)
set(LIBWEBSOCKETS_FOUND 1)
endif()
message("libwebsockets include dirs: ${LIBWEBSOCKETS_INCLUDE_DIRS}")
message("libwebsockets libraries: ${LIBWEBSOCKETS_LIBRARIES}")
include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
set(HAVE_LIBWEBSOCKETS_H 1)


message("Looking for jansson...")
find_package(JANSSON QUIET)
if(NOT JANSSON_FOUND)
message("jansson not found, using bundled copy.")
add_subdirectory(jansson)
list(APPEND JANSSON_INCLUDE_DIRS
			${CMAKE_CURRENT_SOURCE_DIR}/jansson/src
			${CMAKE_BINARY_DIR}/jansson/include
)
set(JANSSON_FOUND 1)
endif()
message("jansson include dirs: ${JANSSON_INCLUDE_DIRS}")
message("jansson libraries: ${JANSSON_LIBRARIES}")
include_directories(${JANSSON_INCLUDE_DIRS})
set(HAVE_JANSSON_H 1)


check_include_file("stdlib.h" HAVE_STDLIB_H)
if(HAVE_STDLIB_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES stdlib.h)
endif(HAVE_STDLIB_H)

check_include_file("inttypes.h" HAVE_INTTYPES_H)
if(HAVE_INTTYPES_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES inttypes.h)
endif(HAVE_INTTYPES_H)

check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
if(HAVE_SYS_TYPES_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/types.h)
endif(HAVE_SYS_TYPES_H)

check_include_file("stddef.h" HAVE_STDDEF_H)
if(HAVE_STDDEF_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES stddef.h)
endif(HAVE_STDDEF_H)

check_include_file("stdbool.h" HAVE_STDBOOL_H)
if(HAVE_STDBOOL_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES stdbool.h)
endif(HAVE_STDBOOL_H)

check_include_file("stdint.h" HAVE_STDINT_H)
if(HAVE_STDINT_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES stdint.h)
endif(HAVE_STDINT_H)

check_include_file("stdarg.h" HAVE_STDARG_H)
if(HAVE_STDARG_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES stdarg.h)
endif(HAVE_STDARG_H)

check_include_file("limits.h" HAVE_LIMITS_H)
if(HAVE_LIMITS_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES limits.h)
endif(HAVE_LIMITS_H)

check_include_file("float.h" HAVE_FLOAT_H)
if(HAVE_FLOAT_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES float.h)
endif(HAVE_FLOAT_H)

check_include_file("math.h" HAVE_MATH_H)
if(HAVE_MATH_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES math.h)
endif(HAVE_MATH_H)

check_include_file("wchar.h" HAVE_WCHAR_H)
if(HAVE_WCHAR_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES wchar.h)
endif(HAVE_WCHAR_H)

check_include_file("malloc.h" HAVE_MALLOC_H)
if(HAVE_MALLOC_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES malloc.h)
endif(HAVE_MALLOC_H)

check_include_file("memory.h" HAVE_MEMORY_H)
if(HAVE_MEMORY_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES memory.h)
endif(HAVE_MEMORY_H)

check_include_file("string.h" HAVE_STRING_H)
if(HAVE_STRING_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES string.h)
endif(HAVE_STRING_H)


check_include_file("mach/mach_time.h" HAVE_MACH_MACH_TIME_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("time.h" HAVE_TIME_H)
check_include_file("locale.h" HAVE_LOCALE_H)
check_include_file("assert.h" HAVE_ASSERT_H)

check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
if(HAVE_SYS_STAT_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
endif(HAVE_SYS_STAT_H)

check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("pthread.h" HAVE_PTHREAD_H)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H)

check_include_file("direct.h" HAVE_DIRECT_H)
if(HAVE_DIRECT_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES direct.h)
endif(HAVE_DIRECT_H)

check_include_file("byteswap.h" HAVE_BYTESWAP_H)

check_include_file("errno.h" HAVE_ERRNO_H)
if(HAVE_ERRNO_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES errno.h)
endif(HAVE_ERRNO_H)

check_include_file("sys/errno.h" HAVE_SYS_ERRNO_H)
if(HAVE_SYS_ERRNO_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES sys/errno.h)
endif(HAVE_SYS_ERRNO_H)

check_include_file("io.h" HAVE_IO_H)
if(HAVE_IO_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES io.h)
endif(HAVE_IO_H)

check_include_file("signal.h" HAVE_SIGNAL_H)
check_include_file("setjmp.h" HAVE_SETJMP_H)

check_include_file("unistd.h" HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
	LIST(APPEND CMAKE_EXTRA_INCLUDE_FILES unistd.h)
endif(HAVE_UNISTD_H)

check_include_file("dispatch/dispatch.h" HAVE_DISPATCH_DISPATCH_H)

message(CMAKE_EXTRA_INCLUDE_FILES ": " ${CMAKE_EXTRA_INCLUDE_FILES})

# To check the size of a primitive type:
check_type_size("void*" SIZEOF_VOIDP_T)
if(SIZEOF_VOIDP_T AND (NOT VOIDP_TYPE))
	SET(VOIDP_TYPE void*)
endif()

check_type_size("int8_t" SIZEOF_INT8_T)
if(SIZEOF_INT8_T AND (NOT INT8_TYPE))
	SET(INT8_TYPE int8_t)
endif()

check_type_size("uint8_t" SIZEOF_UINT8_T)
if(SIZEOF_UINT8_T AND (NOT UINT8_TYPE))
	SET(UINT8_TYPE uint8_t)
endif()

check_type_size("u_int8_t" SIZEOF_U_INT8_T)
if(SIZEOF_U_INT8_T AND (NOT UINT8_TYPE))
	SET(UINT8_TYPE u_int8_t)
endif()

check_type_size("int16_t" SIZEOF_INT16_T)
if(SIZEOF_INT16_T AND (NOT INT16_TYPE))
	SET(INT16_TYPE int16_t)
endif()

check_type_size("uint16_t" SIZEOF_UINT16_T)
if(SIZEOF_UINT16_T AND (NOT UINT16_TYPE))
	SET(UINT16_TYPE uint16_t)
endif()

check_type_size("u_int16_t" SIZEOF_U_INT16_T)
if(SIZEOF_U_INT16_T AND (NOT UINT16_TYPE))
	SET(UINT16_TYPE u_int16_t)
endif()

check_type_size("int32_t" SIZEOF_INT32_T)
if(SIZEOF_INT32_T AND (NOT INT32_TYPE))
	SET(INT32_TYPE int32_t)
endif()

check_type_size("uint32_t" SIZEOF_UINT32_T)
if(SIZEOF_UINT32_T AND (NOT UINT32_TYPE))
	SET(UINT32_TYPE uint32_t)
endif()

check_type_size("int64_t" SIZEOF_INT64_T)
if(SIZEOF_INT64_T AND (NOT INT64_TYPE))
	SET(INT64_TYPE int64_t)
endif()

check_type_size("uint64_t" SIZEOF_UINT64_T)
if(SIZEOF_UINT64_T AND (NOT UINT64_TYPE))
	SET(UINT64_TYPE uint64_t)
endif()

check_type_size("u_int64_t" SIZEOF_U_INT64_T)
if(SIZEOF_U_INT64_T AND (NOT UINT64_TYPE))
	SET(UINT64_TYPE u_int64_t)
endif()

check_type_size("uintptr_t" SIZEOF_UINTPTR_T)
if(SIZEOF_UINTPTR_T AND (NOT UINTPTR_TYPE))
	SET(UINTPTR_TYPE uintptr_t)
endif()


check_type_size("char" SIZEOF_CHAR)
if(SIZEOF_CHAR AND (NOT INT8_TYPE))
	SET(INT8_TYPE char)
endif()

check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
if(SIZEOF_UNSIGNED_CHAR AND (NOT UINT8_TYPE))
	SET(UINT8_TYPE "unsigned char")
endif()

check_type_size("short" SIZEOF_SHORT)
if(SIZEOF_SHORT AND (NOT INT16_TYPE) AND (SIZEOF_SHORT EQUAL 2))
	SET(INT16_TYPE short)
endif()

check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
if(SIZEOF_UNSIGNED_SHORT AND (NOT UINT16_TYPE) AND (SIZEOF_UNSIGNED_SHORT EQUAL 2))
	SET(UINT16_TYPE "unsigned short")
endif()

check_type_size("int" SIZEOF_INT)
if(SIZEOF_INT AND (NOT INT32_TYPE) AND (SIZEOF_INT EQUAL 4))
	SET(INT32_TYPE int)
endif()

check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
if(SIZEOF_UNSIGNED_INT AND (NOT UINT32_TYPE) AND (SIZEOF_UNSIGNED_INT EQUAL 4))
	SET(UINT32_TYPE "unsigned int")
endif()

check_type_size("long int" SIZEOF_LONG_INT)
if(SIZEOF_LONG_INT AND (SIZEOF_LONG_INT EQUAL 4) AND (NOT INT32_TYPE))
	SET(INT32_TYPE "long int")
endif()
if(SIZEOF_LONG_INT AND (SIZEOF_LONG_INT EQUAL 8) AND (NOT INT64_TYPE))
	SET(INT64_TYPE "long int")
endif()


check_type_size("unsigned long int" SIZEOF_UNSIGNED_LONG_INT)
if(SIZEOF_UNSIGNED_LONG_INT AND (SIZEOF_UNSIGNED_LONG_INT EQUAL 4) AND (NOT UINT32_TYPE))
	SET(UINT32_TYPE "unsigned long int")
endif()
if(SIZEOF_UNSIGNED_LONG_INT AND (SIZEOF_UNSIGNED_LONG_INT EQUAL 8) AND (NOT UINT64_TYPE))
	SET(UINT64_TYPE "unsigned long int")
endif()

check_type_size("long long int" SIZEOF_LONG_LONG_INT)
if(SIZEOF_LONG_LONG_INT AND (SIZEOF_LONG_LONG_INT EQUAL 4) AND (NOT INT32_TYPE))
	SET(INT32_TYPE "long long int")
endif()
if(SIZEOF_LONG_LONG_INT AND (SIZEOF_LONG_LONG_INT EQUAL 8) AND (NOT INT64_TYPE))
	SET(INT64_TYPE "long long int")
endif()

check_type_size("unsigned long long int" SIZEOF_UNSIGNED_LONG_LONG_INT)
if(SIZEOF_UNSIGNED_LONG_LONG_INT AND (SIZEOF_UNSIGNED_LONG_LONG_INT EQUAL 4) AND (NOT UINT32_TYPE))
	SET(UINT32_TYPE "unsigned long long int")
endif()
if(SIZEOF_UNSIGNED_LONG_LONG_INT AND (SIZEOF_UNSIGNED_LONG_LONG_INT EQUAL 8) AND (NOT UINT64_TYPE))
	SET(UINT64_TYPE "unsigned long long int")
endif()


check_type_size("float_t" SIZEOF_FLOAT_T)
if(SIZEOF_FLOAT_T AND (NOT FLOAT32_TYPE) AND (SIZEOF_FLOAT_T EQUAL 4))
	SET(FLOAT32_TYPE float_t)
endif()

check_type_size("double_t" SIZEOF_DOUBLE_T)
if(SIZEOF_DOUBLE_T AND (NOT FLOAT64_TYPE) AND (SIZEOF_DOUBLE_T EQUAL 8))
	SET(FLOAT32_TYPE double_t)
endif()

check_type_size("long double_t" SIZEOF_LONG_DOUBLE_T)
if(SIZEOF_LONG_DOUBLE_T AND (NOT FLOAT64_TYPE) AND (SIZEOF_LONG_DOUBLE_T EQUAL 8))
	SET(FLOAT64_TYPE "long double_t")
endif()
if(SIZEOF_LONG_DOUBLE_T AND (NOT FLOAT128_TYPE) AND (SIZEOF_LONG_DOUBLE_T EQUAL 16))
	SET(FLOAT128_TYPE "long double_t")
	SET(HAVE_FLOAT128 1)
endif()

check_type_size("float" SIZEOF_FLOAT)
if(SIZEOF_FLOAT AND (NOT FLOAT32_TYPE) AND (SIZEOF_FLOAT EQUAL 4))
	SET(FLOAT32_TYPE float)
endif()

check_type_size("double" SIZEOF_DOUBLE)
if(SIZEOF_DOUBLE AND (NOT FLOAT64_TYPE) AND (SIZEOF_DOUBLE EQUAL 8))
	SET(FLOAT32_TYPE double)
endif()

check_type_size("long double" SIZEOF_LONG_DOUBLE)
if(SIZEOF_LONG_DOUBLE AND (NOT FLOAT64_TYPE) AND (SIZEOF_LONG_DOUBLE EQUAL 8))
	SET(FLOAT64_TYPE "long double")
endif()
if(SIZEOF_LONG_DOUBLE AND (NOT FLOAT128_TYPE) AND (SIZEOF_LONG_DOUBLE EQUAL 16))
	SET(FLOAT128_TYPE "long double")
	SET(HAVE_FLOAT128 1)
endif()

check_type_size("bool" SIZEOF_BOOL)
if(SIZEOF_BOOL)
	SET(BOOL_TYPE bool)
endif()

check_type_size("__int128_t" SIZEOF___INT128_T)
if(SIZEOF___INT128_T AND (NOT INT128_TYPE) AND (SIZEOF___INT128_T EQUAL 16))
	SET(INT128_TYPE __int128_t)
endif()
check_type_size("int128_t" SIZEOF_INT128_T)
if(SIZEOF_INT128_T AND (NOT INT128_TYPE) AND (SIZEOF_INT128_T EQUAL 16))
	SET(INT128_TYPE int128_t)
endif()

check_type_size("__uint128_t" SIZEOF___UINT128_T)
if(SIZEOF___UINT128_T AND (NOT UINT128_TYPE) AND (SIZEOF___UINT128_T EQUAL 16))
	SET(UINT128_TYPE __uint128_t)
endif()
check_type_size("uint128_t" SIZEOF_UINT128_T)
if(SIZEOF_UINT128_T AND (NOT UINT128_TYPE) AND (SIZEOF_UINT128_T EQUAL 16))
	SET(UINT128_TYPE uint128_t)
endif()

if(INT128_TYPE AND UINT128_TYPE)
	SET(HAVE_INTEGERS128 1)
endif()


if(SIZEOF_VOIDP_T EQUAL 4)
	SET(__RE_32BIT_PLATFORM__ 1)
endif()
if(SIZEOF_VOIDP_T EQUAL 8)
	SET(__RE_64BIT_PLATFORM__ 1)
endif()

check_type_size("struct stat" SIZEOF_STRUCT_STAT)
if(SIZEOF_STRUCT_STAT)
	SET(HAVE_STRUCT_STAT 1)
endif()

check_type_size("struct _stat" SIZEOF_STRUCT__STAT)
if(SIZEOF_STRUCT__STAT)
	SET(HAVE_STRUCT__STAT 1)
endif()


# functions
check_function_exists(posix_memalign HAVE_FUNCTION_POSIX_MEMALIGN)
check_function_exists(usleep HAVE_FUNCTION_USLEEP)
check_function_exists(getrlimit HAVE_FUNCTION_GETRLIMIT)

if(HAVE_PTHREAD_H)
check_function_exists(pthread_attr_setscope HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
check_function_exists(pthread_attr_setstacksize HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE)
check_function_exists(pthread_cancel HAVE_FUNCTION_PTHREAD_CANCEL)
check_function_exists(pthread_setname_np HAVE_FUNCTION_PTHREAD_SETNAME_NP)
check_function_exists(pthread_set_name_np HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
endif(HAVE_PTHREAD_H)

check_function_exists(_stat HAVE_FUNCTION__STAT)
check_function_exists(stat HAVE_FUNCTION_STAT)
check_function_exists(_wstat HAVE_FUNCTION__WSTAT)
check_function_exists(access HAVE_FUNCTION_ACCESS)
check_function_exists(_access HAVE_FUNCTION__ACCESS)
check_function_exists(_waccess HAVE_FUNCTION__WACCESS)
check_function_exists(_mkdir HAVE_FUNCTION__MKDIR)
check_function_exists(mkdir HAVE_FUNCTION_MKDIR)
check_function_exists(_wmkdir HAVE_FUNCTION__WMKDIR)
check_function_exists(clock_gettime HAVE_FUNCTION_CLOCK_GETTIME)
check_function_exists(fabsf HAVE_FUNCTION_FABSF)
check_function_exists(fabs HAVE_FUNCTION_FABS)
check_function_exists(floorf HAVE_FUNCTION_FLOORF)
check_function_exists(floor HAVE_FUNCTION_FLOOR)
check_function_exists(powf HAVE_FUNCTION_POWF)
check_function_exists(pow HAVE_FUNCTION_POW)
check_function_exists(cosf HAVE_FUNCTION_COSF)
check_function_exists(cos HAVE_FUNCTION_COS)
check_function_exists(sinf HAVE_FUNCTION_SINF)
check_function_exists(sin HAVE_FUNCTION_SIN)
check_function_exists(sqrtf HAVE_FUNCTION_SQRTF)
check_function_exists(sqrt HAVE_FUNCTION_SQRT)
check_function_exists(_wfopen HAVE_FUNCTION__WFOPEN)
check_function_exists(_wfopen_s HAVE_FUNCTION__WFOPEN_S)
check_function_exists(fopen_s HAVE_FUNCTION_FOPEN_S)
check_function_exists(sscanf_s HAVE_FUNCTION_SSCANF_S)
check_function_exists(sprintf_s HAVE_FUNCTION_SPRINTF_S)
check_function_exists(vsnprintf HAVE_FUNCTION_VSNPRINTF)
check_function_exists(vsprintf_s HAVE_FUNCTION_VSPRINTF_S)

check_function_exists(clock_gettime HAVE_FUNCTION_CLOCK_GETTIME)
check_function_exists(difftime HAVE_FUNCTION_DIFFTIME)
check_function_exists(time HAVE_FUNCTION_TIME)
check_function_exists(localtime HAVE_FUNCTION_LOCALTIME)
check_function_exists(gmtime HAVE_FUNCTION_GMTIME)
check_function_exists(strftime HAVE_FUNCTION_STRFTIME)

if(HAVE_DISPATCH_DISPATCH_H)
check_function_exists(dispatch_sync HAVE_FUNCTION_DISPATCH_SYNC)
check_function_exists(dispatch_async HAVE_FUNCTION_DISPATCH_ASYNC)
check_function_exists(dispatch_get_global_queue HAVE_FUNCTION_DISPATCH_GET_GLOBAL_QUEUE)
check_function_exists(dispatch_after HAVE_FUNCTION_DISPATCH_AFTER)
endif(HAVE_DISPATCH_DISPATCH_H)

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
Version: 0.1

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



set(FAYECPP_SOURCES	src/client.cpp
		src/jsonutils.cpp
		src/message.cpp
		src/REBuffer.cpp
		src/REBufferNoCopy.cpp
		src/RELog.cpp
		src/REMutableString.cpp
		src/REStaticString.cpp
		src/REString.cpp
		src/REStringBase.cpp
		src/REStringList.cpp
		src/REStringUtilsPrivate.cpp
		src/REVariant.cpp
		src/REVariantList.cpp
		src/REVariantMap.cpp
		src/REWideString.cpp
		src/transport.cpp
		src/websocket.cpp
				)
				

set(FAYECPP_HEADERS 	fayecpp.h
				fayecpp_config.h)


add_definitions(-DHAVE_FAYECPP_CONFIG_H)

#add_library(fayecpp STATIC ${FAYECPP_SOURCES} ${FAYECPP_HEADERS})

add_library(fayecpp SHARED ${FAYECPP_SOURCES} ${FAYECPP_HEADERS})


add_dependencies(fayecpp websockets jansson)

target_link_libraries(fayecpp websockets)
target_link_libraries(fayecpp jansson)
if (HAVE_PTHREAD_H)
target_link_libraries(fayecpp pthread)
endif()

install(TARGETS fayecpp
		DESTINATION lib)


install(FILES fayecpp.h 
		DESTINATION include)

enable_testing()
add_subdirectory(test)

# This must always be last!
include(CPack)
