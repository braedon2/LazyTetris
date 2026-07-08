#include <iostream>
#include <random>

#include "particle_swarm.h"
#include "solver.h"
#include "tetris.h"

using namespace std;

int main(void) {
    auto tetriminos = initTetriminos(1000);
    auto particles = initParticles(30);
    vector<double> globalBestPosition;
    int globalBestScore = 0;

    while (true) {
        cout << endl << "**********" << endl;
        cout << "NEW RUN" << endl;
        cout << "**********" << endl << endl;

        for (Particle& particle : particles) {
            int score = runSimulation(tetriminos, getWeights(particle));
            cout << score << endl;

            if (score > particle.personalBestScore) {
                particle.personalBestScore = score;
            }
            if (score > globalBestScore) {
                globalBestScore = score;
                globalBestPosition = particle.position;
            }
        }

        for (Particle& particle : particles) {
            particle.updateVeclocityAndPosition(globalBestPosition);
        }

        cout << endl << "global best: " << globalBestScore << endl;
        for (double i : globalBestPosition) {
            cout << i << endl;
        }
    }

    return 0;
}

void Particle::updateVeclocityAndPosition(vector<double>& globalBestPosition) {
    random_device rd;
    mt19937_64 engine(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    double r1 = dist(engine);
    double r2 = dist(engine);

    double phi1 = ACC_COEFF * r1;
    double phi2 = ACC_COEFF * r2;

    for (int i = 0; i < this->velocity.size(); i++) {
        double v = this->velocity[i];
        double p = this->position[i];
        double personalBest = this->personalBestPosition[i];
        double globalBest = globalBestPosition[i];
        double newV = (INERTIA_WEIGHT * v) + (phi1 * (personalBest - p)) + (phi2 * (globalBest - p));

        newV >= V_MAX ? this->velocity[i] = V_MAX : this->velocity[i] = newV;
        this->position[i] = this->position[i] + this->velocity[i];
    }
}

EvaluationWeights getWeights(Particle& particle) {
    return {
        .totalLinesCleared = particle.position[0],
        .totalLockHeight = particle.position[1],
        .totalWellCells = particle.position[2],
        .totalColumnHoles = particle.position[3],
        .totalColumnTransitions = particle.position[4],
        .totalRowTransitions = particle.position[5],
        .totalDeepWells = particle.position[6]
    };
}

vector<Tetrimino> initTetriminos(int numTetriminos) {
    random_device rd;
    mt19937_64 engine(rd());
    uniform_int_distribution<int> dist(0, numTetriminoShapes - 1);
    vector<Tetrimino> tetriminos;
    
    for (int i = 0; i < numTetriminos; i++) {
        Tetrimino t = Tetrimino(static_cast<TetriminoShape>(dist(engine)));
        t.xDelta = SPAWN_X_DELTA;
        tetriminos.push_back(t);
    }

    return tetriminos;
}

vector<Particle> initParticles(int numParticles) {
    random_device rd;
    mt19937_64 engine(rd());
    uniform_real_distribution<double> dist(X_MIN, X_MAX);
    vector<Particle> particles;

    for (int i = 0; i < numParticles; i++) {
        vector<double> initialPosition;
        for (int p = 0; p < NUM_FACTORS; p++) {
            initialPosition.push_back(dist(engine));
        }
        Particle particle(initialPosition);
        particles.push_back(particle);
    }

    return particles;
}

int runSimulation(std::vector<Tetrimino> tetriminos, EvaluationWeights weights) {
    GameState state;
    int score = 0;

    for (int i = 0; i < tetriminos.size() - 1 and not state.gameOver; i++) {
        if (state.grid.checkCollision(tetriminos[i])) {
            state.gameOver = true;
        }

        Tetrimino tetriminoToPlace = solveForOptimalTetrimino(state.getGrid(), tetriminos[i], tetriminos[i + 1], weights);
        state.currentTetrimino = tetriminoToPlace;
        state.isCurrentTetriminoPlaced = false; // usually reset by initNewTetrimino but we are providing our own tetriminos
        state.moveTetrimino(down);

        if (state.isLineClearInProgress()) {
            switch (static_cast<int>(state.linesToClear.size())) {
                case 1: score += 40; break;
                case 2: score += 100; break;
                case 3: score += 300; break;
                case 4: score += 1200; break;
            }
            state.clearFullLines();
        }
    }

    return score;
}