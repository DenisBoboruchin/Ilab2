cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

set(PROJECT_NAME tree)
project(${PROJECT_NAME} LANGUAGES CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER g++)

#include_directories (vector/include)

#tests
option(TESTING "Include testing" ON)
if (TESTING)
    message ("Include testing")
    
    set(SOURCE_TEST tests/tests.cpp)

    find_package (GTest REQUIRED)
    include_directories (${GTEST_INCLUDE_DIRS} include)
    
    add_executable (${PROJECT_NAME}_tests ${SOURCE_TEST})
    target_link_libraries (${PROJECT_NAME}_tests PRIVATE GTest::GTest GTest::Main)

    add_custom_target (run_${PROJECT_NAME}_tests 
        COMMENT "Running ${PROJECT_NAME}_tests"    
        COMMAND ./${PROJECT_NAME}_tests
    )
    add_dependencies (run_${PROJECT_NAME}_tests ${PROJECT_NAME}_tests)
    
    enable_testing ()
endif ()

#add_subdirectory (vector)

#sanitizers
option(SANITIZERS "Include sanitizers" ON)
if (SANITIZERS)
    message ("Build with sanitizers")

    add_compile_options (-fsanitize=address -fsanitize=undefined -g)
    set_target_properties (${PROJECT_NAME}_tests PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=undefined")
endif ()

#clang-format
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    message (clang-format " from " ${CMAKE_SOURCE_DIR}/.clang-format)

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp *.h *.c)
    add_custom_target(
        clang_format
        COMMAND clang-format
        -i
        -style=file:${CMAKE_SOURCE_DIR}/.clang-format
        ${ALL_SOURCE_FILES}
    )
endif()

