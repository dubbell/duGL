
FetchContent_Declare(
	glad-generator
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
	GIT_TAG v2.0.8
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glad-generator)

set(GLAD_VENV_DIR ${CMAKE_BINARY_DIR}/glad-venv)

execute_process(
	COMMAND ${Python3_EXECUTABLE} -m venv ${GLAD_VENV_DIR}
	RESULT_VARIABLE venv_result)

if(NOT venv_result EQUAL 0)
	message(FATAL_ERROR "Failed to create Python virtual environment")
endif()

if(WIN32)
	set(GLAD_PYTHON ${GLAD_VENV_DIR}/Scripts/python.exe)
else()
	set(GLAD_PYTHON ${GLAD_VENV_DIR}/bin/python)
endif()

execute_process(
	COMMAND ${GLAD_PYTHON} -m pip install jinja2
	RESULT_VARIABLE pip_result)

if(NOT pip_result EQUAL 0)
	message(FATAL_ERROR "Failed to install jinja2 into virtual environment")
endif()

execute_process(
	COMMAND ${GLAD_PYTHON} -m glad
		--out-path ${CMAKE_BINARY_DIR}/glad
		--api gl:core=4.6
		c
	WORKING_DIRECTORY ${glad-generator_SOURCE_DIR}
	RESULT_VARIABLE glad_result)

if(NOT glad_result EQUAL 0)
	message(FATAL_ERROR "GLAD generation failed")
endif()

add_library(glad STATIC ${CMAKE_BINARY_DIR}/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_BINARY_DIR}/glad/include)
