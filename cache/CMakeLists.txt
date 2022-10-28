cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(PROJECT_NAME caches)
project(${PROJECT_NAME} LANGUAGES CXX)

include_directories(caches/include parser/include)

set(SOURCE_TEST tests/tests.cpp)
set(SOURCE_PARSER parser/src/parser.cpp)

add_compile_options(-g)

#sanitizers
option(SANITIZERS "Include sanitizers" ON)
if (SANITIZERS)
    message ("Build with sanitizers")

    add_compile_options (-fsanitize=address -fsanitize=undefined -g)
endif ()

add_library(parser STATIC ${SOURCE_PARSER})

add_definitions(-DPROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\")

#tests
option(TESTING "Include testing" ON)
if (TESTING)
    message ("Include testing")
    find_package (GTest REQUIRED)
    include_directories (${GTEST_INCLUDE_DIRS})

    add_executable (${PROJECT_NAME}_tests ${SOURCE_TEST})
    if (SANITIZERS)
        set_target_properties (${PROJECT_NAME}_tests PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=undefined")
    endif ()

    target_link_libraries (${PROJECT_NAME}_tests PUBLIC GTest::GTest GTest::Main parser)
    enable_testing ()

    add_dependencies (${PROJECT_NAME}_tests parser)

    add_custom_target (run_${PROJECT_NAME}_tests 
        COMMENT "Running ${PROJECT_NAME}_tests"    
        COMMAND ./${PROJECT_NAME}_tests
    )
    add_dependencies (run_${PROJECT_NAME}_tests ${PROJECT_NAME}_tests)
endif ()


