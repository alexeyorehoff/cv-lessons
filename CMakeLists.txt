set(CURRENT_TARGET lab1)

set(lab1_DEPS lab1/sprite.h lab1/app.h)

cmake_minimum_required(VERSION 3.24)
project(cv_lessons)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR C:/libs/opencv/build-debug)
find_package(OpenCV REQUIRED)


include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${CURRENT_TARGET}
)


add_executable(${CURRENT_TARGET} ${CURRENT_TARGET}/main.cpp ${${CURRENT_TARGET}_DEPS} lab1/app.h)
target_link_libraries(${CURRENT_TARGET} ${OpenCV_LIBS})