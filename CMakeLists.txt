cmake_minimum_required(VERSION 2.8)
project( PupilDetecor )
find_package( OpenCV )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#set(SOURCES src/main.cpp src/Canvas.cpp)

add_executable( PupilDetector src/main.cpp src/Canvas.cpp include/Canvas.h)
target_link_libraries( PupilDetector ${OpenCV_LIBS} )
target_include_directories(PupilDetector PUBLIC 
   ${OpenCV_INCLUDE_DIRS} 
   include
)
# include_directories("${PROJECT_SOURCE_DIR}")
