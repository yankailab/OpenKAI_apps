cmake_minimum_required(VERSION 3.18)
project(ACSL VERSION 0.0.1)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

OPTION(BUILD_AS_LIB "Build as library" OFF)
option(BUILD_SHARED_LIBS "Build shared library" ON)

OPTION(USE_GLOG "Use glog" ON)
OPTION(USE_ROS "Use ROS2 interface" OFF)
OPTION(USE_OPENCV "Use OpenCV" ON)

find_package(Eigen3 3.1.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(ACSL_definitions "-O3 -w -Wall -c -fmessage-length=0 -MMD -MP -std=c++17")
set(ACSL_definitions "-w -Wall -c -fmessage-length=0 -MMD -MP")
set(ACSL_libDir /usr/lib /usr/local/lib)
set(ACSL_include /usr/local/include /usr/include ${EIGEN3_INCLUDE_DIR})
set(ACSL_lib OpenKAI pthread boost_system uuid ncurses ssl ${EIGEN3_LIBS})

file(GLOB_RECURSE ACSL_cpp src/*.cpp src/*.cc src/*.c)
file(GLOB_RECURSE ACSL_FC_cpp src/FlightControl/*.cpp src/FlightControl/*.cc src/FlightControl/*.c)

set(ACSL_cpp ${ACSL_cpp} ${ACSL_FC_cpp})

# Optional modules
if(USE_GLOG)
  add_definitions(-DUSE_GLOG)
  set(ACSL_lib ${ACSL_lib} glog)
endif()

if(USE_OPENCV)
  add_definitions(-DUSE_OPENCV)
  set(OpenCV_include "/usr/local/include/opencv4" CACHE STRING "")
  set(OpenCV_lib "/usr/local/lib" CACHE STRING "")
  set(ACSL_include ${ACSL_include} ${OpenCV_include})
  set(ACSL_libDir ${ACSL_libDir} ${OpenCV_lib})
  set(ACSL_lib ${ACSL_lib} opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_videoio opencv_calib3d opencv_objdetect opencv_features2d opencv_dnn)
endif()

if(USE_OPENCV_CONTRIB)
  add_definitions(-DUSE_OPENCV_CONTRIB)
  set(ACSL_lib ${ACSL_lib} opencv_xobjdetect opencv_tracking opencv_aruco)
endif()

# Setting specific lib here
link_directories(${ACSL_libDir})

# packages
if(USE_ROS)
  find_package(ament_cmake REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(sensor_msgs REQUIRED)
  find_package(nav_msgs REQUIRED)

  ament_target_dependencies(ACSL rclcpp std_msgs sensor_msgs nav_msgs)
  ament_package()
endif()

# Definitions
add_definitions(${ACSL_definitions})

# Exe or Lib
if(BUILD_AS_LIB)
add_library(ACSL ${ACSL_cpp})
else()
add_executable(ACSL ${ACSL_cpp})
endif() # build_as_lib

# C/C++ settings
include_directories(${ACSL_include})
target_link_libraries(ACSL ${ACSL_lib})


include(GNUInstallDirs)
set_target_properties(ACSL PROPERTIES
    VERSION ${PROJECT_VERSION})

install(TARGETS ACSL
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ACSL
    FILES_MATCHING
    PATTERN "*.h"
)
