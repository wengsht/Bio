cmake_minimum_required (VERSION 2.6)
project (Bio)

SET(OPENCV_CPP_SAMPLES_REQUIRED_DEPS opencv_core opencv_imgproc opencv_flann
    opencv_highgui opencv_ml opencv_video
    opencv_objdetect opencv_photo opencv_features2d opencv_calib3d
    opencv_stitching opencv_videostab )

set(DEMO demo)
set(SIFT sift)
set(CAMERA camera)
set(MATCH_DEMO match2img)
set(DATABASE_DEMO match2db)

set(ACCU accuracyTest)

set(DETECT detect)

set(IMG_DIR img)
set(HAAR_DIR haarcascades)

add_executable(${DEMO} ${DEMO}.cpp)
add_executable(${CAMERA} ${CAMERA}.cpp)
add_executable(${MATCH_DEMO} ${MATCH_DEMO}.cpp)
add_executable(${DATABASE_DEMO} ${DATABASE_DEMO}.cpp)

add_executable(${DETECT} ${DETECT}.cpp)

add_executable(${ACCU} ${ACCU}.cpp)

include_directories ("${PROJECT_SOURCE_DIR}/include")

add_subdirectory ("${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(${DEMO} ${SIFT})
target_link_libraries(${DEMO} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

target_link_libraries(${CAMERA} ${SIFT})
target_link_libraries(${CAMERA} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})
target_link_libraries(${CAMERA} pthread)

target_link_libraries(${MATCH_DEMO} ${SIFT})
target_link_libraries(${MATCH_DEMO} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

target_link_libraries(${DATABASE_DEMO} ${SIFT})
target_link_libraries(${DATABASE_DEMO} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

target_link_libraries(${DETECT} ${SIFT})
target_link_libraries(${DETECT} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

target_link_libraries(${ACCU} ${SIFT})
target_link_libraries(${ACCU} ${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})

file(COPY ${PROJECT_SOURCE_DIR}/../${IMG_DIR}/ DESTINATION ${PROJECT_BINARY_DIR}/${IMG_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../${HAAR_DIR}/ DESTINATION ${PROJECT_BINARY_DIR}/${HAAR_DIR}/)
