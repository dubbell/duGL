# Pinned to a specific commit (rather than `master`) and verified by hash so the build is
# reproducible and the downloaded header can't silently change contents between builds.
set(STB_IMAGE_COMMIT 2c980bb59875b0d32144a71867fbdebb2f77cd20)
set(STB_IMAGE_SHA256 594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3)

set(STB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/stb)
file(MAKE_DIRECTORY ${STB_INCLUDE_DIR})
file(DOWNLOAD
	 https://raw.githubusercontent.com/nothings/stb/${STB_IMAGE_COMMIT}/stb_image.h
	 ${STB_INCLUDE_DIR}/stb_image.h
	 EXPECTED_HASH SHA256=${STB_IMAGE_SHA256}
	 STATUS stb_download_status)
list(GET stb_download_status 0 stb_download_result)
if(NOT stb_download_result EQUAL 0)
	list(GET stb_download_status 1 stb_download_message)
	message(FATAL_ERROR "Failed to download stb_image.h: ${stb_download_message}")
endif()
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})