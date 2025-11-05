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
#include <iomanip>
#include "base64.hpp"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

void printBitmapData(const std::string& bitmap_data, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            // Print '1' as a filled square and '0' as an empty square
            if (bitmap_data[index] == '1') {
                std::cout << "\u2588\u2588"; // Full block
            } else {
                std::cout << "  "; // Empty space
            }
        }
        std::cout << std::endl;
    }
}



using namespace rgb_matrix;
using namespace std;

#define BIT_WIDTH 64
#define BIT_HEIGHT 32
uint8_t bit_buffer[BIT_WIDTH * BIT_HEIGHT / 8] = {0}; 


void setBit(int i, bool value) {
  if (value)
    bit_buffer[i / 8] |= (1 << (i % 8));
  else
    bit_buffer[i / 8] &= ~(1 << (i % 8));
}

bool getBit(int i) {
  return (bit_buffer[i / 8] >> (i % 8)) & 1;
}


void setPixel(FrameCanvas *canvas, int x, int y, float alpha, const vector<vector<TZColor>> &gradient, int mirror = 1, bool eyes = false) {
    if (!(alpha > 0)) return;
    int orig_y = y;
    y = 32 - y;
    TZColor col = gradient[y][x];

    if (eyes) {
        col.r = 100;
        col.g = 30;
        col.b = 255;
    }

    if (mirror < 2) canvas->SetPixel(x, y, col.r * alpha, col.g * alpha, col.b * alpha);
    if (mirror > 0) canvas->SetPixel(128 - x, y, col.r * alpha, col.g * alpha, col.b * alpha);

    int bit_index = orig_y * BIT_WIDTH + x;

    // calculate brightness (simple luminance formula)
    float brightness = 0.299f * col.r + 0.587f * col.g + 0.114f * col.b;
    bool bit_value = (alpha > 0.5f) && (brightness > 10); // threshold brightness (10 is example)

    if (mirror > 0) {
        setBit(bit_index, bit_value);
    }
}

std::string getBitmapData() {
 std::string result;
  result.reserve(BIT_WIDTH * BIT_HEIGHT);
  for (int i = 0; i < BIT_WIDTH * BIT_HEIGHT; i++) {
    result += getBit(i) ? '1' : '0';
  }
  return result;
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


std::mutex send_mutex;

void send_in_chunks(int serial_fd, const std::string &message, size_t chunk_size = 200) {
    std::thread([serial_fd, message, chunk_size]() {
        std::lock_guard<std::mutex> lock(send_mutex);

        size_t len = message.size();
        size_t pos = 0;
        while (pos < len) {
            size_t send_len = std::min(chunk_size, len - pos);
            size_t sent = 0;
            while (sent < send_len) {
                ssize_t ret = write(serial_fd, message.c_str() + pos + sent, send_len - sent);
                if (ret <= 0) {
                    // handle error or break
                    break;
                }
                sent += ret;
            }
            pos += send_len;
            usleep(25000); // 20ms
        }
    }).detach();
}


std::string packBitmap(const std::string& bits) {
    std::string packed;
    for (size_t i = 0; i < bits.size(); i += 8) {
        std::string byte_str = bits.substr(i, 8);
        while (byte_str.size() < 8) byte_str += '0'; // pad if needed
        unsigned char byte = std::stoi(byte_str, nullptr, 2);
        packed += byte;
    }
    return packed;
}

std::string base64_encode(const std::string &in) {
    static const std::string b64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

int main() {
    RGBMatrix::Options defaults;
    defaults.rows = 32;
    defaults.cols = 64;
    defaults.chain_length = 2;
    defaults.hardware_mapping = "adafruit-hat";

    RuntimeOptions runtime;
    runtime.gpio_slowdown = 2;
    defaults.brightness = 100;
    defaults.limit_refresh_rate_hz = 90;

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(defaults, runtime);
    if (!matrix) {
        std::cerr << "Could not create LED matrix" << std::endl;
        return 1;
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

    float serialtime = 0;

    
    // Serial port initialization
    const char* serialPort = "/dev/ttyACM0";
    int serial_fd = open(serialPort, O_RDWR | O_NOCTTY);  // Open the serial port
    if (serial_fd == -1) {
        std::cerr << "Error: Could not open serial port: " << strerror(errno) << std::endl;
    }

    std::string last_message = "";

    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();
    double fps = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        memset(bit_buffer, 0, sizeof(bit_buffer));
        drawScreen(canvas, eyes.GetTexture().textureMap, eyes.GetTexture().textureMap_Open,
                   face.GetTexture().textureMap, preprocessedGradient, time);

        canvas = matrix->SwapOnVSync(canvas);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        time += (duration.count() / 50);
        serialtime += (duration.count() / 50);

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

        if (serialtime > 5) {
            serialtime = 0;
        
            std::string face_part = face.texture;
        
            int fps_int = static_cast<int>(fps);
            std::string fps_str = std::to_string(fps_int);
        
            std::string eyes_part = eyes.texture;
        
            std::string bitmap_data = getBitmapData();

            std::string message = "\n" + eyes_part + ";" + face_part + ";" + fps_str + ";" + base64_encode(packBitmap(bitmap_data)) + "\n";

            send_in_chunks(serial_fd, message);
        }

        
    }

    // Clean up
    close(serial_fd);
    delete matrix;
    return 0;
}
