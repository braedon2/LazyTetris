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

GameGrid GameState::getGrid() { return this->grid; }
Position GameState::getCurrentTetronimo() { return this->currentTetronimo; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetronimoPlaced; }

void GameState::initNewTetronimo() { 
    this->currentTetronimo = Position(0, 0); 
    this->isCurrentTetronimoPlaced = false;
}

void GameState::moveTetronimo(Direction direction) {
    // tetromino can no longer be moved so command is ignored
    if (this->isCurrentTetronimoPlaced) {
        return;
    }

    if (direction == down) {
        Position tmpTetronimo(this->currentTetronimo.x, this->currentTetronimo.y + 1);
        if (this->grid.checkCollision(tmpTetronimo)) {
            this->grid.setCell(this->currentTetronimo, RED);
            this->isCurrentTetronimoPlaced = true;
            this->linesToClear = this->grid.getFullRows();
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
        this->grid.clearRows(this->linesToClear);
        this->lineClearStep = 0;
        this->linesToClear.clear();
    }
}