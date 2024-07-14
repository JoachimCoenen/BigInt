cmake_minimum_required(VERSION 3.14)
project ("BigInt")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add sources & test sources:
add_subdirectory (src) 
add_subdirectory (test)
