cmake_minimum_required(VERSION 3.2.2)
project(grandeur)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp
        constants.h
        move_notifier.h
        move.cpp move.h
        gems.cpp gems.h
        card.cpp card.h
        board.cpp board.h
        noble.cpp noble.h
        config.cpp config.h
        logger.cpp logger.h
        player.cpp player.h
        random_player.cpp random_player.h
        text_player.cpp text_player.h)

add_executable(grandeur ${SOURCE_FILES})

add_subdirectory(tests)