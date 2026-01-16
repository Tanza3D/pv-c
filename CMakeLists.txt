cmake_minimum_required(VERSION 3.12)
project(pv-c)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Desktop build flag
if(DEFINED DESKTOP)
    find_package(SDL3 REQUIRED)
    add_compile_definitions(DESKTOP=${DESKTOP})
endif()

# Common source files
set(SOURCE_FILES
        src/main.cpp
        src/gradient.cpp
        src/Eye.cpp
        src/Eyes.cpp
        src/Face.cpp
        src/Faces.cpp
        src/Texture.cpp
        src/stb_image.h
)

# Add platform-specific sources
if(DEFINED DESKTOP)
    list(APPEND SOURCE_FILES
            src/matrix_emu/led-matrix.h
            src/matrix_emu/led-matrix.cpp
    )
    include_directories(src/matrix_emu)
else()
    # Headers don't need to be in SOURCE_FILES, just include the directory
    include_directories(src/matrix)
    include_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/include)
    link_directories(${CMAKE_SOURCE_DIR}/../rpi-rgb-led-matrix/lib)
endif()

# Create executable
add_executable(pv-c ${SOURCE_FILES})

# Link libraries
target_link_libraries(pv-c PRIVATE pthread)
if(NOT DEFINED DESKTOP)
    target_link_libraries(pv-c PRIVATE rgbmatrix)
else()
    target_link_libraries(pv-c PRIVATE SDL3::SDL3)
endif()