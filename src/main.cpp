#include "raylib.h"
#include "raymath.h"

class Timer {
    private:
        double startTime;
    public:
        Timer() {
            this->startTime = GetTime();
        }

        void start() {
            this->startTime = GetTime();
        }

        double getElapsed() {
            return GetTime() - this->startTime;
        }
};

int main(void) { 
    InitWindow(50, 250, "Template-4.0.0");
    Timer timer;
    SetTargetFPS(60);
    int pos = 0;

    while (!WindowShouldClose()) {
        if (timer.getElapsed() > 1 and pos < 9) {
            timer.start();
            pos++;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(0, 25 * pos, 25, 25, RED);
            DrawRectangleLines(0, 25 * pos, 25, 25, BLACK);
        EndDrawing();
    }
    
    CloseWindow();
    
    return 0;
}