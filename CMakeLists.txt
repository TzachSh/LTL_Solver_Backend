cmake_minimum_required(VERSION 3.15)
project(C___Project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)
include_directories(include)
include_directories(src)

find_library(spot spot)
find_library(z3 z3)
link_libraries(spot)
link_libraries(z3)

add_executable(C___Project
        include/Automaton.h
        include/OlgChecker.h
        include/Constants.h
        include/NormalForm.h
        include/Obligation.h
        include/ObligationFormula.h
        include/ObligationSet.h
        include/Parser.h
        include/State.h
        include/WeakSatisfactionChecker.h
        src/Automaton.cpp
        src/OlgChecker.cpp
        src/main.cpp
        src/NormalForm.cpp
        src/Obligation.cpp
        src/ObligationFormula.cpp
        src/ObligationSet.cpp
        src/Parser.cpp
        src/State.cpp
        src/WeakSatisfactionChecker.cpp
        src/Notations.cpp
        src/TransitionsSystem.cpp src/ServerHandler.cpp include/ServerHandler.h)
