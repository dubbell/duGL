set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "ProgramDatabase" CACHE STRING "" FORCE)
FetchContent_Declare(
	JoltPhysics
	GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
	GIT_TAG v5.5.0
	GIT_SHALLOW TRUE
	SOURCE_SUBDIR Build)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(JoltPhysics)
