set(CURRENT_TARGET lab1_2)

set(lab1_INCL lab1)
set(lab1_DEPS lab1/sprite.h lab1/app.h)

set(lab1_2_INCL lab1 lab1_2)
set(lab1_2_DEPS lab1/sprite.h lab1/app.h)

set(lab2_INCL lab2)
set(lab2_DEPS)

cmake_minimum_required(VERSION 3.24)
project(cv_lessons)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR C:/libs/opencv/build-debug)
find_package(OpenCV REQUIRED)


include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${${CURRENT_TARGET}_INCL}
)


add_executable(${CURRENT_TARGET} ${CURRENT_TARGET}/${CURRENT_TARGET}_main.cpp ${${CURRENT_TARGET}_DEPS})
target_link_libraries(${CURRENT_TARGET} ${OpenCV_LIBS})