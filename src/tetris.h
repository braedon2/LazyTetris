#ifndef TETRIS_H
#define TETRIS_H

#include <ranlib.h>
#include <vector>

#include "util.h"
#include "constants.h"

class GridCell {
    public:
    Color color;
    bool isEmpty;
    GridCell(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

class Tetronimo {
    private:
    TetronimoType shape;
    std::vector<std::vector<Position>> rotationList;
    int xDelta;
    int yDelta;
    int rotationStep;

    public:
    Tetronimo(TetronimoType shape);
    Tetronimo(TetronimoType shape, int xDelta, int yDelta, int rotationStep);
    std::vector<Position> getPositions();
    Tetronimo move(Direction direction);
    Tetronimo rotate(Rotation rotation);
};

class GameGrid {
    private:
    // first dimension is row, second dimension is column
    std::vector<std::vector<GridCell>> grid;

    public:
    GameGrid();
    bool isEmpty(Position p);
    Color getColor(Position p);
    void setCell(Position p, Color color);
    void clearCell(Position p);
    bool checkCollision(std::vector<Position> positions);
    std::vector<int> getFullRows();
    void clearRows(std::vector<int> row_indexes);
};

class GameState {
    private:
    // Keeps track of the fallen tetrominos that can no longer be moved. This usually does not
    // include the current tetromino
    GameGrid grid;

    // The falling tetromino being controlled by the player
    Tetronimo currentTetronimo = Tetronimo(T);

    // Flag keeps track of if the current tetromino has been placed in the grid. 
    // Flag is set by moveTetronimo member function when the current tetronimo can no longer move
    // down and is placed in the grid.
    // Flag is unset when initNewTetronimo is called.
    bool isCurrentTetronimoPlaced = false;

    // used for animating row clears
    std::vector<int> linesToClear; // indices of rows being cleared
    int lineClearStep = 0; 

    public:
    int linesCleared = 0;

    GameState();
    GameGrid getGrid();
    Tetronimo getCurrentTetronimo();
    bool isCurrentTetrominoPlaced();
    void initNewTetronimo();

    // Moves the current tetromino one cell in the given direction so long as it does not cause a 
    // collision with the grid bounds or placed tetrominoes.
    // Left/right movements are ignored if they would cause a collision
    // A down movement that results in a collision causes the current tetromino to be placed where it 
    // is in the grid and a new tetromino to be initialized
    void moveTetronimo(Direction direction);

    void rotateTetronimo(Rotation rotation);

    // returns true if a row clear animation is in progress. This means that rowsToClear and 
    // rowClearStep attributes are set
    bool isLineClearInProgress();

    // Advances the row clear animation.
    // Unsets the rowsToClear and rowClearStep attributes when the animation is finished
    void nextLineClearStep();
};

#endif