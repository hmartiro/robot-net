cmake_minimum_required(VERSION 2.8.4)
project(robot_net)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall")
# set(CMAKE_VERBOSE_MAKEFILE ON)

# ---------------------------------------------------------
# Source files
# ---------------------------------------------------------

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SRC_UTILS_DIR ${SRC_DIR}/utils)

set(SRC_CORE
  ${SRC_DIR}/node.cpp
)

set(SRC_UTILS
  ${SRC_UTILS_DIR}/ostreamlock.cpp
)

set(SRC_ALL
  ${SRC_CORE}
  ${SRC_UTILS}
)

# ---------------------------------------------------------
# Libraries
# ---------------------------------------------------------

set(LIB_ZMQ zmq zmqpp)
set(LIB_ALL ${LIB_ZMQ})

# ---------------------------------------------------------
# Main program?
# ---------------------------------------------------------

add_executable(robot_net ${SRC_RNET_ALL} main.cpp)

target_link_libraries(robot_net ${LIB_ZMQ})

# ---------------------------------------------------------
# ZeroMQ Tests
# ---------------------------------------------------------

add_executable(zmq-pub tests/zmq-publisher.cpp)
target_link_libraries(zmq-pub ${LIB_ZMQ})

add_executable(zmq-sub tests/zmq-subscriber.cpp)
target_link_libraries(zmq-sub ${LIB_ZMQ})

# ---------------------------------------------------------
# MessagePack Tests
# ---------------------------------------------------------

add_executable(msgpack-test tests/msgpack-test.cpp)

# ---------------------------------------------------------
# Node Tests
# ---------------------------------------------------------

add_executable(node-test ${SRC_ALL} tests/node-test.cpp)
target_link_libraries(node-test ${LIB_ALL})
