#include "led-matrix.h"

#include <iostream>

using namespace rgb_matrix;


#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


RGBMatrix::Options::Options() {
    hardware_mapping = nullptr;
    rows = 32;
    cols = 64;
    chain_length = 1;
    parallel = 1;
    pwm_bits = 11;
    pwm_lsb_nanoseconds = 130;
    pwm_dither_bits = 0;
    brightness = 100;
    scan_mode = 0;
    row_address_type = 0;
    multiplexing = 0;
    disable_hardware_pulsing = false;
    show_refresh_rate = false;
    inverse_colors = false;
    led_rgb_sequence = nullptr;
    pixel_mapper_config = nullptr;
    panel_type = nullptr;
    limit_refresh_rate_hz = 0;
    disable_busy_waiting = false;
}

bool RGBMatrix::Options::Validate(std::string *err) const {
    return true; // surely
}

RGBMatrix::RuntimeOptions::RuntimeOptions() {
    gpio_slowdown = 1;
    daemon = 0;
    drop_privileges = 1;
    do_gpio_init = true;
    drop_priv_user = nullptr;
    drop_priv_group = nullptr;
}

RGBMatrix* RGBMatrix::CreateFromOptions(const Options &options, const RuntimeOptions &runtime) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    RGBMatrix* matrix = new RGBMatrix();
    matrix->options = options;
    matrix->runtime = runtime;

    int width = options.cols * options.chain_length;
    int height = options.rows;
    matrix->window_ = SDL_CreateWindow("the tnaz",
                                        width * 8, height * 8,
                                        0);

    if (!matrix->window_) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return nullptr;
    }

    matrix->renderer_ = SDL_CreateRenderer(matrix->window_, NULL);

    return matrix;
}

FrameCanvas* RGBMatrix::CreateFrameCanvas() {
    int width = options.cols * options.chain_length;
    int height = options.rows;
    return new FrameCanvas(width, height);
}

void RGBMatrix::SetBrightness(int brightness) {
    return;
}

FrameCanvas* RGBMatrix::SwapOnVSync(FrameCanvas *canvas) {
    // THIS IS WHERE WE RENDER THE FRAME. THEN WE RETURN NEW TO CLEAR
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            SDL_DestroyRenderer(renderer_);
            SDL_DestroyWindow(window_);
            SDL_Quit();
            exit(0);
        }
    }

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    const uint8_t* pixels = canvas->GetPixels();
    int scale = 8;
    for (int y = 0; y < canvas->height(); y++) {
        for (int x = 0; x < canvas->width(); x++) {
            int index = ((canvas->height() - (y+1)) * canvas->width() + x) * 3;
            SDL_SetRenderDrawColor(renderer_, pixels[index], pixels[index+1], pixels[index+2], 255);
            SDL_FRect rect = {(float)(x * scale), (float)(y * scale), (float)scale, (float)scale};
            SDL_RenderFillRect(renderer_, &rect);
        }
    }

    SDL_RenderPresent(renderer_);
    return canvas;


    return new FrameCanvas();
}

void FrameCanvas::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    int index = (y * width_ + x) * 3;
    pixels_[index] = red;
    pixels_[index + 1] = green;
    pixels_[index + 2] = blue;
}

void FrameCanvas::Fill(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < pixels_.size(); i += 3) {
        pixels_[i] = red;
        pixels_[i + 1] = green;
        pixels_[i + 2] = blue;
    }
}