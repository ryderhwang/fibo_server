cmake_minimum_required(VERSION 3.6)
project(rfpgt)

add_library(rfpgt
    rfpgt_threadpool.h
    rfpgt_threadpool.cpp
)
target_include_directories(rfpgt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rfpgt dplp)

add_executable(rfpgt_threadpool.t rfpgt_threadpool.t.cpp)
target_link_libraries(rfpgt_threadpool.t rfpgt GTest::GTest)
add_test(NAME rfpgt_threadpool.t COMMAND rfpgt_threadpool.t)
# ----------------------------------------------------------------------------
# Copyright 2017 Bloomberg Finance L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
