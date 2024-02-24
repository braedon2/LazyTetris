#ifndef CONSTANTS_H
#define CONSTANTS_H

const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 15;
const int WINDOW_WIDTH = GRID_WIDTH * BLOCK_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * BLOCK_SIZE;

// For animating line clears.
// Assumes grid width is even, LEFT_MIDDLE_INDEX is the index along a row to the left of the
// midpoint. E.g GRID_WIDTH=10, midpoint would between index 0 and 9 is 4.5 so left and right 
// indices would be 4 and 5 respectively
const int LEFT_MIDDLE_INDEX = (GRID_WIDTH / 2) - 1;
const int RIGHT_MIDDLE_INDEX = GRID_WIDTH / 2;

#endif