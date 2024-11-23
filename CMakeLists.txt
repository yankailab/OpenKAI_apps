cmake_minimum_required(VERSION 3.18)
project(OKapps VERSION 0.0.1)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

OPTION(USE_GLOG "Use glog" ON)
OPTION(USE_ROS "Use ROS2 interface" OFF)
OPTION(USE_OPEN3D "Use Open3D" OFF)
OPTION(USE_OPENCV "Use OpenCV" OFF)

OPTION(WITH_DRONEBOX_AIR "" OFF)
OPTION(WITH_DRONEBOX_GND "" OFF)
OPTION(WITH_FASTLIO_SCAN "" OFF)
OPTION(WITH_GSV "" OFF)
OPTION(WITH_HELLO_AP "" OFF)
OPTION(WITH_TGRS "" OFF)

find_package(Eigen3 3.1.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(OKapps_definitions "-O3 -w -Wall -c -fmessage-length=0 -MMD -MP -std=c++17")
set(OKapps_definitions "-w -Wall -c -fmessage-length=0 -MMD -MP")
set(OKapps_libDir /usr/lib /usr/local/lib)
set(OKapps_include /usr/local/include /usr/include ${EIGEN3_INCLUDE_DIR})
set(OKapps_lib OpenKAI pthread boost_system uuid ncurses ssl ${EIGEN3_LIBS})

# Optional modules
if(USE_GLOG)
  add_definitions(-DUSE_GLOG)
  set(OKapps_lib ${OKapps_lib} glog)
endif()

if(USE_OPEN3D)
  add_definitions(-DUSE_OPEN3D)
  set(Open3D_jsoncpp "/usr/include/jsoncpp" CACHE STRING "")
  set(OKapps_include ${OKapps_include} ${Open3D_jsoncpp})
  set(OKapps_lib ${OKapps_lib} Open3D)
endif()

if(USE_OPENCV)
  add_definitions(-DUSE_OPENCV)
  set(OpenCV_include "/usr/local/include/opencv4" CACHE STRING "")
  set(OpenCV_lib "/usr/local/lib" CACHE STRING "")
  set(OKapps_include ${OKapps_include} ${OpenCV_include})
  set(OKapps_libDir ${OKapps_libDir} ${OpenCV_lib})
  set(OKapps_lib ${OKapps_lib} opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_videoio opencv_calib3d opencv_objdetect opencv_features2d opencv_dnn)
endif()

if(USE_OPENCV_CONTRIB)
  add_definitions(-DUSE_OPENCV_CONTRIB)
  set(OKapps_lib ${OKapps_lib} opencv_xobjdetect opencv_tracking opencv_aruco)
endif()

# Setting specific lib here
link_directories(${OKapps_libDir})

# Definitions
add_definitions(${OKapps_definitions})

# C/C++ settings
include_directories(${OKapps_include})

# common
#file(GLOB_RECURSE OKapps_cpp src/*.cpp src/*.cc src/*.c)

# apps
if(WITH_DRONEBOX_AIR)
add_definitions(-DWITH_APP_DRONEBOX_AIR)
SET(USE_OPENCV ON CACHE BOOL "" FORCE)
SET(USE_OPENCV_CONTRIB ON CACHE BOOL "" FORCE)
set(AppDroneBoxAir_cpp ${AppDroneBoxAir_cpp} src/DroneBox/_AP_droneBox.cpp)
set(AppDroneBoxAir_cpp ${AppDroneBoxAir_cpp} src/DroneBox/_AP_droneBoxJSON.cpp)
set(AppDroneBoxAir_cpp ${AppDroneBoxAir_cpp} src/DroneBox/AP_droneBoxMain.cpp)
add_executable(DroneBoxAir ${OKapps_cpp} ${AppDroneBoxAir_cpp})
target_link_libraries(DroneBoxAir ${OKapps_lib})
endif()


if(WITH_DRONEBOX_GND)
add_definitions(-DWITH_APP_DRONEBOX_GND)
set(AppDroneBoxGnd_cpp ${AppDroneBoxGnd_cpp} src/DroneBox/_DroneBox.cpp)
set(AppDroneBoxGnd_cpp ${AppDroneBoxGnd_cpp} src/DroneBox/_DroneBoxJSON.cpp)
set(AppDroneBoxGnd_cpp ${AppDroneBoxGnd_cpp} src/DroneBox/DroneBoxMain.cpp)
add_executable(DroneBoxGnd ${OKapps_cpp} ${AppDroneBoxGnd_cpp})
target_link_libraries(DroneBoxGnd ${OKapps_lib})
endif()


if(WITH_GSV)
add_definitions(-DWITH_APP_GSV)
file(GLOB_RECURSE AppGSV_cpp src/Application/GSV/*.cpp src/Application/GSV/*.cc src/Application/GSV/*.c)
add_executable(GSV ${OKapps_cpp} ${AppGSV_cpp})
target_link_libraries(GSV ${OKapps_lib})
endif()


if(WITH_HELLO_AP)
file(GLOB_RECURSE AppHelloAP_cpp src/helloAP/*.cpp)
add_executable(helloAP ${OKapps_cpp} ${AppHelloAP_cpp})
target_link_libraries(helloAP ${OKapps_lib})
endif()


if(WITH_FASTLIO_SCAN)
set(OKapps_cpp_fastLioScan src/fastLioScan/scan.cpp src/fastLioScan/_fastLioScan.cpp)
add_executable(fastLioScan ${OKapps_cpp} ${OKapps_cpp_fastLioScan})
target_link_libraries(fastLioScan ${OKapps_lib})

set(OKapps_cpp_fastLioRGB src/fastLioScan/rgb.cpp src/fastLioScan/_fastLioRGB.cpp)
add_executable(fastLioRGB ${OKapps_cpp} ${OKapps_cpp_fastLioRGB})
target_link_libraries(fastLioRGB ${OKapps_lib})
endif()


if(WITH_TGRS)
file(GLOB_RECURSE AppTGRS_cpp src/TGRS/*.cpp)
add_executable(TGRS ${OKapps_cpp} ${AppTGRS_cpp})
target_link_libraries(TGRS ${OKapps_lib})
endif()


if(USE_ROS)
  find_package(ament_cmake REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(sensor_msgs REQUIRED)
  find_package(nav_msgs REQUIRED)

  ament_target_dependencies(fastLioScan rclcpp std_msgs sensor_msgs nav_msgs)
  ament_package()
endif()


include(GNUInstallDirs)
#set_target_properties(OKapps PROPERTIES VERSION ${PROJECT_VERSION})
