FetchContent_Declare(
	catch
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.4
)
FetchContent_MakeAvailable(catch)

add_executable(tests main.cpp eval.cpp search.cpp "../src/agents/robin.cpp" "../src/ai/eval.cpp" "../src/ai/state_of_mind.cpp")
add_executable(scenario_tests scenarios.cpp "../src/agents/robin.cpp" "../src/ai/eval.cpp" "../src/ai/state_of_mind.cpp")

target_link_libraries(tests PRIVATE Catch2::Catch2 libsnake)
target_include_directories(tests PRIVATE ${libsnake_SOURCE_DIR}/include/)
target_compile_features(tests PRIVATE cxx_std_20)

target_link_libraries(scenario_tests PRIVATE Catch2::Catch2 libsnake)
target_include_directories(scenario_tests PRIVATE ${libsnake_SOURCE_DIR}/include/)
target_compile_features(scenario_tests PRIVATE cxx_std_20)

add_test(NAME snaketests COMMAND tests)
add_test(NAME scenario_tests COMMAND scenario_tests)

if (WIN32)
	add_custom_command(TARGET tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
		COMMAND_EXPAND_LISTS
	)
endif()