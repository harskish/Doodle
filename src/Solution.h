#pragma once
#include "Phenotype.h"

// Solution used in Simulated Annealing, has its own probabilities
class Solution : public Phenotype
{
public:
	Solution(SDL_Surface const* reference) : Phenotype(reference) {};

    void writeProbs(std::ofstream &out);
	void mutate(int iteration); // used to find neighbors

private:
	struct
	{
        int random = 1;       // per gene
		int shuffle = 5;      // once
		int addCircle = 0;    // once
		int removeCircle = 0; // once
		int perturbation = 2; // per gene
	} probs;
};