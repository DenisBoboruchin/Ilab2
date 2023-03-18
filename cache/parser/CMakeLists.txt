include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include include)

set(SOURCE_PARSER src/parser.cpp)

add_library(parser STATIC ${SOURCE_PARSER})

if (SANITIZERS)
    set_target_properties (parser PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=undefined")
endif ()

