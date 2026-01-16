#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace rgb_matrix {
    class FrameCanvas;

    class RGBMatrix {
    public:
        struct Options {
            Options();

            bool Validate(std::string *err) const;

            const char *hardware_mapping;
            int rows;
            int cols;
            int chain_length;
            int parallel;
            int pwm_bits;
            int pwm_lsb_nanoseconds;
            int pwm_dither_bits;
            int brightness;
            int scan_mode;
            int row_address_type;
            int multiplexing;
            bool disable_hardware_pulsing;
            bool show_refresh_rate;
            bool inverse_colors;
            const char *led_rgb_sequence;
            const char *pixel_mapper_config;
            const char *panel_type;
            int limit_refresh_rate_hz;
            bool disable_busy_waiting;
        };

        struct RuntimeOptions {
            RuntimeOptions();

            int gpio_slowdown;
            int daemon;
            int drop_privileges;
            bool do_gpio_init;
            const char *drop_priv_user;
            const char *drop_priv_group;
        };


        static RGBMatrix *CreateFromOptions(const Options &options, const RuntimeOptions &runtime);

        FrameCanvas *CreateFrameCanvas();

        void SetBrightness(int brightness);

        FrameCanvas *SwapOnVSync(FrameCanvas *canvas);

    private:
        SDL_Window* window_;
        SDL_Renderer* renderer_;
        Options options;
        RuntimeOptions runtime;
    };

    class FrameCanvas {
    private:
        int width_, height_;
        std::vector<uint8_t> pixels_;
    public:
        void SetPixel(int x, int y,
                      uint8_t red, uint8_t green, uint8_t blue);

        void Fill(uint8_t red, uint8_t green, uint8_t blue);

        FrameCanvas(int w = 128, int h = 32) : width_(w), height_(h) {
            pixels_.resize(w * h * 3, 0);
        }


        int width() const { return width_; }
        int height() const { return height_; }
        const uint8_t* GetPixels() const { return pixels_.data(); }
    };
};
