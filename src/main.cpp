#include <raylib.h>
#include <raymath.h>
#include <vector>

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

class BoardCell {
    public:
        Color color;
        bool isEmpty;

        BoardCell(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

int main(void) { 
    InitWindow(100, 250, "Template-4.0.0");
    Timer timer;
    SetTargetFPS(60);
    int pos = 0;
    std::vector<BoardCell> board(10, BoardCell(BLANK, true));

    while (!WindowShouldClose()) {
        if (timer.getElapsed() > 1) {
            if (pos == 9) {
                board[pos] = BoardCell(RED, false);
                pos = 0;
            }
            else if (board[pos + 1].isEmpty) {
                pos++;
            }
            else if (!board[pos].isEmpty) {
                // game over
            }
            else {
                board[pos] = BoardCell(RED, false);
                pos = 0;
            }

            timer.start();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(0, 25 * pos, 25, 25, RED);
            DrawRectangleLines(0, 25 * pos, 25, 25, BLACK);

            for (int i = 0; i < 10; i++) {
                BoardCell cell = board[i];
                if (!cell.isEmpty) {
                    DrawRectangle(0, 25 * i, 25, 25, RED);
                    DrawRectangleLines(0, 25 * i, 25, 25, BLACK);
                }
            }
        EndDrawing();
    }
    
    CloseWindow();
    
    return 0;
}