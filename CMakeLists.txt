cmake_minimum_required(VERSION 3.22)
project(project)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(doctest)
add_executable(tests tests.cpp func.cpp func.h)
target_link_libraries(tests sfml-graphics doctest)
add_test(NAME test COMMAND test --force-colors -d)
add_custom_target(cloud-test COMMAND test --force-colors -d)
add_executable(project main.cpp)
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS window network graphics audio REQUIRED)
target_link_libraries(project sfml-network sfml-window sfml-graphics sfml-audio)



