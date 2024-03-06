#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>

const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 12;
const int GAP_SIZE = 2;
const int WINDOW_WIDTH = GRID_WIDTH * (BLOCK_SIZE + GAP_SIZE) + GAP_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * (BLOCK_SIZE + GAP_SIZE) + GAP_SIZE;

// For animating line clears.
// Assumes grid width is even, LEFT_MIDDLE_INDEX is the index along a row to the left of the
// midpoint. E.g GRID_WIDTH=10, midpoint between index 0 and 9 is 4.5 so left and right 
// indices would be 4 and 5 respectively
const int LEFT_MIDDLE_INDEX = (GRID_WIDTH / 2) - 1;
const int RIGHT_MIDDLE_INDEX = GRID_WIDTH / 2;

// frame counters for controlling the speed of animations
const int FRAMES_PER_GRID_CELL = 24; // speed at which the tetronimo falls taken from https://tetris.fandom.com/wiki/Tetris_(NES,_Nintendo)
const int FRAMES_PER_SOFT_DROP = 1; // falling speed down down arrow key is pressed
const int FRAMES_PER_TETRONIMO_RESET = 5; // frames between tetronimo being placed and a new one initializing
const int FRAMES_PER_LINE_CLEAR = 2; // speed of the line clearing animation

enum Rotation { clockwise, counterClockwise };

enum Direction {
    down,
    right,
    left
};

enum TetronimoType { l, J, L, O, S, T, Z };
const int numTetronimoTypes = 6;

/*
* Maps each tetronimo type to a list of initial positions
* The first item of each rotation list is the initial position for a newly spawned tetronimo of 
* that type. The next list of positions for a type are the positions of the tetronimo if it were
* rotated clockwise. 
*
* For example:
* The first item in the rotation list for tetronimo type T looks like
*  000
*   0
* The second item in the rotation list is the previous representation rotated 90 degrees clockwise
*   0
*   00
*   0
*/
const std::map<TetronimoType, std::vector<std::vector<Position>>> rotationListMap = {
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

#endif