#include <iostream>
#include "SDL.h"
#include "GeneticOptimizer.h"

namespace Settings
{
    int width;
    int height;
    int populationSize = 8;
    int maxTris = 1024;
};

// SDL defines its own main
#undef main
int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Unable to initialize SDL\n";
        return -1;
    }

    atexit(SDL_Quit);

    SDL_Surface* image = SDL_LoadBMP("mona.bmp"); // CPU memory
    if (image == NULL)
    {
        std::cout << "Unable to load image\n";
        return -1;
    }

    int width = image->w;
    int height = image->h;

    SDL_Window* window = SDL_CreateWindow("Doodle", 0, 0, width * 2, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image); // GPU memory
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    SDL_Rect dstrect = { 0, 0, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    // Initialize optimizer
    GeneticOptimizer opt(image);

    // Main loop
    SDL_Texture *progressTexture;
    bool running = true;
    while (running)
    {
        // Advance optimization
        bool updated = opt.step();
        
        // Update preview
        if (updated)
        {
            progressTexture = SDL_CreateTextureFromSurface(renderer, opt.getCurrentBest());
            SDL_Rect dstrect = { width, 0, width * 2, height };
            SDL_RenderFillRect(renderer, &dstrect);
            SDL_RenderCopy(renderer, progressTexture, NULL, &dstrect);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(progressTexture);
        }

        // Poll events
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

