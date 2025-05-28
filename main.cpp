#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "gradient.h"
#include "matrix/led-matrix.h"
#include "matrix/graphics.h"
#include "Texture.h"
#include "Eyes.h"
#include "Face.h"
#include <fstream>
#include <fcntl.h>   
#include <unistd.h> 
#include <cstring> 
#include <errno.h>  



using namespace rgb_matrix;
using namespace std;

void setPixel(FrameCanvas *canvas, int x, int y, float alpha, const vector <vector<TZColor>> &gradient, int mirror = 1, bool eyes = false) {
    if (!(alpha > 0)) return;
    y = 32 - y;
    TZColor col = gradient[y][x];

    if(eyes) {
        col.r = 100;
        col.g = 30;
        col.b = 255;

    }

    if(mirror < 2) canvas->SetPixel(x, y, col.r * alpha, col.g * alpha, col.b * alpha);
    if(mirror > 0) canvas->SetPixel(128 - x, y, col.r * alpha, col.g * alpha, col.b * alpha);
}

void drawImage(FrameCanvas *canvas, const vector <vector<double>> &image, const vector <vector<TZColor>> &gradient) {
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 64; x++) {
            setPixel(canvas, x, y, image[y][x], gradient);
        }
    }
}


void drawPupil(FrameCanvas *canvas, const vector <vector<TZColor>> &gradient, const vector <vector<double>> &area,
               int time, float eyeX, float eyeY, bool mirror = false) {

    int eyeMinX = std::numeric_limits<int>::max();
    int eyeMaxX = std::numeric_limits<int>::min();
    int eyeMinY = std::numeric_limits<int>::max();
    int eyeMaxY = std::numeric_limits<int>::min();

    for (int y = 0; y < area.size(); ++y) {
        for (int x = 0; x < area[y].size(); ++x) {
            if (area[y][x] == 1.0) {
                eyeMinX = std::min(eyeMinX, x);
                eyeMaxX = std::max(eyeMaxX, x);
                eyeMinY = std::min(eyeMinY, y);
                eyeMaxY = std::max(eyeMaxY, y);
            }
        }
    }

    if (eyeMinX == std::numeric_limits<int>::max() || eyeMaxX == std::numeric_limits<int>::min() ||
        eyeMinY == std::numeric_limits<int>::max() || eyeMaxY == std::numeric_limits<int>::min()) {
        return;
    }

    int pupilX = eyeMinX + static_cast<int>((eyeMaxX - eyeMinX) * eyeX);
    int pupilY = eyeMinY + static_cast<int>((eyeMaxY - eyeMinY) * eyeY);

    pupilX = std::min(std::max(pupilX, eyeMinX + 1), eyeMaxX - 1);
    pupilY = std::min(std::max(pupilY, eyeMinY + 1), eyeMaxY - 1);

    int width = 3;
    int height = 7;

    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            if ((dy == 0 && dx == 0) ||
                (dy == 0 && dx == width - 1) ||
                (dy == height - 1 && dx == 0) ||
                (dy == height - 1 && dx == width - 1))
                continue;

            int px = pupilX + dx;
            int py = pupilY + dy;

            float alpha = area[py][px];

            int m = 0;
            if(mirror) {
                m = 2;
            }
            setPixel(canvas, px, py, alpha, gradient, m, true);
        }
    }
}
#include <cmath>
#include <cstdlib>

double smoothOscillation(double time, double frequency, double amplitude) {
    return amplitude * sin(frequency * time);
}

void pupilHelper(FrameCanvas *canvas, const vector <vector<TZColor>> &gradient, const vector <vector<double>> &area,
                 int time) {

    double oscillationX = smoothOscillation(time * 0.1, 0.5, 0.5);

    double pupilOffsetX_left = 0.5 - (oscillationX * 0.7);
    double pupilOffsetX_right = 0.5 + (oscillationX * 0.7);

    drawPupil(canvas, gradient, area, time, pupilOffsetX_left, 0.3);
    drawPupil(canvas, gradient, area, time, pupilOffsetX_right, 0.3, true);
}


void drawScreen(FrameCanvas *canvas, const vector <vector<double>> eyes, const vector <vector<double>> pupilArea,
                const vector <vector<double>> face, const vector <vector<TZColor>> &gradient, int time) {
    canvas->Fill(0, 0, 0);

    pupilHelper(canvas, gradient, pupilArea, time);


    drawImage(canvas, eyes, gradient);
    drawImage(canvas, face, gradient);
}

int main() {
    RGBMatrix::Options defaults;
    defaults.rows = 32;
    defaults.cols = 64;
    defaults.chain_length = 2;
    defaults.hardware_mapping = "adafruit-hat";
    defaults.brightness = 100;

    RuntimeOptions runtime;
    runtime.drop_privileges = 0;
    runtime.gpio_slowdown = 4;

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(defaults, runtime);
    if (!matrix) {
        std::cerr << "Could not create LED matrix" << std::endl;
        return 1;
    }

    // Fill the screen with red
    for (size_t y = 0; y < 32; y++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        for (size_t x = 0; x < 128; x++) {
            matrix->SetPixel(x, y, y*6, 0, x*3);
        }
    }

    vector <vector<TZColor>> gradientMap = {
            {{255, 100, 190}, {200, 20, 141}},
            {{51, 20,  190}, {255, 105, 200}},
    };
    vector <vector<TZColor>> preprocessedGradient = Gradient::preprocessGradient(gradientMap, 65, 33);

    Eyes eyes = Eyes();
    Face face = Face();
    eyes.Init();
    face.Init();
    std::cout << "Eyes texture size: " << eyes.GetTexture().textureMap.size() << "x"
         << eyes.GetTexture().textureMap[0].size() << std::endl;

    FrameCanvas *canvas = matrix->CreateFrameCanvas();

    float time = 0;

    // Serial port initialization
    const char* serialPort = "/dev/ttyACM0";
    int serial_fd = open(serialPort, O_WRONLY | O_NOCTTY);  // Open the serial port
    if (serial_fd == -1) {
        std::cerr << "Error: Could not open serial port: " << strerror(errno) << std::endl;
        //return 1;
    }

    std::string last_message = "";

    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();
    double fps = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        // Your existing code for drawing the screen, etc.
        drawScreen(canvas, eyes.GetTexture().textureMap, eyes.GetTexture().textureMap_Open,
                   face.GetTexture().textureMap, preprocessedGradient, time);

        canvas = matrix->SwapOnVSync(canvas);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        time += (duration.count() / 50);

        frameCount++;

        // Calculate FPS every second
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - lastTime;
        
        if (elapsedTime.count() >= 1.0) {  // If 1 second has passed
            fps = frameCount / elapsedTime.count();
            std::cout << "FPS: " << fps << std::endl;

            // Reset for the next second
            lastTime = currentTime;
            frameCount = 0;
        }

        if (time > 60) {
            time = 0;
            std::string face_part = "F: " + face.texture;
        
            // Convert FPS to an integer and then to a string
            int fps_int = static_cast<int>(fps);
            std::string fps_str = std::to_string(fps_int);
        
            // Calculate the number of spaces needed to align FPS to the right within the first 16 characters
            int spaces_needed = 16 - (face_part.size() + fps_str.size() + 1);  // +1 for the space before FPS
            while (spaces_needed > 0) {
                face_part += " ";  // Add spaces to the end of face_part
                spaces_needed--;
            }
        
            // Append FPS value
            face_part += " " + fps_str;
        
            std::string eyes_part = "E: " + eyes.texture;
            
            // Check if there are any remaining characters for the second row
            //std::string message = face_part + eyes_part;
            //if (last_message != message) {
            //    last_message = message;
            //    ssize_t bytes_written = write(serial_fd, message.c_str(), message.size());
            //}
        }
        
    }

    // Clean up
    close(serial_fd);
    delete matrix;
    return 0;
}
