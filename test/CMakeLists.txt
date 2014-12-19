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

include(CheckLibraryExists)

include_directories(${PROJECT_BINARY_DIR})

link_directories(${PROJECT_BINARY_DIR})

check_include_file("curl/curl.h" HAVE_CURL_CURL_H)
find_package(CURL)

add_executable(test_fayecpp_header test_fayecpp_header.cpp)
target_link_libraries(test_fayecpp_header fayecpp)
add_test(test_fayecpp_header test_fayecpp_header)


add_executable(test_fayecpp_init test_fayecpp_init.cpp)
target_link_libraries(test_fayecpp_init fayecpp)
add_test(test_fayecpp_init test_fayecpp_init)


if(HAVE_CURL_CURL_H)
add_executable(dbuploader dbuploader.cpp)
target_link_libraries(dbuploader ${CURL_LIBRARIES})
endif(HAVE_CURL_CURL_H)


install(TARGETS test_fayecpp_header DESTINATION bin)
install(TARGETS test_fayecpp_init DESTINATION bin)

if(HAVE_CURL_CURL_H)
install(TARGETS dbuploader DESTINATION bin)
endif(HAVE_CURL_CURL_H)
