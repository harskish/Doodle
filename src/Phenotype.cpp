#include "Phenotype.h"

Phenotype::Phenotype(SDL_Surface const* reference)
{
    target = reference;
    data = surfaceWithEndian(target->w, target->h);
    numCircles = 1;
    dirty = true;
    randomInit();
}

Phenotype::~Phenotype()
{
    SDL_FreeSurface(data);
}

Phenotype& Phenotype::operator=(Phenotype const& other)
{
    genotype = other.genotype;
    numCircles = other.numCircles;
    if (SDL_BlitSurface(other.data, NULL, data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
    fitnessValue = other.fitnessValue;
    dirty = other.dirty;
    return *this;
}

Phenotype::Phenotype(Phenotype const& other)
{
    target = other.target;
    genotype = other.genotype;
    numCircles = other.numCircles;
    data = surfaceWithEndian(target->w, target->h);
    if (SDL_BlitSurface(other.data, NULL, data, NULL))
        throw std::runtime_error("Failed to copy phenotype data");
    fitnessValue = other.fitnessValue;
    dirty = other.dirty;
}

// Map [0,255] to [-w/2, 3w/2]
inline int Phenotype::geneToX(Gene g)
{
    int w = data->w;
    int x = -w / 2 + g * (2 * w) / 255;
    return x;
}

// Map [0,255] to [-h/2, 3h/2]
inline int Phenotype::geneToY(Gene g)
{
    int h = data->h;
    int y = -h / 2 + g * (2 * h) / 255;
    return y;
}

// Map [0,255] to [0, min(w,h)/k]
inline int Phenotype::geneToRadius(Gene g)
{
    const int k = 1;
    int R = g * std::min(data->w, data->h) / (k * 255);
    return R;
}

// Chromosomes normalized into range [0,255], corresponding physical quantities scaled
void Phenotype::randomInit()
{
    genotype.resize(numCircles * genesPerCircle);
    std::for_each(genotype.begin(), genotype.end(), [&](Gene &g) { g = randGene(); });
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
    if (!dirty)
        return fitnessValue;
        
    // Update internal image
    draw();

    // Compute fitness, cache result
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

            sum += (dr*dr + dg*dg + db*db);
        }
    }

    // Penalize adding unnecessary circles
    sum += numCircles;

    fitnessValue = -1.0f * sum;
    dirty = false;
    return fitnessValue;
}

void Phenotype::addCircle()
{
    numCircles++;
    for(int i = 0; i < genesPerCircle; i++)
    {
        genotype.push_back(randGene());
    }
    dirty = true;
}

void Phenotype::mutate()
{
    auto randomMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        bool mutated = false;

        const int mutationRate = 5; // percent
        for (int g = 0; g < numCircles * genesPerCircle; g++)
        {
            if (rand() % 100 < mutationRate)
            {
                genotype[g] = randGene();
                mutated = true;
            }
        }

        return mutated;
    };

    auto perturbationMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        bool mutated = false;

        const int mutationRate = 10; // percent
        for (int g = 0; g < numCircles * genesPerCircle; g++)
        {
            if (rand() % 100 < mutationRate)
            {
                genotype[g] += (-10 + rand() % 21);
                mutated = true;
            }
        }

        return mutated;
    };

    auto shuffleMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        if (numCircles == 1) return false;

        int idx1, idx2;
        idx1 = rand() % numCircles;
        while((idx2 = rand() % numCircles) == idx1);

        for(int g = 0; g < genesPerCircle; g++)
        {
            std::swap(genotype[idx1 * genesPerCircle + g], genotype[idx2 * genesPerCircle + g]);
        }

        return true;
    };

    auto addCircleMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        const int mutationRate = 15; // percent
        if (rand() % 100 < mutationRate)
        {
            addCircle();
            return true;
        }

        return false;
    };

    auto removeCircleMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        if (numCircles == 1) return false;

        const int mutationRate = 15; // percent
        if (rand() % 100 < mutationRate)
        {
            auto it = genotype.begin() + (rand() % numCircles) * genesPerCircle;
            genotype.erase(it, it + genesPerCircle);
            numCircles--;
            return true;
        }

        return false;
    };

    // Perform mutations
    dirty |= randomMutation(genotype);
    dirty |= perturbationMutation(genotype);
    dirty |= shuffleMutation(genotype);
    dirty |= addCircleMutation(genotype);
    dirty |= removeCircleMutation(genotype);
}