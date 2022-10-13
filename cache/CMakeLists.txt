cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(PROJECT_NAME caches)
project(${PROJECT_NAME} LANGUAGES CXX)

include_directories(include)

set(SOURCE_LIB src/cache.cpp)
set(SOURCE_MAIN main.cpp)
set(SOURCE_TEST tests/tests.cpp)

include(FetchContent)
FetchContent_Declare(
    googletest
 
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

add_library(cache_lib STATIC ${SOURCE_LIB})
add_executable(main ${SOURCE_MAIN})
add_executable(cache ${SOURCE_LIB})
add_executable(test ${SOURCE_TEST})

target_link_libraries(main cache_lib)
target_link_libraries(test gtest_main cache_lib)
