#include "Solution.h"

void Solution::writeProbs(std::ofstream & out)
{
    out << "Shuffle: " << probs.shuffle << "%" << std::endl;
    out << "Add Circle: " << probs.addCircle << "%" << std::endl;
    out << "Remove Circle: " << probs.removeCircle << "%" << std::endl;
    out << "Perturbation: " << probs.perturbation << "%" << std::endl;
    out << "Initial circles: " << numCircles << std::endl;
    out << "Min X: " << geneToX((Gene)0) << std::endl;
    out << "Max R: " << geneToRadius((Gene)255) << std::endl;
}

void Solution::mutate(int iteration)
{
	auto perturbationMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		bool mutated = false;

		const int mutationRate = probs.perturbation;
		for (int g = 0; g < numCircles * genesPerCircle; g++)
		{
			if (rand() % 100 < mutationRate)
			{
                genotype[g] = std::max(0, std::min(255, genotype[g] + (-20 + rand() % 41)));
				mutated = true;
			}
		}

		return mutated;
	};

	auto shuffleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		if (numCircles == 1) return false;

		const int mutationRate = probs.shuffle;
		if (rand() % 100 >= mutationRate) return false;

		int idx1, idx2;
		idx1 = rand() % numCircles;
		while ((idx2 = rand() % numCircles) == idx1);

		for (int g = 0; g < genesPerCircle; g++)
		{
			std::swap(genotype[idx1 * genesPerCircle + g], genotype[idx2 * genesPerCircle + g]);
		}

		return true;
	};

	auto addCircleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		const int mutationRate = probs.addCircle;
		if (rand() % 100 < mutationRate)
		{
			addCircle();
			return true;
		}

		return false;
	};

    auto addCircleDecreasingMutation = [&](std::vector<Gene> &genotype) -> bool
    {
        const int mutationRate = probs.addCircle;
        if (rand() % 100 < mutationRate)
        {
            addCircle();
            float scale = 1.3f * powf(iteration + 500, -1.0f / 20.0f);
            auto R = genotype.end() - 5;
            *R = std::max((Gene)1, (Gene)(*R * scale));
            return true;
        }

        return false;
    };

	auto removeCircleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		if (numCircles == 1) return false;

		const int mutationRate = probs.removeCircle;
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
	dirty |= perturbationMutation(genotype);
	dirty |= shuffleMutation(genotype);
	dirty |= addCircleDecreasingMutation(genotype);
	dirty |= removeCircleMutation(genotype);
}