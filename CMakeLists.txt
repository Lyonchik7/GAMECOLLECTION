cmake_minimum_required(VERSION 3.16)
project(GameCollection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)  # Убрал Multimedia

set(SOURCES
    main.cpp
    mainwindow.cpp
    TicTacToe.cpp
    Snake.cpp
    Minessweeper.cpp
    Game2048.cpp
    SimpleMario.cpp
    # SoundManager.cpp удален
)

set(HEADERS
    mainwindow.h
    TicTacToe.h
    Snake.h
    Minessweeper.h
    Game2048.h
    SimpleMario.h
    # SoundManager.h удален
)

add_executable(GameCollection ${SOURCES} ${HEADERS})

target_link_libraries(GameCollection Qt6::Core Qt6::Widgets)  # Убрал Qt6::Multimedia

if(WIN32)
    set_target_properties(GameCollection PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
