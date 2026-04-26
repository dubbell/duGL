set(STB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/stb)
file(MAKE_DIRECTORY ${STB_INCLUDE_DIR})
file(DOWNLOAD
	 https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	 ${STB_INCLUDE_DIR}/stb_image.h
	 STATUS stb_download_status)
list(GET stb_download_status 0 stb_download_result)
if(NOT stb_download_result EQUAL 0)
	message(FATAL_ERROR "Failed to download stb_image.h")
endif()
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})