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

using namespace rgb_matrix;
using namespace std;




void setPixel(RGBMatrix *matrix, int x, int y, float alpha, const vector<vector<TZColor>> &gradient) {
    y = 32 - y;
    TZColor col = gradient[y][x];
    matrix->SetPixel(x, y, col.r * alpha, col.g * alpha, col.b * alpha);
    matrix->SetPixel(128 - x, y, col.r * alpha, col.g * alpha, col.b * alpha);
}

void drawImage(RGBMatrix *matrix, const vector<vector<int>> &image, int offsetX, const vector<vector<TZColor>> &gradient) {
    for (size_t y = 0; y < image.size(); y++) {
        for (size_t x = 0; x < image[y].size(); x++) {
            float alpha = image[y][x] / 255.0;
            if (alpha > 0) {
                setPixel(matrix, x + offsetX, y, alpha, gradient);
            }
        }
    }
}

void drawScreen(RGBMatrix *matrix, const vector<vector<int>> &eyes, const vector<vector<int>> &face, const vector<vector<TZColor>> &gradient) {
    matrix->Clear();
    drawImage(matrix, eyes, 0, gradient);
    drawImage(matrix, face, 64, gradient);
}

int main() {
    RGBMatrix::Options defaults;
    defaults.rows = 32;
    defaults.cols = 64;
    defaults.chain_length = 2;
    defaults.hardware_mapping = "adafruit-hat";

    RuntimeOptions runtime;
    runtime.drop_privileges = 1;

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(defaults, runtime);
    if (!matrix) {
        cerr << "Could not create LED matrix" << endl;
        return 1;
    }

    vector<vector<TZColor>> gradientMap = {
            {{255, 100, 190}, {200, 100, 255}},
            {{180, 60, 190}, {255, 105, 200}},
    };
    vector<vector<TZColor>> preprocessedGradient = Gradient::preprocessGradient(gradientMap, 65, 33);

    // constructor initializes
    Eyes* eyes = new Eyes();
    Face* face = new Face();

    while (true) {
        auto start = chrono::high_resolution_clock::now();
        drawScreen(matrix, eyes->GetTexture().textureMap, face->GetTexture().textureMap, preprocessedGradient);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        cout << "drawScreen took " << duration.count() << "ms" << endl;
        this_thread::sleep_for(chrono::milliseconds(16));
    }

    delete matrix;
    return 0;
}
