cmake_minimum_required(VERSION 3.12)
project(pv-c)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add the matrix directory as an include directory
include_directories(matrix)

# Define the source files
set(SOURCE_FILES
        src/main.cpp
        src/gradient.cpp
        src/Eye.cpp
        src/Eyes.cpp
        src/Face.cpp
        src/Faces.cpp
        src/Texture.cpp
        src/stb_image.h
        src/matrix/canvas.h
        src/matrix/content-streamer.h
        src/matrix/graphics.h
        src/matrix/led-matrix-c.h
        src/matrix/led-matrix.h
        src/matrix/pixel-mapper.h
        src/matrix/thread.h
        src/matrix/threaded-canvas-manipulator.h
)

# Add the rpi-rgb-led-matrix include directory
include_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/include)

# Link against the shared library
link_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/lib)

# Add executable target
add_executable(pv-c ${SOURCE_FILES})

# Now link the library after defining the executable
target_link_libraries(pv-c PRIVATE rgbmatrix pthread)
