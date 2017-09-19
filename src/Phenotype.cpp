#include "Phenotype.h"

Phenotype::Phenotype(SDL_Surface const* reference)
{
    target = reference;
    data = surfaceWithEndian(target->w, target->h);
    numCircles = 10;
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

inline int Phenotype::geneToX(Gene g)
{
    int w = data->w;
    int x = -w / 2 + g * (2 * w) / 255;
    return x;
}
inline int Phenotype::geneToY(Gene g)
{
    int h = data->h;
    int y = -h / 2 + g * (2 * h) / 255;
    return y;
}
inline int Phenotype::geneToRadius(Gene g)
{
    int R = g * std::min(data->w, data->h) / (4 * 255);
    return R;
}

// Chromosomes normalized into range [0,255], corresponding physical quantities scaled
void Phenotype::randomInit()
{
    genotype.resize(numCircles * genesPerCircle);
    std::for_each(genotype.begin(), genotype.end(), [&](Gene &g) { g = (Gene)(rand() % 256); });
}

void Phenotype::drawCircle(Gene *genes)
{
    int x = geneToX(*genes++);
    int y = geneToY(*genes++);
    int R = geneToRadius(*genes++);
    Uint32 r = (Uint32)*genes++;
    Uint32 g = (Uint32)*genes++;
    Uint32 b = (Uint32)*genes++;
    Uint8 a = (Uint8)*genes++;

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
                    Uint32 *row = (Uint32 *)data->pixels + ypos * data->w + xpos;
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
    Gene *ptr = genotype.data();
    for (int c = 0; c < numCircles; c++)
        drawCircle(ptr + c * genesPerCircle);
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
    randomInit();
}