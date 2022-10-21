cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(PROJECT_NAME caches)
project(${PROJECT_NAME} LANGUAGES CXX)

include_directories(include)

set(SOURCE_LIB src/cache.cpp)
set(SOURCE_TEST tests/tests.cpp)

add_compile_options(-g)
add_library(${PROJECT_NAME} STATIC ${SOURCE_LIB})

add_definitions(-DPROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\")

#tests
option(TESTING "Include testing" ON)
if (TESTING)
    message ("Include testing")
    find_package (GTest REQUIRED)
    include_directories (${GTEST_INCLUDE_DIRS})

    add_executable (${PROJECT_NAME}_tests ${SOURCE_TEST})

    target_link_libraries (${PROJECT_NAME}_tests PUBLIC GTest::GTest GTest::Main caches)
    enable_testing ()

    add_dependencies (${PROJECT_NAME}_tests ${PROJECT_NAME})
endif ()

#sanitizers
option(SANITIZERS "Include sanitizers" ON)
if (SANITIZERS)
    message ("Build with sanitizers")

    add_compile_options (-fsanitize=address -fsanitize=undefined -g)
    set_target_properties (${PROJECT_NAME}_tests PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=undefined")
endif ()
