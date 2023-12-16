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
#define BLINKING_TIMER_MAX 1.5f
#define BLINKING_TICK_MAX 0.5f
#define INIT_SPEED 1.0f
#define SPEED_MULTIPLIER 2.0f
#define TETROMINO_START_X STAGE_WIDTH * 0.5f
#define TETROMINO_START_Y 0
#define MAIN_THEME_PATH "mainTheme.wav"

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

void InitGame(Stage* stage, float* tetrominoMoveTimer, float* speed, bool* blinking, float* blinkingTimer, int* completedLinesAmount, float* blinkingTick);
void InitTetromino(Tetromino* tetromino);
bool CheckCollision(const Tetromino* tetromino, const Stage stage);
void CopyTetromino(const Tetromino* source, Tetromino* destination);
void ManageRotation(Tetromino* tetromino, const Stage stage);
void ManageHorizontalMovement(Tetromino* tetromino, const Stage stage);
bool ManageTimer(float* moveTimer, float* speed);
void ShiftLineDown(const int startLineY, Stage* stage);
int DeleteLines(Stage* stage);
void MoveTetrominoDown(Tetromino* tetromino, Stage* stage, unsigned long long int* score, float* speed, bool* blinking, int* completedLinesAmount);
void DrawStage(const Stage stage);
void DrawTetromino(const Tetromino tetromino, const Stage stage);
void Input(Tetromino* tetromino, const Stage stage);
void Update(Tetromino* tetromino, Stage* stage, float* tetrominoMoveTimer, unsigned long long int* score, float* speed, Music* mainThem, bool* blinking, float* blinkingTimer, int* completedLinesAmount, float* blinkingTick);
void Draw(const Tetromino tetromino, Stage* stage, unsigned long long int score, const bool blinking, const int completedLinesAmount, float* blinkingTick, const float speed);
unsigned long long int CalculateScore(const int completedLinesAmount);
int InitScore(void);
void DrawScore(const unsigned long long int score);
void InitAudio(Music* mainTheme);
void Exit(Music* mainTheme);
bool ManageBlinkingTimer(float* blinkingTimer, float* speed);
void DrawBlinks(const int completedLinesAmount, Stage* stage, float* blinkingTick, const float speed);
void Blink(int* box, float* blinkingTick, const float speed);