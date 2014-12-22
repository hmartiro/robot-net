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
set(LIB_REDIS hiredis event)
set(LIB_ALL ${LIB_ZMQ})

# ---------------------------------------------------------
# Main program?
# ---------------------------------------------------------

add_executable(robot-net ${SRC_RNET_ALL} main.cpp)
target_link_libraries(robot-net ${LIB_ZMQ})

# ---------------------------------------------------------
# ZeroMQ Tests
# ---------------------------------------------------------

add_executable(zmq-pub tests/zmq-publisher.cpp)
target_link_libraries(zmq-pub ${LIB_ZMQ})

add_executable(zmq-sub tests/zmq-subscriber.cpp)
target_link_libraries(zmq-sub ${LIB_ZMQ})

# ---------------------------------------------------------
# Redis Tests
# ---------------------------------------------------------

add_executable(redis-set tests/redis-setter.cpp)
target_link_libraries(redis-set ${LIB_REDIS})

add_executable(redis-get tests/redis-getter.cpp)
target_link_libraries(redis-get ${LIB_REDIS})

add_executable(redis-set-async tests/redis-setter-async.cpp)
target_link_libraries(redis-set-async ${LIB_REDIS})

add_executable(redis-get-async tests/redis-getter-async.cpp)
target_link_libraries(redis-get-async ${LIB_REDIS})

add_executable(redis-pub tests/redis-publisher.cpp)
target_link_libraries(redis-pub ${LIB_REDIS})

add_executable(redis-sub tests/redis-subscriber.cpp)
target_link_libraries(redis-sub ${LIB_REDIS})

# ---------------------------------------------------------
# MessagePack Tests
# ---------------------------------------------------------

add_executable(msgpack-test tests/msgpack-test.cpp)

# ---------------------------------------------------------
# Redis C++ Wrapper Tests
# ---------------------------------------------------------

add_executable(redis-cpp-test tests/redis-cpp-test.cpp src/redis.cpp)
target_link_libraries(redis-cpp-test ${LIB_REDIS})

# ---------------------------------------------------------
# Node Tests
# ---------------------------------------------------------

add_executable(node-test ${SRC_ALL} tests/node-test.cpp)
target_link_libraries(node-test ${LIB_ALL})
