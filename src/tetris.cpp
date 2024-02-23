#include "tetris.h"
#include "constants.h"

/**********
 * GameGrid
 **********/

GameGrid::GameGrid() {
    for (int i = 0; i < GRID_WIDTH; i++) {
        grid.push_back(std::vector<GridCell>(GRID_HEIGHT, GridCell(BLANK, true)));
    }
}

bool GameGrid::isEmpty(Position p) {
    return this->grid[p.x][p.y].isEmpty;
}

Color GameGrid::getColor(Position p) {
    return this->grid[p.x][p.y].color;
}

void GameGrid::setCell(Position p, Color color) {
    this->grid[p.x][p.y] = GridCell(color, false);
}

void GameGrid::clearCell(Position p) {
    this->grid[p.x][p.y] = GridCell(BLANK, true);
}

bool GameGrid::checkCollision(Position p) {
    if (p.x < 0 or p.x >= GRID_WIDTH) {
        return true;
    }
    if (p.y >= GRID_HEIGHT) {
        return true;
    }
    if (!this->isEmpty(p)) {
        return true;
    }
    return false;
}

/***********
 * GameState
 ***********/

GameGrid GameState::getGrid() { return this->grid; }
Position GameState::getCurrentTetronimo() { return this->currentTetronimo; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetronimoPlaced; }

void GameState::initNewTetronimo() { 
    this->currentTetronimo = Position(0, 0); 
    this->isCurrentTetronimoPlaced = false;
}

void GameState::moveTetronimo(Direction direction) {
    if (this->isCurrentTetronimoPlaced) {
        return;
    }

    if (direction == down) {
        Position tmpTetronimo(this->currentTetronimo.x, this->currentTetronimo.y + 1);
        if (this->grid.checkCollision(tmpTetronimo)) {
            this->grid.setCell(this->currentTetronimo, RED);
            this->isCurrentTetronimoPlaced = true;
        }
        else {
            this->currentTetronimo = tmpTetronimo;
        }
    }
    else if (direction == right) {
        Position tmpTetronimo(this->currentTetronimo.x + 1, this->currentTetronimo.y);
        if (not this->grid.checkCollision(tmpTetronimo)) {
            this->currentTetronimo = tmpTetronimo;
        }
    }
    else if (direction == left) {
        Position tmpTetronimo(this->currentTetronimo.x - 1, this->currentTetronimo.y);
        if (not this->grid.checkCollision(tmpTetronimo)) {
            this->currentTetronimo = tmpTetronimo;
        }
    }
}