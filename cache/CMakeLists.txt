cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

set(PROJECT_NAME caches)
project(${PROJECT_NAME} LANGUAGES CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER g++)

set(CACHE_LFU_SOURCE src/cache_lfu.cpp)
set(CACHE_LRU_SOURCE src/cache_lru.cpp)
set(CACHE_PERFECT_SOURCE src/cache_perfect.cpp)

include_directories(include parser/include) 

#cache_lfu
add_executable (cache_lfu ${CACHE_LFU_SOURCE})
target_link_libraries (cache_lfu PRIVATE parser)
add_custom_target (run_cache_lfu 
    COMMAND ./cache_lfu
)
add_dependencies (run_cache_lfu cache_lfu)

#cache_lru
add_executable (cache_lru ${CACHE_LRU_SOURCE})
target_link_libraries (cache_lru PRIVATE parser)
add_custom_target (run_cache_lru 
    COMMAND ./cache_lru
)
add_dependencies (run_cache_lru cache_lru)

#cache_perfect
add_executable (cache_perfect ${CACHE_PERFECT_SOURCE})
target_link_libraries (cache_perfect PRIVATE parser)
add_custom_target (run_cache_perfect 
    COMMAND ./cache_perfect
)
add_dependencies (run_cache_perfect cache_perfect)

#tests
option(TESTING "Include testing" ON)
if (TESTING)
    message ("Include testing")
    
    set(SOURCE_TEST tests/tests.cpp)
    add_definitions(-DPROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\")
    
    find_package (GTest REQUIRED)
    include_directories (${GTEST_INCLUDE_DIRS})
    
    add_executable (${PROJECT_NAME}_tests ${SOURCE_TEST})
    target_link_libraries (${PROJECT_NAME}_tests PRIVATE GTest::GTest GTest::Main parser)

    add_custom_target (run_${PROJECT_NAME}_tests 
        COMMENT "Running ${PROJECT_NAME}_tests"    
        COMMAND ./${PROJECT_NAME}_tests
    )
    add_dependencies (run_${PROJECT_NAME}_tests ${PROJECT_NAME}_tests)
    
    enable_testing ()
endif ()

add_subdirectory (parser)

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

