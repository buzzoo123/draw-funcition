#include "SDL2/SDL.h"
#include "thinning.h"
#include <iostream>
#include <vector>
#include "fourier.h"
#include <cmath>

using namespace std;

void displayDominantFrequencies(const vector<vector<complex<double>>>& ifft_result, double threshold = 0.1) {
    int rows = ifft_result.size();
    int cols = ifft_result[0].size();
    int M = rows;
    int N = cols;

    cout << "Dominant frequency components:\n";

    for (int k = 0; k < rows; ++k) {
        for (int l = 0; l < cols; ++l) {
            // Extract real and imaginary parts
            double realPart = ifft_result[k][l].real();
            double imagPart = ifft_result[k][l].imag();

            // Calculate amplitude
            double amplitude = sqrt(realPart * realPart + imagPart * imagPart);

            // Check if amplitude is above the threshold
            if (amplitude > threshold) {
                // Calculate frequency in each dimension
                double frequencyX = 2.0 * M_PI * k / M;
                double frequencyY = 2.0 * M_PI * l / N;

                // Calculate phase
                double phase = atan2(imagPart, realPart);

                // Output the component as a sine and cosine term
                cout << "Amplitude: " << amplitude
                     << " | cos(" << frequencyX << " * x + " << frequencyY << " * y + " << phase << ")\n";
            }
        }
    }
}

// Function to print a matrix for visualization
void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int pixel : row) {
            cout << (pixel ? "1" : "0");
        }
        cout << "\n";
    }
    cout << "\n";
}

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

    vector<vector<int>> thinnedPixels = createSkeleton(windowPixelColors);
    vector<vector<complex<double>>> dftImage = DFT2DFFT(thinnedPixels);
    printComplexMatrix(dftImage);
    cout << endl;

    vector<vector<complex<double>>> ifft_result;
    performIFFT(dftImage, ifft_result);

    // Print IFFT result
    for (const auto& row : ifft_result) {
        for (const auto& val : row) {
            cout << "(" << val.real() << ", " << val.imag() << "i) ";
        }
        cout << endl;
    }

    cout << "\nExtracted Function Representation:\n";
    displayDominantFrequencies(ifft_result, 0.1);

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
