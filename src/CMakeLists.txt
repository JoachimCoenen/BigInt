add_library ("BigInt" bigInt.cpp bigInt.h utils.h)
add_executable ("demo" main.cpp)
target_link_libraries ("demo" BigInt)
