cmake_minimum_required(VERSION 3.24)
project ("BigInt")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0135 NEW)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# using Clang
	add_compile_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -pedantic)
	add_link_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# using GCC
	if(MINGW)
		# no address sanitazer for gcc on windows :(
		# no libubsan rich logging capabilities for gcc on windows :(
		# no sanitazer at all for gcc on windows :'(
		add_compile_options(-fsanitize=undefined -fsanitize-undefined-trap-on-error)
		add_link_options(-fsanitize=undefined -fsanitize-undefined-trap-on-error)
	else()
		add_compile_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -pedantic)
		add_link_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -pedantic)
	endif()
# elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
#   # using Intel C++. Not yets supported
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# using Visual Studio C++
	add_compile_options(/fsanitize=address /W4)
	add_link_options(/fsanitize=address /W4)
endif()


# add sources & test sources:
add_subdirectory (src) 
add_subdirectory (test)
