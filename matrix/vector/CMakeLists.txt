include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PROJECT_NAME vector)

#tests
option(TESTING "Include testing" ON)
if (TESTING)
    message ("Include " ${PROJECT_NAME} " testing")
    
    set(SOURCE_TEST tests/tests.cpp)
    include_directories(include)
    #    add_definitions(-DPROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\")
    
    find_package (GTest REQUIRED)
    include_directories (${GTEST_INCLUDE_DIRS})
    
    add_executable (${PROJECT_NAME}_tests ${SOURCE_TEST})
    target_link_libraries (${PROJECT_NAME}_tests PRIVATE GTest::GTest GTest::Main)

    add_custom_target (run_${PROJECT_NAME}_tests 
        COMMENT "Running ${PROJECT_NAME}_tests"    
        COMMAND ./${PROJECT_NAME}_tests
    )
    add_dependencies (run_${PROJECT_NAME}_tests ${PROJECT_NAME}_tests)
    
    enable_testing ()
endif ()

