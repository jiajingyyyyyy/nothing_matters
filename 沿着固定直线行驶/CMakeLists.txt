cmake_minimum_required(VERSION 3.0)
project(KF)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src DIR_SRCS)

add_executable(KF ${DIR_SRCS})

target_link_libraries(KF  
    ${EIGEN3_LIBRARIES}
    ${OpenCV_LIBRARIES}
)
