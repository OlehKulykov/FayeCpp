#
#   Copyright 2014 - 2015 Kulykov Oleh
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

include(CheckLibraryExists)

include_directories(${PROJECT_BINARY_DIR})

link_directories(${PROJECT_BINARY_DIR})

remove_definitions(-DCMAKE_BUILD)

check_include_file("curl/curl.h" HAVE_CURL_CURL_H)
find_package(CURL)

add_executable(test_fayecpp_header test_fayecpp_header.cpp)
set_target_properties(test_fayecpp_header PROPERTIES COMPILE_FLAGS "-DFAYECPP_STATIC")
set_target_properties(test_fayecpp_header PROPERTIES COMPILE_DEFINITIONS "-DFAYECPP_STATIC")
target_link_libraries(test_fayecpp_header fayecpp_static websockets jansson)
add_test(test_fayecpp_header test_fayecpp_header)


add_executable(test_fayecpp_init test_fayecpp_init.cpp)
set_target_properties(test_fayecpp_init PROPERTIES COMPILE_FLAGS "-DFAYECPP_STATIC")
set_target_properties(test_fayecpp_init PROPERTIES COMPILE_DEFINITIONS "-DFAYECPP_STATIC")
target_link_libraries(test_fayecpp_init fayecpp_static websockets jansson)
add_test(test_fayecpp_init test_fayecpp_init)


add_executable(test_fayecpp_connect test_fayecpp_connect.cpp)
set_target_properties(test_fayecpp_connect PROPERTIES COMPILE_FLAGS "-DFAYECPP_STATIC")
set_target_properties(test_fayecpp_connect PROPERTIES COMPILE_DEFINITIONS "-DFAYECPP_STATIC")
target_link_libraries(test_fayecpp_connect fayecpp_static websockets jansson)
add_test(test_fayecpp_connect test_fayecpp_connect)


if(HAVE_CURL_CURL_H)
add_executable(dbuploader dbuploader.c)
target_link_libraries(dbuploader ${CURL_LIBRARIES})
endif(HAVE_CURL_CURL_H)


install(TARGETS test_fayecpp_header DESTINATION bin)
install(TARGETS test_fayecpp_init DESTINATION bin)
install(TARGETS test_fayecpp_connect DESTINATION bin)

if(HAVE_CURL_CURL_H)
install(TARGETS dbuploader DESTINATION bin)
endif(HAVE_CURL_CURL_H)
