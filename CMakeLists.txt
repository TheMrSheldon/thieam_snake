cmake_minimum_required(VERSION 3.14)

project(thieam_snake VERSION 0.1.0)

include(FetchContent)
include(CTest)
enable_testing()

set(THIEAM_SOURCE_FILES "src/agents/robin.cpp" "src/ai/eval.cpp" "src/ai/state_of_mind.cpp" "src/interface/cli_controller.cpp")

add_executable(thieam_snake "src/main.cpp" ${THIEAM_SOURCE_FILES})
target_compile_features(thieam_snake PRIVATE cxx_std_20)

include_directories(PUBLIC include)

if (WIN32)
	add_custom_command(TARGET thieam_snake POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:thieam_snake> $<TARGET_FILE_DIR:thieam_snake>
		COMMAND_EXPAND_LISTS
	)
endif()

##########################################################################################
# Libraries
##########################################################################################

FetchContent_Declare(
	libsnake_lib
	GIT_REPOSITORY https://github.com/TheMrSheldon/libsnake.git
)
FetchContent_MakeAvailable(libsnake_lib)
FetchContent_GetProperties(libsnake_lib)
target_include_directories(thieam_snake PRIVATE ${libsnake_SOURCE_DIR}/include/)
target_link_libraries(thieam_snake PRIVATE libsnake)

set(OATPP_BUILD_TESTS OFF) #We do not want oatpp to run their tests in our project
option(OATPP_BUILD_TESTS OFF)
FetchContent_Declare(
	oatpp_lib
	GIT_REPOSITORY https://github.com/oatpp/oatpp.git
	GIT_TAG 1.3.0
)
FetchContent_MakeAvailable(oatpp_lib)
FetchContent_GetProperties(oatpp_lib)
target_include_directories(thieam_snake PRIVATE ${oatpp_lib_SOURCE_DIR}/src/)
target_link_libraries(thieam_snake PRIVATE oatpp)

FetchContent_Declare(
	cli_lib
	GIT_REPOSITORY https://github.com/TheMrSheldon/cli.git
)
FetchContent_MakeAvailable(cli_lib)
FetchContent_GetProperties(cli_lib)
target_include_directories(thieam_snake PRIVATE ${cli_lib_SOURCE_DIR}/include/)

##########################################################################################
# Testing
##########################################################################################
# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
	add_subdirectory(tests)
endif()
