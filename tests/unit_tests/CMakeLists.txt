include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${grandeur_SOURCE_DIR})

add_executable(runGrandeurTests testGems.cpp)

target_link_libraries(runGrandeurTests gtest gtest_main)