add_executable ("demo" main.cpp)

if (BIGINT_TRACY_ENABLE)
	target_link_libraries(demo PUBLIC
		TracyClient
	)
endif()
