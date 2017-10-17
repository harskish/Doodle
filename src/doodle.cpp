#include <iostream>
#include <ctime>
#include "SDL.h"
#include "GeneticOptimizer.h"
#include "AnnealingOptimizer.h"

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
    //SDL_GL_SetSwapInterval(0); // disable double buffered vsync

    SDL_Surface* tmp = SDL_LoadBMP("bull.bmp"); // CPU memory
    if (tmp == NULL)
    {
        std::cout << "Unable to load image\n";
        return -1;
    }

    // Force familiar format
    SDL_Surface* image = surfaceWithEndian(tmp->w, tmp->h);
    if (SDL_BlitSurface(tmp, NULL, image, NULL))
        throw std::runtime_error("Faied to update current best solution");

    SDL_FreeSurface(tmp);

    int scale = 2;
    int width = image->w * scale;
    int height = image->h * scale;

    SDL_Window* window = SDL_CreateWindow("Doodle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * 2, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image); // GPU memory
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_Rect dstrect = { 0, 0, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    // Initialize optimizer
    GeneticOptimizer opt(image);
    //AnnealingOptimizer opt(image);
    srand((unsigned int)time(NULL));

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
            SDL_Rect rect = { width, 0, width, height }; // x, y, w, h
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, progressTexture, NULL, &rect); // current
            SDL_RenderCopy(renderer, texture, NULL, &dstrect); // reference
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(progressTexture);
        }

        // Poll events
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_F5:
						SDL_SaveBMP(opt.getCurrentBest(), "out.bmp");
						break;
				}
				break;
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

