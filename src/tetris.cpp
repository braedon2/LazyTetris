#include <cstdlib>

#include <raylib.h>

#include "tetris.h"
#include "constants.h"

/***********
 * Tetronimo
 ***********/

Tetronimo::Tetronimo(TetronimoType shape) {
    this->shape = shape;
    this->rotationList = rotationListMap.at(shape);
    this->xDelta = 0;
    this->yDelta = 0;
    this->rotationStep = 0;
}

Tetronimo::Tetronimo(TetronimoType shape, int xDelta, int yDelta, int rotationStep) {
    this->shape = shape;
    this->rotationList = rotationListMap.at(shape);
    this->xDelta = xDelta;
    this->yDelta = yDelta;
    this->rotationStep = rotationStep % this->rotationList.size();
}

std::vector<Position> Tetronimo::getPositions() {
    std::vector<Position> positions;
    for (Position p : this->rotationList[this->rotationStep]) {
        positions.push_back({p.x + this->xDelta, p.y + this->yDelta});
    }
    return positions;
}

Tetronimo Tetronimo::move(Direction direction) {
    if (direction == down) {
        return Tetronimo(this->shape, this->xDelta, this->yDelta + 1, this->rotationStep);
    }
    else if (direction == right) {
        return Tetronimo(this->shape, this->xDelta + 1, this->yDelta, this->rotationStep);
    }
    else { // direction == left
        return Tetronimo(this->shape, this->xDelta - 1, this->yDelta, this->rotationStep);
    }
}

Tetronimo Tetronimo::rotate(Rotation rotation) {
    int rotationStep;
    if (rotation == clockwise) {
        rotationStep = (this->rotationStep + 1) % this->rotationList.size();
    }
    else { // rotation == counterClockwise
        rotationStep = (this->rotationStep - 1) % this->rotationList.size();
    }
    return Tetronimo(this->shape, this->xDelta, this->yDelta, rotationStep);
}

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

bool GameGrid::checkCollision(std::vector<Position> positions) {
    for (auto p : positions) {
        if (p.x < 0 or p.x >= GRID_WIDTH) {
            return true;
        }
        if (p.y >= GRID_HEIGHT) {
            return true;
        }
        if (p.y >= 0 && !this->isEmpty(p)) {
            return true;
        }
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

void GameGrid::clearRows(std::vector<int> row_indices) {
    // for each cleared row, delete the row then insert an empty row at the beginning of the list
    // so all the rows above it appear to shift down one level in the grid
    for (int i : row_indices) {
        this->grid.erase(this->grid.begin() + i);
        this->grid.insert(
            this->grid.begin(), 
            std::vector<GridCell>(GRID_WIDTH, GridCell(BLANK, true)));
    }
}

/***********
 * GameState
 ***********/

GameState::GameState() {
    this->currentTetronimo = Tetronimo(static_cast<TetronimoType>(rand() % numTetronimoTypes)); 
}

GameGrid GameState::getGrid() { return this->grid; }
Tetronimo GameState::getCurrentTetronimo() { return this->currentTetronimo; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetronimoPlaced; }

void GameState::initNewTetronimo() { 
    this->currentTetronimo = Tetronimo(static_cast<TetronimoType>(rand() % numTetronimoTypes)); 
    this->isCurrentTetronimoPlaced = false;
}

void GameState::moveTetronimo(Direction direction) {
    // tetromino can no longer be moved so command is ignored
    if (this->isCurrentTetronimoPlaced) {
        return;
    }

    if (direction == down) {
        Tetronimo tmpTetronimo = this->currentTetronimo.move(down);
        if (this->grid.checkCollision(tmpTetronimo.getPositions())) {
            for (auto p : this->currentTetronimo.getPositions()) {
                this->grid.setCell(p, RED);
            }
            this->isCurrentTetronimoPlaced = true;
            this->linesToClear = this->grid.getFullRows();
        }
        else {
            this->currentTetronimo = tmpTetronimo;
        }
    }
    else { // direction is left or right
        Tetronimo tmpTetronimo = this->currentTetronimo.move(direction);
        if (not this->grid.checkCollision(tmpTetronimo.getPositions())) {
            this->currentTetronimo = tmpTetronimo;
        }
    }
}

void GameState::rotateTetronimo(Rotation rotation) {
    Tetronimo tmpTetronimo = this->currentTetronimo.rotate(rotation);
    if (not this->grid.checkCollision(tmpTetronimo.getPositions())) {
        this->currentTetronimo = tmpTetronimo;
    }
}

bool GameState::isLineClearInProgress() {
    return not this->linesToClear.empty();
}

void GameState::nextLineClearStep() {
    for (int lineIndex : this->linesToClear) {
        this->grid.clearCell(Position(LEFT_MIDDLE_INDEX - this->lineClearStep, lineIndex));
        this->grid.clearCell(Position(RIGHT_MIDDLE_INDEX + this->lineClearStep, lineIndex));
    }
    this->lineClearStep++;
    if (this->lineClearStep == GRID_WIDTH / 2) {
        this->linesCleared += this->linesToClear.size();
        this->grid.clearRows(this->linesToClear);
        this->lineClearStep = 0;
        this->linesToClear.clear();
    }
}