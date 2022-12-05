cmake_minimum_required(VERSION 3.8)
project(src)

set(CMAKE_CXX_STANDARD 17)

add_executable(src
        main.cpp
        RegularExpression.cpp RegularExpression.h
        SMGraph.cpp SMGraph.h
        StateMachine.cpp StateMachine.h
)
