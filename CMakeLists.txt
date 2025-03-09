cmake_minimum_required(VERSION 3.12)
project(pv-c)

set(CMAKE_CXX_STANDARD 14)

# Add the matrix directory as an include directory
include_directories(matrix)

# Define the source files
set(SOURCE_FILES
        main.cpp
        gradient.cpp
        Eyes.cpp
        Face.cpp
        TextureSource.cpp
        Texture.cpp
        stb_image.h
        matrix/canvas.h
        matrix/content-streamer.h
        matrix/graphics.h
        matrix/led-matrix-c.h
        matrix/led-matrix.h
        matrix/pixel-mapper.h
        matrix/thread.h
        matrix/threaded-canvas-manipulator.h
)

# Add the rpi-rgb-led-matrix include directory
include_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/include)

# Link against the shared library
link_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/lib)

# Add executable target
add_executable(pv-c ${SOURCE_FILES})

# Now link the library after defining the executable
target_link_libraries(pv-c PRIVATE rgbmatrix pthread)
