#include "Phenotype.h"

Phenotype::Phenotype(SDL_Surface const* reference)
{
    target = reference;
    data = surfaceWithEndian(target->w, target->h);
    numCircles = 5;
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

        int posX = -w/2 + rand() % (2*w);
        int posY = -h/2 + rand() % (2*h);
        int radius = rand() % (std::min(w, h) / 2);
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        int a = 255; // rand() % 256;

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
                if (xpos > 0 && xpos < data->w && ypos > 0 && ypos < data->h)
                {
                    unsigned char* pixels = (unsigned char*)data->pixels;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    pixels[4 * (ypos * data->w + xpos) + 0] = (unsigned char)r;
                    pixels[4 * (ypos * data->w + xpos) + 1] = (unsigned char)g;
                    pixels[4 * (ypos * data->w + xpos) + 2] = (unsigned char)b;
                    pixels[4 * (ypos * data->w + xpos) + 3] = (unsigned char)a;
#else
                    pixels[4 * (ypos * data->w + xpos) + 3] = (unsigned char)r;
                    pixels[4 * (ypos * data->w + xpos) + 2] = (unsigned char)g;
                    pixels[4 * (ypos * data->w + xpos) + 1] = (unsigned char)b;
                    pixels[4 * (ypos * data->w + xpos) + 0] = (unsigned char)a;
#endif
                }
            }
        }
    }
}

void Phenotype::draw()
{
    std::fill((Uint32*)data->pixels, (Uint32*)data->pixels + data->w * data->h, 0); // clear existing data
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
            const unsigned char* src = (const unsigned char*)data->pixels;
            const unsigned char* dst = (const unsigned char*)target->pixels;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            int dr = src[4 * (h * data->w + w) + 0] - dst[4 * (h * data->w + w) + 0];
            int dg = src[4 * (h * data->w + w) + 1] - dst[4 * (h * data->w + w) + 1];
            int db = src[4 * (h * data->w + w) + 2] - dst[4 * (h * data->w + w) + 2];
#else
            int dr = src[4 * (h * data->w + w) + 3] - dst[4 * (h * data->w + w) + 3];
            int dg = src[4 * (h * data->w + w) + 2] - dst[4 * (h * data->w + w) + 2];
            int db = src[4 * (h * data->w + w) + 1] - dst[4 * (h * data->w + w) + 1];
#endif
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