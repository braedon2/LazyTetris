#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>

#include <raylib.h>

#include "tetris.h"
#include "constants.h"

/***********
 * Tetronimo
 ***********/

Tetronimo::Tetronimo(TetronimoShape shape) {
    this->shape = shape;
    this->rotationList = rotationListMap.at(shape);
    this->xDelta = 0;
    this->yDelta = 0;
    this->rotationStep = 0;
}

Tetronimo::Tetronimo(TetronimoShape shape, int xDelta, int yDelta, int rotationStep) {
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
        grid.push_back(std::vector<GridCell>(GRID_WIDTH, GridCell(0, true)));
    }
}

bool GameGrid::isEmpty(Position p) {
    return this->grid[p.y][p.x].isEmpty;
}

int GameGrid::getSpriteType(Position p) {
    return this->grid[p.y][p.x].spriteType;
}

void GameGrid::setCell(Position p, int spriteType) {
    this->grid[p.y][p.x] = GridCell(spriteType, false);
}

void GameGrid::clearCell(Position p) {
    this->grid[p.y][p.x] = GridCell(0, true);
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
            std::vector<GridCell>(GRID_WIDTH, GridCell(0, true)));
    }
}

/***********
 * GameState
 ***********/

GameState::GameState() {
    this->currentTetronimo = Tetronimo(static_cast<TetronimoShape>(rand() % numTetronimoShapes)); 
    this->nextTetronimo = Tetronimo(static_cast<TetronimoShape>(rand() % numTetronimoShapes));
}

GameGrid GameState::getGrid() { return this->grid; }
Tetronimo GameState::getCurrentTetronimo() { return this->currentTetronimo; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetronimoPlaced; }

void GameState::initNewTetronimo() { 
    this->currentTetronimo = this->nextTetronimo;
    this->nextTetronimo = Tetronimo(static_cast<TetronimoShape>(rand() % numTetronimoShapes)); 
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
                this->grid.setCell(p, spriteMap.at(this->currentTetronimo.shape));
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

FrameDrawer::FrameDrawer() {
    this->font = LoadFontEx("assets/CommitMonoNerdFont-Regular.otf", 28, NULL, 0);

    std::vector<std::string> spriteTypes = {"1", "2", "3", "4"};
    std::string level("1");

    std::vector<Texture2D> tmpList;
    for (auto spriteType : spriteTypes) {
        std::string filename = "assets/level" + level + "-" + spriteType + ".png";
        tmpList.push_back(LoadTexture(filename.c_str()));
    }
    this->levelTextures.push_back(tmpList);
}

void FrameDrawer::drawFrame(GameState state) {
    Texture2D sprite;
    BeginDrawing();
        ClearBackground(BLACK);
        // draw current tetronimo
        if (not state.isCurrentTetrominoPlaced()) {
            Tetronimo tetronimo = state.getCurrentTetronimo();
            int spriteIndex = spriteMap.at(tetronimo.shape) - 1;
            sprite = this->levelTextures[0][spriteMap.at(tetronimo.shape) - 1];
            for (auto gridPos : state.getCurrentTetronimo().getPositions()) {
                float x = (gridPos.x * BLOCK_SIZE) + (gridPos.x * GAP_SIZE) + GAP_SIZE;
                float y = (BLOCK_SIZE * gridPos.y) + (gridPos.y * GAP_SIZE) + GAP_SIZE;
                DrawTexturePro(
                    sprite,
                    { 0.0f, 0.0f, (float)sprite.width, (float)sprite.height },
                    { x, y, (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }
        }

        // draw grid cells
        GameGrid grid = state.getGrid();
        for (int gridX = 0; gridX < GRID_WIDTH; gridX++) {
            for (int gridY = 0; gridY < GRID_HEIGHT; gridY++) {
                Position gridPos(gridX, gridY);

                if (!grid.isEmpty(gridPos)) {
                    sprite = this->levelTextures[0][grid.getSpriteType(gridPos) - 1];
                    float x = (gridPos.x * BLOCK_SIZE) + (gridPos.x * GAP_SIZE) + GAP_SIZE;
                    float y = (BLOCK_SIZE * gridPos.y) + (gridPos.y * GAP_SIZE) + GAP_SIZE;

                    DrawTexturePro(
                        sprite,
                        { 0.0f, 0.0f, (float)sprite.width, (float)sprite.height },
                        { x, y, (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                }
            }
        }

        DrawLine(GRID_FRAME_WIDTH, 0, GRID_FRAME_WIDTH, GRID_FRAME_HEIGHT, WHITE);

        DrawTextEx(this->font, "Lines:", {GRID_FRAME_WIDTH + 10, 10}, 14, 0, WHITE);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << state.linesCleared;
        DrawTextEx(this->font, ss.str().c_str(), {GRID_FRAME_WIDTH + 10, 28} , 14, 0, WHITE);

        DrawTextEx(this->font, "Next:", {GRID_FRAME_WIDTH + 10, 56}, 14, 0, WHITE);
    EndDrawing();
}