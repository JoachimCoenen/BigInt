cmake_minimum_required(VERSION 3.24)
project ("BigInt")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0135 NEW)


# USER CONFIGURATION
# ==================
set(BIGINT_TRACY_ENABLE FALSE CACHE BOOL
	"set to true if you want to use the Tracy profiler to profile BigInt operations. This will add Tracy to the project using FetchContent_Declare(...).\n you will still have to link 'TracyClient' with your target: target_link_libraries(YourTargetNameHere TracyClient).")
set(BIGINT_TRACY_GIT_REPO "https://github.com/wolfpld/tracy.git" CACHE STRING
	"The Git repositoryf where tracy can be found.")
set(BIGINT_TRACY_VERSION "v0.11.0" CACHE STRING
	"The version of tracy to use (e.g. v0.11.0). must be a valifd git tag in the tracy git repository.")


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
		add_compile_options(-fsanitize=undefined -fsanitize-undefined-trap-on-error -pedantic)
		add_link_options(-fsanitize=undefined -fsanitize-undefined-trap-on-error -pedantic)
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


# DEPENDENCIES
# ============

if (BIGINT_TRACY_ENABLE)
	include(FetchContent)
	FetchContent_Declare(
		tracy
		GIT_REPOSITORY ${BIGINT_TRACY_GIT_REPO}
		GIT_TAG        ${BIGINT_TRACY_VERSION}
		GIT_SHALLOW    TRUE
		GIT_PROGRESS   TRUE
	)
	FetchContent_MakeAvailable(tracy)

	add_compile_definitions(TRACY_ENABLE BIGINT_TRACY_ENABLE)
endif()


# add sources & test sources
# ==========================
add_subdirectory (src)
add_subdirectory (test)
