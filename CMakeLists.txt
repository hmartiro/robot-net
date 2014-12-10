cmake_minimum_required(VERSION 2.8.4)
project(robot_net)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall")
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(SOURCE_FILES main.cpp)
add_executable(robot_net ${SOURCE_FILES})

target_link_libraries(robot_net zmq zmqpp)

# ---------------------------------------------------------
# ZeroMQ Tests
# ---------------------------------------------------------

add_executable(zmq-pub tests/zmq-publisher.cpp)
target_link_libraries(zmq-pub zmq zmqpp)

add_executable(zmq-sub tests/zmq-subscriber.cpp)
target_link_libraries(zmq-sub zmq zmqpp)

# ---------------------------------------------------------
# MessagePack Tests
# ---------------------------------------------------------

add_executable(msgpack-test tests/msgpack-test.cpp)
