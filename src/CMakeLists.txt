cmake_minimum_required (VERSION 2.6)
project (Bio)

SET(OPENCV_CPP_SAMPLES_REQUIRED_DEPS opencv_core opencv_imgproc opencv_flann
    opencv_highgui opencv_ml opencv_video
    opencv_objdetect opencv_photo opencv_features2d opencv_calib3d
    opencv_stitching opencv_videostab )

set(DEMO demo)
set(SIFT sift)
set(CAMERA camera)

add_executable(${DEMO} ${DEMO}.cpp)
add_executable(${CAMERA} ${CAMERA}.cpp)

include_directories ("${PROJECT_SOURCE_DIR}/include")

add_subdirectory ("${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(${DEMO} ${SIFT})

target_link_libraries(${DEMO} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

target_link_libraries(${CAMERA} ${SIFT})

target_link_libraries(${CAMERA} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})


