add_library(error_detection STATIC error_detection.cc)
target_include_directories(error_detection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
