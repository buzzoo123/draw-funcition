#include "SDL2/SDL.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!window || !renderer) {
        std::cerr << "Window/Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set the initial white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    bool quit = false;
    bool drawing = false;
    SDL_Event event;
    int prevX, prevY;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                // Clear the screen again if resized
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    drawing = true;
                    SDL_GetMouseState(&prevX, &prevY);
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    drawing = false;
                }
            } else if (event.type == SDL_MOUSEMOTION && drawing) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Draw the line directly on the renderer
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for line
                SDL_RenderDrawLine(renderer, prevX, prevY, x, y);

                // Update the screen
                SDL_RenderPresent(renderer);

                // Update previous position
                prevX = x;
                prevY = y;
            }
        }
    }

    // Capture renderer output for printing
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, windowWidth, windowHeight, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

    // Print matrix with 1 for black and 0 for white
    vector<vector<int>> windowPixelColors(windowHeight, vector<int>(windowWidth, 0));;
    uint32_t* pixels = (uint32_t*)surface->pixels;
    for (int y = 0; y < windowHeight; ++y) {
        for (int x = 0; x < windowWidth; ++x) {
            uint32_t pixel = pixels[(y * windowWidth) + x];
            uint8_t r = (pixel & 0xFF000000) >> 24;
            uint8_t g = (pixel & 0x00FF0000) >> 16;
            uint8_t b = (pixel & 0x0000FF00) >> 8;
            if (r == 255 && g == 255 && b == 255) {
                windowPixelColors[y][x] = 0;
            } else {
                windowPixelColors[y][x] = 1;
            }
        }
    }

    for (int y = 0; y < windowHeight; ++y) {
        for (int x = 0; x < windowWidth; ++x) {
            cout << windowPixelColors[y][x];
        }
        cout << "\n";
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
