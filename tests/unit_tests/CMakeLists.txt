include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${grandeur_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(runGrandeurTests
        testGems.cpp ${grandeur_SOURCE_DIR}/gems.cpp
        testCards.cpp ${grandeur_SOURCE_DIR}/card.cpp
        testBoard.cpp ${grandeur_SOURCE_DIR}/board.cpp)

target_link_libraries(runGrandeurTests gtest gtest_main)