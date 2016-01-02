#
#   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
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

include(CheckLibraryExists)

include_directories(${PROJECT_BINARY_DIR})

link_directories(${PROJECT_BINARY_DIR})

remove_definitions(-DCMAKE_BUILD)

check_include_file("curl/curl.h" RE_HAVE_CURL_CURL_H)
find_package(CURL)

add_executable(test_fayecpp_header test_fayecpp_header.cpp)
set_property(TARGET test_fayecpp_header APPEND PROPERTY COMPILE_FLAGS -DFAYECPP_STATIC)
target_link_libraries(test_fayecpp_header fayecpp_static websockets jansson)
add_test(test_fayecpp_header test_fayecpp_header)


add_executable(test_fayecpp_variantmessage test_fayecpp_variantmessage.cpp)
set_property(TARGET test_fayecpp_variantmessage APPEND PROPERTY COMPILE_FLAGS -DFAYECPP_STATIC)
target_link_libraries(test_fayecpp_variantmessage fayecpp_static websockets jansson)
add_test(test_fayecpp_variantmessage test_fayecpp_variantmessage)


add_executable(test_fayecpp_init test_fayecpp_init.cpp)
set_property(TARGET test_fayecpp_init APPEND PROPERTY COMPILE_FLAGS -DFAYECPP_STATIC)
target_link_libraries(test_fayecpp_init fayecpp_static websockets jansson)
add_test(test_fayecpp_init test_fayecpp_init)


add_executable(test_fayecpp_connect test_fayecpp_connect.cpp)
set_property(TARGET test_fayecpp_connect APPEND PROPERTY COMPILE_FLAGS -DFAYECPP_STATIC)
target_link_libraries(test_fayecpp_connect fayecpp_static websockets jansson)
add_test(test_fayecpp_connect test_fayecpp_connect)


if(RE_HAVE_PTHREAD_H)
	target_link_libraries(test_fayecpp_header pthread)
	target_link_libraries(test_fayecpp_variantmessage pthread)
	target_link_libraries(test_fayecpp_init pthread)
	target_link_libraries(test_fayecpp_connect pthread)
endif(RE_HAVE_PTHREAD_H)

if(RE_HAVE_COREFOUNDATION_FRAMEWORK)
	target_link_libraries(test_fayecpp_header ${COREFOUNDATION_FRAMEWORK})
	target_link_libraries(test_fayecpp_variantmessage ${COREFOUNDATION_FRAMEWORK})
	target_link_libraries(test_fayecpp_init ${COREFOUNDATION_FRAMEWORK})
	target_link_libraries(test_fayecpp_connect ${COREFOUNDATION_FRAMEWORK})
endif(RE_HAVE_COREFOUNDATION_FRAMEWORK)

if(MINGW)
	target_link_libraries(test_fayecpp_header ws2_32)
	target_link_libraries(test_fayecpp_variantmessage ws2_32)
	target_link_libraries(test_fayecpp_init ws2_32)
	target_link_libraries(test_fayecpp_connect ws2_32)
endif(MINGW)


if(RE_HAVE_CURL_CURL_H)
	add_executable(dbuploader dbuploader.c)
	target_link_libraries(dbuploader ${CURL_LIBRARIES})
endif(RE_HAVE_CURL_CURL_H)


install(TARGETS test_fayecpp_header DESTINATION bin)
install(TARGETS test_fayecpp_variantmessage DESTINATION bin)
install(TARGETS test_fayecpp_init DESTINATION bin)
install(TARGETS test_fayecpp_connect DESTINATION bin)

if(RE_HAVE_CURL_CURL_H)
	install(TARGETS dbuploader DESTINATION bin)
endif(RE_HAVE_CURL_CURL_H)
