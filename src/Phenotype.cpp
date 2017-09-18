#include "Phenotype.h"

Phenotype::Phenotype(SDL_Surface const* reference)
{
    target = reference;
    data = surfaceWithEndian(target->w, target->h);
    numCircles = 15;
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
    fresh.numCircles = other.numCircles;
    if (SDL_BlitSurface(other.data, NULL, fresh.data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
    return fresh;
}

Phenotype::Phenotype(Phenotype const& other)
{
    target = other.target;
    genotype = other.genotype;
    numCircles = other.numCircles;
    data = surfaceWithEndian(target->w, target->h);
    if (SDL_BlitSurface(other.data, NULL, data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
}

void Phenotype::randomInit()
{
    for (int i = 0; i < numCircles; i++)
    {
        // TODO: normalize chromosomes to be in range [0,255]?
        int w = target->w;
        int h = target->h;

        int posX = -w / 2 + rand() % (2 * w);
        int posY = -h / 2 + rand() % (2 * h);
        int radius = rand() % (std::min(w, h) / 2);
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
    Uint32 r = (Uint32)*genes++;
    Uint32 g = (Uint32)*genes++;
    Uint32 b = (Uint32)*genes++;
    uint8_t a = (uint8_t)*genes++;

    for (int h = 0; h < R * 2; h++)
    {
        for (int w = 0; w < R * 2; w++)
        {
            int dx = R - w; // horizontal offset
            int dy = R - h; // vertical offset
            if ((dx*dx + dy*dy) <= (R * R))
            {
                int xpos = x + dx;
                int ypos = y + dy;
                if (xpos >= 0 && xpos < data->w && ypos >= 0 && ypos < data->h)
                {
                    Uint8 RR, GG, BB, AA;
                    Uint32 *row = (Uint32 *)data->pixels + ypos * data->pitch / 4 + xpos;
                    SDL_GetRGBA(*row, data->format, &RR, &GG, &BB, &AA);           
                    
                    // Custom alpha blending
                    Uint32 R = RR * (255 - a) + r * a;
                    Uint32 G = GG * (255 - a) + g * a;
                    Uint32 B = BB * (255 - a) + b * a;
                    Uint32 A = 255;

                    *row = SDL_MapRGBA(data->format, R / 255, G / 255, B / 255, A);
                }
            }
        }
    }
}

void Phenotype::draw()
{
    SDL_FillRect(data, NULL, SDL_MapRGBA(data->format, 0, 0, 0, 255)); // clear existing data
    int *ptr = genotype.data();
    for (int c = 0; c < numCircles; c++)
        drawCircle(ptr + c * 7);
}

// Negative sum of squared distances
float Phenotype::fitness()
{
    float sum = 0.0f;
    for (int h = 0; h < data->h; h++)
    {
        for (int w = 0; w < data->w; w++)
        {
            Uint8 srcR, srcG, srcB, srcA, dstR, dstG, dstB, dstA;
            Uint32 *pixelSrc = (Uint32*)data->pixels + h * data->w + w;
            Uint32 *pixelDst = (Uint32*)target->pixels + h * target->w + w;
            SDL_GetRGBA(*pixelSrc, data->format, &srcR, &srcG, &srcB, &srcA);
            SDL_GetRGBA(*pixelDst, target->format, &dstR, &dstG, &dstB, &dstA);

            int dr = (int)srcR - (int)dstR;
            int dg = (int)srcG - (int)dstG;
            int db = (int)srcB - (int)dstB;
            int da = (int)srcA - (int)dstA;

            sum += (dr*dr + dg*dg + db*db);
        }
    }

    return -1.0f * sum;
}

void Phenotype::mutate()
{
    // TEST!
    genotype.clear();
    randomInit();
}