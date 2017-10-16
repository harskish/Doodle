#include "AnnealingOptimizer.h"

AnnealingOptimizer::AnnealingOptimizer(SDL_Surface const * reference) : Optimizer(reference)
{
    steps = 0;
    stepsWithoutImprovement = 0;
    T = T0;
    current = new Solution(reference);

    progressStream.open("out/sa_stats.csv", std::ofstream::out | std::ofstream::trunc);
    progressStream << "iteration,temperature,current,best\n";
}

AnnealingOptimizer::~AnnealingOptimizer()
{
    delete current;
    progressStream.close();
}

void AnnealingOptimizer::saveImage()
{
	std::string filename = "out/sa_out_" + std::to_string(steps / 1000) + "k.bmp";
	SDL_SaveBMP(this->currentBest, filename.c_str());
}

void AnnealingOptimizer::writeProgress()
{
    auto fitnessPercentage = [&](double f) { return f / (255UL * 255UL * 3UL * target->w * target->h); };

    double curr = fitnessPercentage(current->fitness());
    double currScaled = 100 * pow(curr, 30);
    double best = fitnessPercentage(bestSeenFitness);
    double bestScaled = 100 * pow(best, 30);

    printf("[AnnealingOptimizer] Iteration: %dk, T: %.2f, fitness: %.2f%% (best: %.2f%%)\n", steps / 1000, T, currScaled, bestScaled);

    // Write to csv file
    if (!progressStream.good())
        std::cout << "Could not export progress to csv" << std::endl;
    else
        progressStream << steps << "," << T << "," << curr << "," << best << std::endl << std::flush;
}

bool AnnealingOptimizer::step()
{
    // Cooling functions
    auto alpha1 = [](float T) -> float
    {
        const float k = 1.0f - 1e-5f;
        return k*T;
    };

    auto alpha2 = [](float T) -> float
    {
        const float k = 0.005f;
        return T / (1 + k*T);
    };

    // Acceptance probability, ascent -> always accept, descent -> accept probabilistically
    auto P = [](float T, float deltaF) -> float
    {
        return (deltaF > 0) ? 1.0f : exp(deltaF / T);
    };

    // Pick a random neighbor
    Solution n = *current;
    n.mutate();

    // Check acceptance
    bool newBestFound = false;
    float deltaF = float(n.fitness() - current->fitness());
    float r = (float)rand() / RAND_MAX;
    if (P(T, deltaF) >= r)
    {
        *current = n;

        if (current->fitness() > bestSeenFitness)
        {
            stepsWithoutImprovement = 0;
            SDL_Surface* best = current->getImage();
            this->bestSeenFitness = current->fitness();

            if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
                throw std::runtime_error("Failed to update current best solution");

            newBestFound = true;
        }
        else
        {
            stepsWithoutImprovement++;
        }
    }

    // Cooling
    T = alpha1(T);
    steps++;

	if (steps % 1000 == 0)
		writeProgress();
	if (steps % 5000 == 0)
		saveImage();

    return newBestFound;
}
