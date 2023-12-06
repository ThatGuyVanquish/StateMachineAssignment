cmake_minimum_required(VERSION 3.26)
project(State_machine)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(State_machine
        event.h
        main.cpp
        stateHandler.h
        stateMachine.h
)
