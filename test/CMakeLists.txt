# load googletest:
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY    https://github.com/google/googletest.git
	GIT_TAG           v1.15.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable("Test_BigInt"
	test_bigint.cpp
	values_for_test.h
	utils_for_test.h
	data_loader.h
)

if (BIGINT_TRACY_ENABLE)
	target_link_libraries(Test_BigInt PUBLIC
		GTest::gtest_main
		TracyClient
	)
else()
	target_link_libraries(Test_BigInt PUBLIC
		GTest::gtest_main
	)
endif()


include(GoogleTest)
gtest_discover_tests(Test_BigInt)
