#include "tetris.h"
#include "constants.h"

/**********
 * GameGrid
 **********/

GameGrid::GameGrid() {
    for (int i = 0; i < GRID_HEIGHT; i++) {
        grid.push_back(std::vector<GridCell>(GRID_WIDTH, GridCell(BLANK, true)));
    }
}

bool GameGrid::isEmpty(Position p) {
    return this->grid[p.y][p.x].isEmpty;
}

Color GameGrid::getColor(Position p) {
    return this->grid[p.y][p.x].color;
}

void GameGrid::setCell(Position p, Color color) {
    this->grid[p.y][p.x] = GridCell(color, false);
}

void GameGrid::clearCell(Position p) {
    this->grid[p.y][p.x] = GridCell(BLANK, true);
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

std::vector<int> GameGrid::getFullRows() {
    std::vector<int> fullRows;
    int currentRow = 0;
    for (auto row : this->grid) {
        bool foundEmptyCell = false;
        for (auto cell : row) {
            foundEmptyCell = foundEmptyCell or cell.isEmpty;
        }
        if (not foundEmptyCell) {
            fullRows.push_back(currentRow);
        }
        currentRow++;
    }
    return fullRows;
}

/***********
 * GameState
 ***********/

GameGrid GameState::getGrid() { return this->grid; }
Position GameState::getCurrentTetronimo() { return this->currentTetronimo; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetronimoPlaced; }
std::vector<int> GameState::getRowsToClear() { return this->grid.getFullRows(); }

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