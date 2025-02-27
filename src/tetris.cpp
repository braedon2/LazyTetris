#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>
#include <iso646.h>
#include <iostream>

#include <raylib.h>

#include "tetris.h"
#include "constants.h"

/***********
 * Tetrimino
 ***********/

Tetrimino::Tetrimino(TetriminoShape shape) {
    this->shape = shape;
    this->rotationList = rotationListMap.at(shape);
    this->xDelta = 0;
    this->yDelta = 0;
    this->rotationStep = 0;
}

Tetrimino::Tetrimino(TetriminoShape shape, int xDelta, int yDelta, int rotationStep) {
    this->shape = shape;
    this->rotationList = rotationListMap.at(shape);
    this->xDelta = xDelta;
    this->yDelta = yDelta;
    this->rotationStep = rotationStep % this->rotationList.size();
}

std::vector<Position> Tetrimino::getPositions() {
    std::vector<Position> positions;
    for (Position p : this->rotationList[this->rotationStep]) {
        positions.push_back({p.x + this->xDelta, p.y + this->yDelta});
    }
    return positions;
}

Tetrimino Tetrimino::move(Direction direction) {
    if (direction == down) {
        return Tetrimino(this->shape, this->xDelta, this->yDelta + 1, this->rotationStep);
    }
    else if (direction == right) {
        return Tetrimino(this->shape, this->xDelta + 1, this->yDelta, this->rotationStep);
    }
    else { // direction == left
        return Tetrimino(this->shape, this->xDelta - 1, this->yDelta, this->rotationStep);
    }
}

Tetrimino Tetrimino::rotate(Rotation rotation) {
    int rotationStep;
    if (rotation == clockwise) {
        rotationStep = (this->rotationStep + 1) % this->rotationList.size();
    }
    else { // rotation == counterClockwise
        rotationStep = (this->rotationStep - 1) % this->rotationList.size();
    }
    return Tetrimino(this->shape, this->xDelta, this->yDelta, rotationStep);
}

SpriteType Tetrimino::getSpriteType() {
    return spriteTypeMap.at(this->shape);
}

/**********
 * GameGrid
 **********/

GameGrid::GameGrid() {
    for (int i = 0; i < GRID_HEIGHT; i++) {
        grid.push_back(std::vector<GridCell>(GRID_WIDTH, GridCell(none, true)));
    }
}

bool GameGrid::isEmpty(Position p) {
    return this->grid[p.y][p.x].isEmpty;
}

SpriteType GameGrid::getSpriteType(Position p) {
    return this->grid[p.y][p.x].spriteType;
}

void GameGrid::setCells(Tetrimino tetrimino) {
    for (Position p : tetrimino.getPositions()) {
        this->grid[p.y][p.x] = GridCell(tetrimino.getSpriteType(), false);
    }
}

void GameGrid::clearCell(Position p) {
    this->grid[p.y][p.x] = GridCell(none, true);
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
            std::vector<GridCell>(GRID_WIDTH, GridCell(none, true)));
    }
}

void GameGrid::print() {
    for (const auto& row : this->grid) {
        std::cout << "|";
        for (const auto& cell : row) {
            if (cell.isEmpty) {
                std::cout << " ";
            } 
            else {
                std::cout << "*";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "------------" << std::endl;
}

/***********
 * GameState
 ***********/

GameState::GameState() {
    this->currentTetrimino = Tetrimino(static_cast<TetriminoShape>(rand() % numTetriminoShapes)); 
    this->currentTetrimino.xDelta = SPAWN_X_DELTA;

    this->nextTetrimino = Tetrimino(static_cast<TetriminoShape>(rand() % numTetriminoShapes));
}

GameGrid GameState::getGrid() { return this->grid; }
Tetrimino GameState::getCurrentTetrimino() { return this->currentTetrimino; }
Tetrimino GameState::getNextTetrimino() { return this->nextTetrimino; }
bool GameState::isCurrentTetrominoPlaced() { return this->isCurrentTetriminoPlaced; }

void GameState::initNewTetrimino() { 
    this->currentTetrimino = this->nextTetrimino;
    this->currentTetrimino.xDelta = SPAWN_X_DELTA;

    this->nextTetrimino = Tetrimino(static_cast<TetriminoShape>(rand() % numTetriminoShapes)); 
    this->isCurrentTetriminoPlaced = false;

    if (this->grid.checkCollision(this->currentTetrimino.getPositions())) {
        this->gameOver = true;
    }
}

void GameState::moveTetrimino(Direction direction) {
    // tetromino can no longer be moved so command is ignored
    if (this->isCurrentTetriminoPlaced) {
        return;
    }

    if (direction == down) {
        Tetrimino tmpTetrimino = this->currentTetrimino.move(down);
        if (this->grid.checkCollision(tmpTetrimino.getPositions())) {
            for (auto p : this->currentTetrimino.getPositions()) {
                this->grid.setCells(this->currentTetrimino);
            }
            this->isCurrentTetriminoPlaced = true;
            this->linesToClear = this->grid.getFullRows();
        }
        else {
            this->currentTetrimino = tmpTetrimino;
        }
    }
    else { // direction is left or right
        Tetrimino tmpTetrimino = this->currentTetrimino.move(direction);
        if (not this->grid.checkCollision(tmpTetrimino.getPositions())) {
            this->currentTetrimino = tmpTetrimino;
        }
    }
}

void GameState::rotateTetrimino(Rotation rotation) {
    Tetrimino tmpTetrimino = this->currentTetrimino.rotate(rotation);
    if (not this->grid.checkCollision(tmpTetrimino.getPositions())) {
        this->currentTetrimino = tmpTetrimino;
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

    // update a bunch of state after the line clear is finished
    if (this->lineClearStep == GRID_WIDTH / 2) {
        // update level if enough lines have been cleared
        if (this->linesCleared + int(this->linesToClear.size()) >= this->level * LINE_CLEARS_PER_LEVEL) {
            this->level++;
        }

        this->linesCleared += this->linesToClear.size();
        this->grid.clearRows(this->linesToClear);
        this->lineClearStep = 0;
        this->linesToClear.clear();
    }
}

Sprites::Sprites() {
    for (auto colors : levelColors) {
        std::vector<Texture2D> spriteList;
        spriteList.push_back(this->generateSprite(0, colors.at(0)));
        spriteList.push_back(this->generateSprite(1, colors.at(0)));
        spriteList.push_back(this->generateSprite(1, colors.at(1)));
        this->sprites.push_back(spriteList);
    }
}

Texture2D Sprites::generateSprite(int pixelLayoutIndex, Color color) {
    std::vector<Color> imageData;
    Texture2D sprite;
    Image image;
    for (int pixel : spritePixelLayouts.at(pixelLayoutIndex)) {
            if (pixel) {
                imageData.push_back(color);
            }
            else {
                imageData.push_back(WHITE);
            }
        }
    image = {
        .data = imageData.data(),
        .width = sprite_width,
        .height = sprite_height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    sprite = LoadTextureFromImage(image);
    return sprite;
}

Texture2D Sprites::getSprite(SpriteType spriteType, int level) {
    return this->sprites.at((level - 1) % this->sprites.size()).at(spriteType);
}

FrameDrawer::FrameDrawer() {
    this->font = LoadFontEx("resources/CommitMonoNerdFont-Regular.otf", 16, NULL, 0);
    SetTextureFilter(this->font.texture, TEXTURE_FILTER_BILINEAR);
}

int FrameDrawer::getHorizontalOffset(Tetrimino tetrimino) {
    int ret = 10; // arbitrary large number
    for (auto pos : tetrimino.getPositions()) {
        if (pos.x < ret) {
            ret = pos.x;
        }
    }
    return -ret;
}

void FrameDrawer::drawCurrentTetrimino(GameState& state) {
    if (not state.isCurrentTetrominoPlaced()) {
        Tetrimino tetrimino = state.getCurrentTetrimino();
        SpriteType spriteType = tetrimino.getSpriteType();
        Texture2D sprite = this->sprites.getSprite(spriteType, state.level);
        for (auto gridPos : tetrimino.getPositions()) {
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

void FrameDrawer::drawGridCells(GameState& state) {
    GameGrid grid = state.getGrid();
    for (int gridX = 0; gridX < GRID_WIDTH; gridX++) {
        for (int gridY = 0; gridY < GRID_HEIGHT; gridY++) {
            Position gridPos(gridX, gridY);

            if (!grid.isEmpty(gridPos)) {
                Texture2D sprite = this->sprites.getSprite(
                    grid.getSpriteType(gridPos), state.level);
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
}

void FrameDrawer::drawSideBar(GameState& state) {
    float yStart = 10;

    DrawLine(GRID_FRAME_WIDTH, 0, GRID_FRAME_WIDTH, GRID_FRAME_HEIGHT, WHITE);

    // draw level in side bar
    DrawTextEx(this->font, "Level:", {GRID_FRAME_WIDTH + 10, yStart}, 16, 0, WHITE);
    std::stringstream ss1;
    ss1 << std::setfill('0') << std::setw(4) << state.level;
    DrawTextEx(this->font, ss1.str().c_str(), {GRID_FRAME_WIDTH + 10, yStart + 16} , 16, 0, WHITE);

    yStart += 44;

    // draw number of line clears in side bar
    DrawTextEx(this->font, "Lines:", {GRID_FRAME_WIDTH + 10, yStart}, 16, 0, WHITE);
    std::stringstream ss2;
    ss2 << std::setfill('0') << std::setw(4) << state.linesCleared;
    DrawTextEx(this->font, ss2.str().c_str(), {GRID_FRAME_WIDTH + 10, yStart + 16} , 16, 0, WHITE);

    yStart += 44;

    // draw next tetrimino in side bar
    DrawTextEx(this->font, "Next:", {GRID_FRAME_WIDTH + 10, yStart}, 16, 0, WHITE);
    Tetrimino tetrimino = state.getNextTetrimino();
    SpriteType spriteType = spriteTypeMap.at(tetrimino.shape);
    Texture2D sprite = this->sprites.getSprite(spriteType, state.level);
    auto positions = rotationListMap.at(tetrimino.shape)[0];
    int xAdjust = this->getHorizontalOffset(tetrimino);
    for (auto pos : positions) {
        float x = GRID_FRAME_WIDTH + 10 + ((pos.x + xAdjust) * BLOCK_SIZE) + ((pos.x + xAdjust) * GAP_SIZE);
        float y = yStart + 20 + (BLOCK_SIZE * pos.y) + (pos.y * GAP_SIZE);
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

void FrameDrawer::drawGameOver(int level) {
    Color color1 = levelColors.at((level - 1) % levelColors.size()).at(0);
    Color color2 = levelColors.at((level - 1) % levelColors.size()).at(1);

    for (int i = 0; i < this->gameOverStep; i++) {
        int y1 = (i * BLOCK_SIZE) + (i * GAP_SIZE);
        int y2 = y1 + 3;
        int y3 = y1 + BLOCK_SIZE - 3;
        int y4 = y3 + GAP_SIZE;

        DrawRectangle(0, y1, GRID_FRAME_WIDTH, 3, color1);
        DrawRectangle(0, y2, GRID_FRAME_WIDTH, BLOCK_SIZE - 6, WHITE);
        DrawRectangle(0, y3, GRID_FRAME_WIDTH, 3, color2);
        DrawRectangle(0, y4, GRID_FRAME_WIDTH, 3, BLACK);
    }
}

void FrameDrawer::drawFrame(GameState& state) {
    BeginDrawing();
        ClearBackground(BLACK);
        this->drawCurrentTetrimino(state);
        this->drawGridCells(state);
        this->drawSideBar(state);

        if (state.gameOver) {
            this->drawGameOver(state.level);
        }
    EndDrawing();
}

void FrameDrawer::nextGameOverStep() {
    if (this->gameOverStep < GRID_HEIGHT) {
        this->gameOverStep++;
    }
}

void FrameCounter::nextFrame() {
    this->framesPerGridCellCounter++;
    this->framesPerSoftDropCounter++;
    this->framesPerTetriminoResetCounter++;
    this->framesPerLineClearCounter++;
    this->framesPerGameOverStepCounter++;
}

void FrameCounter::resetCounters() {
    this->framesPerGridCellCounter = 0;
    this->framesPerSoftDropCounter = 0;
    this->framesPerTetriminoResetCounter = 0;
    this->framesPerLineClearCounter = 0;
    this->framesPerGameOverStepCounter = 0;
}