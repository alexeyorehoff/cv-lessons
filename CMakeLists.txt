cmake_minimum_required(VERSION 3.24)
project(cv_lessons)

set(CMAKE_CXX_STANDARD 20)

#set(OpenCV_DIR D:/libs/opencv/debug)
set(OpenCV_DIR C:/libs/opencv/opencv-debug)

find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories()

add_executable(lab1 lab1/lab1_main.cpp lab1/sprite.h lab1/app.h)
target_link_libraries(lab1 ${OpenCV_LIBS})

add_executable(lab1_2 lab1/lab1_2_main.cpp lab1/sprite.h lab1/app.h)
target_link_libraries(lab1_2 ${OpenCV_LIBS})

add_executable(lab2 lab2/lab2_main.cpp)
target_link_libraries(lab2 ${OpenCV_LIBS})

add_executable(lab3_1 lab3/lab3_1.cpp)
target_link_libraries(lab3_1 ${OpenCV_LIBS})

add_executable(lab3_2 lab3/lab3_2.cpp)
target_link_libraries(lab3_2 ${OpenCV_LIBS})

add_executable(lab3_3 lab3/lab3_3.cpp)
target_link_libraries(lab3_3 ${OpenCV_LIBS})

add_executable(lab3_4 lab3/lab3_4.cpp)
target_link_libraries(lab3_4 ${OpenCV_LIBS})

add_executable(lab4 lab4/lab4_main.cpp)
target_link_libraries(lab4 ${OpenCV_LIBS})

add_executable(lab5 lab5/lab5_main.cpp lab5/texture_render.h lab5/shader.h lab5/window.h)
target_link_libraries(lab5 ${OpenCV_LIBS} glm::glm opengl32.lib GLEW::GLEW glfw)

add_executable(gl-report gl-report/main.cpp)
target_link_libraries(gl-report opengl32.lib GLEW::GLEW glfw)

add_executable(lab6 lab6/lab6_main.cpp)
target_link_libraries(lab6 ${OpenCV_LIBS})

