#include "Phenotype.h"

Phenotype::Phenotype(SDL_Surface const* reference)
{
    target = reference;
    data = SDL_CreateRGBSurface(SDL_SWSURFACE, target->w, target->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    randomInit();
}

Phenotype::~Phenotype()
{
    SDL_FreeSurface(data);
}

Phenotype& Phenotype::operator=(Phenotype const& other)
{
    Phenotype fresh(other.target);
    fresh.genotype = other.genotype;
    if (SDL_BlitSurface(other.data, NULL, fresh.data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
    return fresh;
}

Phenotype::Phenotype(Phenotype const& other)
{
    target = other.target;
    genotype = other.genotype;
    data = SDL_CreateRGBSurface(SDL_SWSURFACE, target->w, target->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (SDL_BlitSurface(other.data, NULL, data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
}

void Phenotype::randomInit()
{
    const int n_circles = 1;

    for (int i = 0; i < n_circles; i++)
    {
        // TODO: normalize chromosomes to be in range [0,255]?
        int w = target->w;
        int h = target->h;

        int posX = -w/2 + rand() % (2*w);
        int posY = -h/2 + rand() % (2*h);
        int radius = rand() % std::min(w, h);
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        int a = rand() % 256;

        genotype.insert(genotype.end(), { posX, posY, radius, r, g, b, a });
    }
}

void Phenotype::drawCircle(int *genes)
{
    int x = *genes++;
    int y = *genes++;
    int R = *genes++;
    int r = *genes++;
    int g = *genes++;
    int b = *genes++;
    int a = *genes++;

    for (int w = 0; w < R * 2; w++)
    {
        for (int h = 0; h < R * 2; h++)
        {
            int dx = R - w; // horizontal offset
            int dy = R - h; // vertical offset
            if ((dx*dx + dy*dy) <= (R * R))
            {
                int xpos = x + dx;
                int ypos = y + dy;
                if (xpos > 0 && xpos < data->w && ypos > 0 && ypos < data->h)
                {
                    unsigned char* pixels = (unsigned char*)data->pixels;
                    pixels[4 * (ypos * data->w + xpos) + 0] = (unsigned char)r;
                    pixels[4 * (ypos * data->w + xpos) + 1] = (unsigned char)g;
                    pixels[4 * (ypos * data->w + xpos) + 2] = (unsigned char)b;
                    pixels[4 * (ypos * data->w + xpos) + 3] = (unsigned char)a;
                }
            }
        }
    }
}

void Phenotype::draw()
{
    drawCircle(genotype.data());
}

float Phenotype::fitness()
{
    return 0.0f;
}

void Phenotype::mutate()
{
    // TEST!
    genotype.clear();
    randomInit();
}