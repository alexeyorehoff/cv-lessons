cmake_minimum_required(VERSION 3.24)
project(cv_lessons)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR C:/libs/opencv/build-debug)
find_package(OpenCV REQUIRED)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
        lab1 lab1_2 lab2 lab3
)

add_executable(lab1 lab1/lab1_main.cpp lab1/sprite.h lab1/app.h)
add_executable(lab1_2 lab1_2/lab1_2_main.cpp lab1/sprite.h lab1/app.h)
add_executable(lab2 lab2/lab2_main.cpp)
add_executable(lab3_1 lab3/lab3_1.cpp)
add_executable(lab3_2 lab3/lab3_2.cpp)
add_executable(lab3_3 lab3/lab3_3.cpp)

target_link_libraries(lab1 ${OpenCV_LIBS})
target_link_libraries(lab1_2 ${OpenCV_LIBS})
target_link_libraries(lab2 ${OpenCV_LIBS})
target_link_libraries(lab3_1 ${OpenCV_LIBS})
target_link_libraries(lab3_2 ${OpenCV_LIBS})
target_link_libraries(lab3_3 ${OpenCV_LIBS})