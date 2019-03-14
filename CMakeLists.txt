cmake_minimum_required(VERSION 2.8)
project( detecor )
find_package( OpenCV )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#set(SOURCES src/main.cpp src/Canvas.cpp)
message(STATUS ${OpenCV_LIBS})
message(STATUS ${OpenCV_INCLUDE_DIRS})
#
add_executable( detector src/main.cpp src/Canvas.cpp src/utils.cpp include/utils.h include/Canvas.h)
target_link_libraries( detector ${OpenCV_LIBS} )
target_include_directories(detector PUBLIC 
   ${OpenCV_INCLUDE_DIRS} 
   include
)
# include_directories("${PROJECT_SOURCE_DIR}")
