cmake_minimum_required(VERSION 3.16)
project(draw2_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(test_remote_deck test_remote_deck.cpp ../src/RemoteDeck.cpp)
target_include_directories(test_remote_deck PRIVATE ../src)
target_link_libraries(test_remote_deck PRIVATE Qt6::Core Qt6::Network)

enable_testing()
add_test(NAME remote_deck COMMAND test_remote_deck)
