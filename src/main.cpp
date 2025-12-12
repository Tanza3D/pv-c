#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "gradient.h"
#include "matrix/led-matrix.h"
#include "matrix/graphics.h"
#include "Texture.h"
#include "Eye.h"
#include "Eyes.h"
#include "Face.h"
#include "Faces.h"
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
#include <termios.h>
#include <poll.h>
#include <atomic>
#include <sstream>

// Simple JSON value extraction (avoids external dependency)
std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\":\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";
    pos += searchKey.length();
    size_t end = json.find("\"", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

int extractJsonInt(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\":";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return -1;
    pos += searchKey.length();
    // Skip whitespace
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    size_t end = pos;
    while (end < json.length() && (isdigit(json[end]) || json[end] == '-')) end++;
    if (end == pos) return -1;
    return std::stoi(json.substr(pos, end - pos));
}

void printBitmapData(const std::string &bitmap_data, int width, int height) {
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


void setPixel(FrameCanvas *canvas, int x, int y, float alpha, const vector<vector<TZColor>> &gradient, int mirror = 1) {
    if (!(alpha > 0)) return;
    int orig_y = y;
    y = 32 - y;
    TZColor col = gradient[y][x];

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


void drawImage(FrameCanvas *canvas, const Texture &image, const vector<vector<TZColor>> &gradient) {
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 64; x++) {
            setPixel(canvas, x, y, image.textureMap[y][x], gradient);
        }
    }
}

void drawPupil(FrameCanvas *canvas, const vector<vector<TZColor>> &gradient, const Texture &area,
               int time, float eyeX, float eyeY, bool mirror = false) {

    int eyeMinX = std::numeric_limits<int>::max();
    int eyeMaxX = std::numeric_limits<int>::min();
    int eyeMinY = std::numeric_limits<int>::max();
    int eyeMaxY = std::numeric_limits<int>::min();

    for (int y = 0; y < area.textureMap.size(); ++y) {
        for (int x = 0; x < area.textureMap[y].size(); ++x) {
            if (area.textureMap[y][x] == 1.0) {
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

            float alpha = area.textureMap[py][px];

            int m = 0;
            if (mirror) {
                m = 2;
            }
            setPixel(canvas, px, py, alpha, gradient, m);
        }
    }
}

#include <cmath>
#include <cstdlib>

double smoothOscillation(double time, double frequency, double amplitude) {
    return amplitude * sin(frequency * time);
}

void pupilHelper(FrameCanvas *canvas, const vector<vector<TZColor>> &gradient, const Texture &area,
                 int time) {

    // oscillation from smoothOscillation: -0.5 .. 0.5
    double oscillationX = smoothOscillation(time * 0.1, 0.5, 0.5);
    double oscBase = oscillationX * 1.1; // max oscillation scale

    // left pupil: -0.5 -> 0, 0 -> 1, 0.5 -> 1
    double pupilOffsetX_left = std::clamp(oscBase + 1.0, 0.0, 1.0) - 0.3;
    // right pupil: -0.5 -> 1, 0 -> 1, 0.5 -> 0
    double pupilOffsetX_right = std::clamp(1.0 - oscBase, 0.0, 1.0) - 0.3;

    drawPupil(canvas, gradient, area, time, pupilOffsetX_left, 0.3);
    drawPupil(canvas, gradient, area, time, pupilOffsetX_right, 0.3, true);
}


void drawScreen(FrameCanvas *canvas, const Eye eye,
                const Face face, const vector<vector<TZColor>> &gradient, int time) {
    canvas->Fill(0, 0, 0);


    vector<vector<TZColor>> pupilGradient = gradient;
    vector<vector<TZColor>> eyeGradient = gradient;
    vector<vector<TZColor>> faceGradient = gradient;

    if (eye.hasOverride) eyeGradient = eye.override;
    if (eye.hasPupilOverride) pupilGradient = eye.pupilOverride;
    if (face.hasOverride) faceGradient = face.override;

    pupilHelper(canvas, pupilGradient, eye.GetPupilAreaTexture(), time);
    drawImage(canvas, eye.GetTexture(), eyeGradient);
    drawImage(canvas, face.GetTexture(), faceGradient);
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


std::string packBitmap(const std::string &bits) {
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
    for (unsigned char c: in) {
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
    runtime.gpio_slowdown = 5;
    defaults.brightness = 100;
    defaults.limit_refresh_rate_hz = 90;

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(defaults, runtime);
    if (!matrix) {
        std::cerr << "Could not create LED matrix" << std::endl;
        return 1;
    }

    vector<vector<TZColor>> gradientMap = {
            {{255, 100, 190}, {200, 20,  141}},
            {{51,  20,  190}, {255, 105, 200}},
    };
    vector<vector<TZColor>> preprocessedGradient = Gradient::preprocessGradient(gradientMap);

    Eyes eyes = Eyes();
    Faces faces = Faces();
    eyes.Init();
    faces.Init();


    FrameCanvas *canvas = matrix->CreateFrameCanvas();

    float time = 0;

    float serialtime = 0;


    // Serial port initialization
    const char *serialPort = "/dev/ttyACM0";
    int serial_fd = open(serialPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_fd == -1) {
        std::cerr << "Error: Could not open serial port: " << strerror(errno) << std::endl;
    } else {
        // Configure serial port
        struct termios tty;
        if (tcgetattr(serial_fd, &tty) == 0) {
            cfsetospeed(&tty, B115200);
            cfsetispeed(&tty, B115200);
            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
            tty.c_cflag &= ~PARENB;
            tty.c_cflag &= ~CSTOPB;
            tty.c_cflag |= CREAD | CLOCAL;
            tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);
            tty.c_oflag &= ~OPOST;
            tcsetattr(serial_fd, TCSANOW, &tty);
        }
        std::cout << "Serial port opened successfully" << std::endl;
    }

    std::string serial_buffer = "";
    std::string last_message = "";
    int currentBrightness = 100;

    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();
    double fps = 0;
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        // Read from serial (non-blocking)
        if (serial_fd != -1) {
            char buf[256];
            ssize_t n = read(serial_fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                serial_buffer += buf;
                
                // Process complete lines
                size_t newline_pos;
                while ((newline_pos = serial_buffer.find('\n')) != std::string::npos) {
                    std::string line = serial_buffer.substr(0, newline_pos);
                    serial_buffer = serial_buffer.substr(newline_pos + 1);
                    
                    // Remove carriage return if present
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    
                    // Process JSON
                    if (!line.empty() && line[0] == '{') {
                        std::string event = extractJsonString(line, "event");
                        
                        if (event == "options") {
                            std::string face = extractJsonString(line, "face");
                            std::string eyesName = extractJsonString(line, "eyes");
                            int brightness = extractJsonInt(line, "brightness");
                            
                            if (!face.empty()) {
                                if (faces.SetCurrent(face)) {
                                    std::cout << "Face set to: " << face << std::endl;
                                } else {
                                    std::cout << "Unknown face: " << face << std::endl;
                                }
                            }
                            
                            if (!eyesName.empty()) {
                                if (eyes.SetCurrent(eyesName)) {
                                    std::cout << "Eyes set to: " << eyesName << std::endl;
                                } else {
                                    std::cout << "Unknown eyes: " << eyesName << std::endl;
                                }
                            }
                            
                            if (brightness >= 0 && brightness <= 100) {
                                currentBrightness = brightness;
                                matrix->SetBrightness(brightness);
                                std::cout << "Brightness set to: " << brightness << std::endl;
                            }
                        } else if (event == "restart") {
                            std::cout << "Restart event received" << std::endl;
                            // Could add restart logic here if needed
                        }
                    }
                }
            }
        }

        memset(bit_buffer, 0, sizeof(bit_buffer));
        drawScreen(canvas,
                   eyes.GetCurrent(),
                   faces.GetCurrent(), preprocessedGradient, time);

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

            std::string bitmap_data = getBitmapData();

            // Create JSON message with only the image data
            std::string message = "{\"image\":\"" + base64_encode(packBitmap(bitmap_data)) + "\"}\n";

            send_in_chunks(serial_fd, message);
        }


    }

    // Clean up
    close(serial_fd);
    delete matrix;
    return 0;
}
