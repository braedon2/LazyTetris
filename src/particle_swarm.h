#include <vector>

#include "solver.h"
#include "tetris.h"

#define INERTIA_WEIGHT 0.7298
#define ACC_COEFF 1.49618
#define X_MIN -50.0
#define X_MAX 50.0
#define V_MAX 10.0

class Particle {
public:
	std::vector<double> position;
	std::vector<double> velocity;
	std::vector<double> personalBestPosition;
	int personalBestScore;

	Particle() : personalBestScore(0) {};
	Particle(std::vector<double> initalPosition) : velocity(NUM_FACTORS, 0.0), position(initalPosition), personalBestPosition(NUM_FACTORS, 0.0), personalBestScore(0) {};
	void updateVeclocityAndPosition(std::vector<double>& globalBestPosition);
};

EvaluationWeights getWeights(Particle& particle);
std::vector<Tetrimino> initTetriminos(int numTetriminos);
std::vector<Particle> initParticles(int numParticles);
int runSimulation(std::vector<Tetrimino> tetriminos, EvaluationWeights weights);