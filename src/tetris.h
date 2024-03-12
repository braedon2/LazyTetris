#ifndef TETRIS_H
#define TETRIS_H

#include <vector>

#include "raylib.h"

#include "util.h"
#include "constants.h"

enum Rotation { clockwise, counterClockwise };
enum Direction { down, right, left };
enum TetronimoShape { l, J, L, O, S, T, Z };
const int numTetronimoShapes = 6;

/*
* Maps a tetronimo shape to a sprite type of which each level has four.
* The mapped number represents which file the texture comes from.
* For example, if the game is on level one, tetronimo of shape Z maps to the texture
* found in level1-3.png since Z maps to integer 3
*/
const std::map<TetronimoShape,int> spriteMap = { {T, 1}, {J, 2}, {Z, 3}, {O, 4}, {S, 2}, {L, 3}, {l, 1}}; 

/*
* Maps each tetronimo shape to a list of initial positions
* The first item of each rotation list is the initial position for a newly spawned tetronimo of 
* that shape. The next list of positions for a shape are the positions of the tetronimo if it were
* rotated clockwise. 
*
* For example:
* The first item in the rotation list for tetronimo shape T looks like
*  000
*   0
* The second item in the rotation list is the previous representation rotated 90 degrees clockwise
*   0
*  00
*   0
*/
const std::map<TetronimoShape, std::vector<std::vector<Position>>> rotationListMap = {
    { T, { 
        {{4,0}, {5,0}, {6,0}, {5,1}},
        {{5,-1}, {4,0}, {5,0}, {5,1}},
        {{5,-1}, {4,0}, {5,0}, {6,0}},
        {{5,-1}, {5,0}, {6,0}, {5,1}}
    }},
    { l, {
        {{3,0}, {4,0}, {5,0}, {6,0}},
        {{5,-2}, {5,-1}, {5,0}, {5,1}}
    }},
    { O, {
        {{4,0}, {5,0}, {4,1}, {5,1}}
    }},
    {J, {
        {{4,0}, {5,0}, {6,0}, {6,1}},
        {{5,-1}, {5,0}, {4,1}, {5,1}},
        {{4,-1}, {4,0}, {5,0}, {6,0}},
        {{5,-1}, {6,-1}, {5,0}, {5,1}}
    }},
    {L, {
        {{4,0}, {5,0}, {6,0}, {4,1}},
        {{4,-1}, {5,-1}, {5,0}, {5,1}},
        {{6,-1}, {4,0}, {5,0}, {6,0}},
        {{5,-1}, {5,0}, {5,1}, {6,1}}
    }},
    {S, {
        {{5,0}, {6,0}, {4,1}, {5,1}},
        {{5,-1}, {5,0}, {6,0}, {6,1}}
    }},
    {Z, {
        {{4,0}, {5,0}, {5,1}, {6,1}},
        {{6,-1}, {5,0}, {6,0}, {5,1}}
    }}
};

class GridCell {
    public:
    int spriteType;
    bool isEmpty;
    GridCell(int _spriteType, bool _isEmpty): spriteType(_spriteType), isEmpty(_isEmpty) {}
};

class Tetronimo {
    private:
    
    std::vector<std::vector<Position>> rotationList;
    int xDelta;
    int yDelta;
    int rotationStep;

    public:
    TetronimoShape shape;
    Tetronimo(TetronimoShape shape);
    Tetronimo(TetronimoShape shape, int xDelta, int yDelta, int rotationStep);
    std::vector<Position> getPositions();
    Tetronimo move(Direction direction);
    Tetronimo rotate(Rotation rotation);
};

class GameGrid {
    private:
    std::vector<std::vector<GridCell>> grid; // first dimension is row, second dimension is column

    public:
    GameGrid();
    bool isEmpty(Position p);
    int getSpriteType(Position p); // returns a number between 1-4, or 0 if the cell is empty
    void setCell(Position p, int spriteType);
    void clearCell(Position p);
    bool checkCollision(std::vector<Position> positions);
    std::vector<int> getFullRows();
    void clearRows(std::vector<int> row_indexes);
};

class GameState {
    /*
    * - grid: keeps track of the fallen tetrominos that can no longer be moved. This usually does not 
    * include the current tetromino.
    * - currentTetronimo: the falling tetronimo being controlled by the player
    * - isCurrentTetronimoPlaced: Flag keeps track of if the current tetromino has been placed in the grid. 
    * The flag is set by moveTetronimo member function when the current tetronimo can no longer move
    * down and is placed in the grid.the flag is unset when initNewTetronimo is called.
    * - linesToClear: used for animating line clears. Grid indices of rows being cleared
    * - lineClearStep: current step in the row clear animation
    */
    private:
    GameGrid grid;
    Tetronimo currentTetronimo = Tetronimo(T);
    bool isCurrentTetronimoPlaced = false;
    std::vector<int> linesToClear;
    int lineClearStep = 0; 

    public:
    int linesCleared = 0;

    GameState();
    GameGrid getGrid();
    Tetronimo getCurrentTetronimo();
    bool isCurrentTetrominoPlaced();
    void initNewTetronimo();

    /* 
    * Moves the current tetromino one cell in the given direction so long as it does not cause a 
    * collision with the grid bounds or placed tetrominoes.
    * Left/right movements are ignored if they would cause a collision
    * A down movement that results in a collision causes the current tetromino to be placed where it 
    * is in the grid and a new tetromino to be initialized
    */
    void moveTetronimo(Direction direction);

    /*
    * Rotates the current tetronimo as long as it doesn't result in a collision. Rotations
    * that would cause a collision are ignored
    */
    void rotateTetronimo(Rotation rotation);

    /* 
    * returns true if a row clear animation is in progress. This means that rowsToClear and 
    * rowClearStep attributes are set
    */
    bool isLineClearInProgress();

    // Advances the row clear animation.
    // Unsets the rowsToClear and rowClearStep attributes when the animation is finished
    void nextLineClearStep();
};


class FrameDrawer {
    private:
    std::vector<std::vector<Texture2D>> levelTextures;
    Font font;

    public:
    FrameDrawer(); // loads all the sprite textures into levelTextures
    void drawFrame(GameState state);
};

#endif