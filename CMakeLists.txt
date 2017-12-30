cmake_minimum_required(VERSION 3.9)
project(network_simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(network_simulator
        src/main.cpp
        src/buffer.cpp
        src/buffer.hpp
        src/client.cpp
        src/client.hpp
        src/event.cpp
        src/event.hpp
        src/generator.cpp
        src/generator.hpp
        src/node.cpp
        src/node.hpp
        src/router.cpp
        src/router.hpp
        src/server.cpp
        src/server.hpp
        src/simulator.cpp
        src/simulator.hpp)