#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "tetromino.h"
#include "raylib.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 700
#define STAGE_WIDTH 12
#define STAGE_HEIGHT 22
#define TILE_SIZE 24
#define GAME_TITLE "TETRIS"
#define FPS 60
#define MOVE_TIMER_MAX 1.0f
#define TETROMINO_START_X STAGE_WIDTH * 0.5f
#define TETROMINO_START_Y 0

typedef enum CustomizedColor
{
    MY_CYAN,
    MY_YELLOW,
    MY_PURPLE,
    MY_GREEN,
    MY_RED,
    MY_BLUE,
    MY_ORANGE,
    MY_GREY,
    LAST_COLOR,
} CustomizedColor;

const Color colors[] =
{
    {0, 255, 255, 255}, //Cyan
    {255, 255, 0, 255}, //Yellow
    {128, 0, 128, 255}, //Purple
    {0, 255, 0, 255}, //Green
    {255, 0, 0, 255}, //Red
    {0, 0, 255, 255}, //Blue
    {255, 127, 0, 255}, //Orange
    {127, 127, 127, 255}, //Grey
};

typedef struct Tetromino
{
    int positionX;
    int positionY;
    TetrominoType type;
    TetrominoRotation rotation;
    Color color;
    TetrominoDefinition* data;
} Tetromino;

typedef struct Stage
{
    int centerX;
    int centerY;
    int blocks[STAGE_WIDTH * STAGE_HEIGHT];
    Color color;
} Stage;

void InitGame(Stage* stage);
void InitTetromino(Tetromino* tetromino);
bool CheckCollision(const Tetromino* tetromino, const Stage stage);
void CopyTetromino(const Tetromino* source, Tetromino* destination);
void ManageRotation(Tetromino* tetromino, const Stage stage);
void ManageHorizontalMovement(Tetromino* tetromino, const Stage stage);
bool ManageTimer(float* moveTimer, const float moveTimerMax);
void ShiftLineDown(const int startLineY, Stage* stage);
void DeleteLines(Stage* stage);
void MoveTetrominoDown(Tetromino* tetromino, Stage* stage);
void DrawStage(const Stage stage);
void DrawTetromino(const Tetromino tetromino, const Stage stage);