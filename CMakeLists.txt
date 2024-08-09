cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(Microscope LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "/opt/Qt5.12.2/5.12.2/gcc_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS

find_package(OpenCV REQUIRED)
set(OpenCV_DIR "/home/zxy/opencv/install/lib")
set(FFmpeng_LIB "/home/zxy/tools/ffmpeg/install/lib")
set(FFmpeng_INC "/home/zxy/tools/ffmpeg/install/include")

file(GLOB_RECURSE FFmpeng_FILE "/home/zxy/tools/ffmpeg/install/lib/*.so")
file(GLOB_RECURSE ALL_SOURCE "src/*.cpp" "src/*.c")
file(GLOB_RECURSE ALL_HEAD "src/*.hpp" "src/*.h")
file(GLOB_RECURSE ALL_UI "src/*.ui")

include_directories(
    ${FFmpeng_INC}
)

add_executable(${PROJECT_NAME}
    # WIN32 # If you need a terminal for debug, please comment this statement 
    main.cpp
    Microscope.cpp
    Microscope.h
    Microscope.ui
    ${ALL_SOURCE}
    ${ALL_HEAD}
    ${ALL_UI}
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets
    ${OpenCV_LIBS}
    ${FFmpeng_FILE}
) # Qt5 Shared Library