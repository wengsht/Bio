cmake_minimum_required (VERSION 2.6)
project (Bio)

set(DEMO demo)
set(SIFT sift)

add_executable(${DEMO} ${DEMO}.cpp)

include_directories ("${PROJECT_SOURCE_DIR}/include")

add_subdirectory ("${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(${DEMO} ${SIFT})
