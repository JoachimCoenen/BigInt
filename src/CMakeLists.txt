add_executable ("demo" main.cpp  bigint/bigInt.h bigint/utils.h)

if (BIGINT_TRACY_ENABLE)
	target_link_libraries(demo PUBLIC
		TracyClient
	)
endif()
