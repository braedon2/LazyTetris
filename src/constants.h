#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>

const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 12;
const int GAP_SIZE = 2;
const int GRID_FRAME_WIDTH = GRID_WIDTH * (BLOCK_SIZE + GAP_SIZE) + GAP_SIZE;
const int GRID_FRAME_HEIGHT = GRID_HEIGHT * (BLOCK_SIZE + GAP_SIZE) + GAP_SIZE;

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

const int LINE_CLEARS_PER_LEVEL = 5;

#endif