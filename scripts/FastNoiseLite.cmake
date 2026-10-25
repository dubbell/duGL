set(FASTNOISELITE_COMMIT 7ccfbc16eb1c932568f177d63a9ba51d89bbe516)
set(FASTNOISELITE_SHA256 47a29750dfe13d15651febd235f3c26b1cc57af7c6e26583b838fd7ab5215dc2)

set(FASTNOISELITE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FastNoiseLite)
file(MAKE_DIRECTORY ${FASTNOISELITE_INCLUDE_DIR})
file(DOWNLOAD
	 https://raw.githubusercontent.com/Auburn/FastNoiseLite/${FASTNOISELITE_COMMIT}/Cpp/FastNoiseLite.h
	 ${FASTNOISELITE_INCLUDE_DIR}/FastNoiseLite.h
	 EXPECTED_HASH SHA256=${FASTNOISELITE_SHA256}
	 STATUS fastnoiselite_download_status)
list(GET fastnoiselite_download_status 0 fastnoiselite_download_result)
if(NOT fastnoiselite_download_result EQUAL 0)
	list(GET fastnoiselite_download_status 1 fastnoiselite_download_message)
	message(FATAL_ERROR "Failed to download FastNoiseLite.h: ${fastnoiselite_download_message}")
endif()
add_library(FastNoiseLite INTERFACE)
target_include_directories(FastNoiseLite INTERFACE ${FASTNOISELITE_INCLUDE_DIR})
