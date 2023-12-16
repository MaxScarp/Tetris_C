#include "tetris.h"

int main(int argc, char** argv, char** envs)
{
    Stage stage;
    Tetromino tetromino;

    float tetrominoMoveTimer;
    float speed;

    unsigned long long int score = InitScore();

    Music mainTheme;

    bool blinking;
    float blinkingTimer;
    int completedLinesAmount;
    float blinkingTick;

    InitGame(&stage, &tetrominoMoveTimer, &speed, &blinking, &blinkingTimer, &completedLinesAmount, &blinkingTick);
    InitAudio(&mainTheme);
    InitTetromino(&tetromino);

    while(!WindowShouldClose())
    {
        Input(&tetromino, stage);
        Update(&tetromino, &stage, &tetrominoMoveTimer, &score, &speed, &mainTheme, &blinking, &blinkingTimer, &completedLinesAmount, &blinkingTick);
        Draw(tetromino, &stage, score, blinking, completedLinesAmount, &blinkingTick, speed);
    }

    Exit(&mainTheme);

    return 0;
}

#pragma region "FUNCTIONS"
void InitGame(Stage* stage, float* tetrominoMoveTimer, float* speed, bool* blinking, float* blinkingTimer, int* completedLinesAmount, float* blinkingTick)
{
    const int blocks[] = 
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
    SetTargetFPS(FPS);  

    time_t unixTime;
    time(&unixTime);
    SetRandomSeed(unixTime);

    stage->centerX = (GetScreenWidth() - STAGE_WIDTH * TILE_SIZE) * 0.5f;
    stage->centerY = (GetScreenHeight() - STAGE_HEIGHT * TILE_SIZE) * 0.5f;
    memcpy(stage->blocks, blocks, sizeof(blocks));
    stage->color = colors[MY_GREY];

    *tetrominoMoveTimer = MOVE_TIMER_MAX;
    *speed = INIT_SPEED;

    *blinking = false;
    *blinkingTimer = BLINKING_TIMER_MAX;
    *completedLinesAmount = 0;
    *blinkingTick = BLINKING_TICK_MAX;

    return;
}

void InitTetromino(Tetromino* tetromino)
{
    tetromino->positionX = TETROMINO_START_X;
    tetromino->positionY = TETROMINO_START_Y;
    tetromino->type = GetRandomValue(0, LAST_TYPE - 1);
    tetromino->rotation = DEGREES_0;
    tetromino->color = colors[GetRandomValue(0, LAST_COLOR - 2)]; //-2 to avoid the grey color that is used for the stage
    tetromino->data = &tetrominoDefinition[tetromino->type][tetromino->rotation];

    return;
}

bool CheckCollision(const Tetromino* tetromino, const Stage stage)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int tetrominoOffset = y * TETROMINO_SIZE + x;

            if(tetromino->data->data[tetrominoOffset] != 0)
            {
                const int stageOffset = (y + tetromino->positionY) * STAGE_WIDTH + (x + tetromino->positionX);

                if (stage.blocks[stageOffset] != 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void CopyTetromino(const Tetromino* source, Tetromino* destination)
{
    memcpy(destination, source, sizeof(Tetromino));
    destination->data = source->data;
}

void ManageRotation(Tetromino* tetromino, const Stage stage)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        Tetromino tempTetromino;
        CopyTetromino(tetromino, &tempTetromino);

        tempTetromino.rotation = (tempTetromino.rotation + 1) % LAST_ROTATION;
        tempTetromino.data = &tetrominoDefinition[tempTetromino.type][tempTetromino.rotation];

        if(!CheckCollision(&tempTetromino, stage))
        {
            CopyTetromino(&tempTetromino, tetromino);
        }
    }

    return;
}

void ManageHorizontalMovement(Tetromino* tetromino, const Stage stage)
{
    if(IsKeyPressed(KEY_RIGHT))
    {
        tetromino->positionX += 1;

        if(CheckCollision(tetromino, stage))
        {
            tetromino->positionX -= 1;
            return;
        }
    }
    else if(IsKeyPressed(KEY_LEFT))
    {
        tetromino->positionX -= 1;

        if(CheckCollision(tetromino, stage))
        {
            tetromino->positionX += 1;
            return;
        }
    }

    return;
}

bool ManageTimer(float* moveTimer, float* speed)
{
    *moveTimer -= GetFrameTime() * (*speed);
    if(*moveTimer <= 0.0f || IsKeyPressed(KEY_DOWN))
    {
        *moveTimer = MOVE_TIMER_MAX;
        return true;
    }

    return false;
}

void ShiftLineDown(const int startLineY, Stage* stage)
{
    for(int y = startLineY; y >= 0; y--)
    {
        for(int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int stageOffset = y * STAGE_WIDTH + x;

            if(stage->blocks[stageOffset] != 1)
            {
                continue;
            }

            const int stageOffsetBelow = (y + 1) * STAGE_WIDTH + x;

            if(stage->blocks[stageOffsetBelow] != 0)
            {
                continue;
            }

            stage->blocks[stageOffsetBelow] = stage->blocks[stageOffset];
            stage->blocks[stageOffset] = 0;
        }
    }

    return;
}

int DeleteLines(Stage* stage)
{
    int completedLinesAmount = 0;

    for(int y = 0; y < STAGE_HEIGHT - 1; y++)
    {
        bool isLineComplete = true;

        for(int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int stageOffset = y * STAGE_WIDTH + x;

            if(stage->blocks[stageOffset] != 1)
            {
                isLineComplete = false;
                break;
            }
        }

        if(isLineComplete)
        {
            const int stageOffset = y * STAGE_WIDTH + 1;
            memset(stage->blocks + stageOffset, 0, (STAGE_WIDTH - 2) * sizeof(int));

            ShiftLineDown(y, stage);

            completedLinesAmount++;
        }
    }

    return completedLinesAmount;
}

void MoveTetrominoDown(Tetromino* tetromino, Stage* stage, unsigned long long int* score, float* speed, bool* blinking, int* completedLinesAmount)
{
    tetromino->positionY += 1;

    if(CheckCollision(tetromino, *stage))
    {
        tetromino->positionY -= 1;
        for(int y = 0; y < TETROMINO_SIZE; y++)
        {
            for(int x = 0; x < TETROMINO_SIZE; x++)
            {
                const int tetrominoOffset = y * TETROMINO_SIZE + x;
                
                if(tetromino->data->data[tetrominoOffset] != 0)
                {
                    const int stageOffset = (y + tetromino->positionY) * STAGE_WIDTH + (x + tetromino->positionX);

                    stage->blocks[stageOffset] = 1;
                }
            }
        }

        *completedLinesAmount = DeleteLines(stage);
        if(*completedLinesAmount > 0)
        {
            *blinking = true;

            *score += CalculateScore(*completedLinesAmount);
            if(*score % 500 == 0)
            {
                *speed *= SPEED_MULTIPLIER;
            }
        }

        InitTetromino(tetromino);
    }
    
    return;
}

void DrawStage(const Stage stage)
{
    for(int y = 0; y < STAGE_HEIGHT; y++)
    {
        for(int x = 0; x < STAGE_WIDTH; x++)
        {
            const int stageOffset = y * STAGE_WIDTH + x;

            if(stage.blocks[stageOffset] != 0)
            {
                DrawRectangle(x * TILE_SIZE + stage.centerX, y * TILE_SIZE + stage.centerY, TILE_SIZE, TILE_SIZE, stage.color);
            }

            DrawRectangleLines(x * TILE_SIZE + stage.centerX, y * TILE_SIZE + stage.centerY, TILE_SIZE, TILE_SIZE, WHITE);
        }
    }

    return;
}

void DrawTetromino(const Tetromino tetromino, const Stage stage)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int tetrominoOffset = y * TETROMINO_SIZE + x;

            if(tetromino.data->data[tetrominoOffset] != 0)
            {
                DrawRectangle((x + tetromino.positionX) * TILE_SIZE + stage.centerX, (y + tetromino.positionY) * TILE_SIZE + stage.centerY, TILE_SIZE, TILE_SIZE, tetromino.color);
            }
        }
    }

    return;
}

void Input(Tetromino* tetromino, const Stage stage)
{
    ManageRotation(tetromino, stage);
    ManageHorizontalMovement(tetromino, stage);

    return;
}

void Update(Tetromino* tetromino, Stage* stage, float* tetrominoMoveTimer, unsigned long long int* score, float* speed, Music* mainTheme, bool* blinking, float* blinkingTimer, int* completedLinesAmount, float* blinkingTick)
{
    UpdateMusicStream(*mainTheme);
    if(ManageTimer(tetrominoMoveTimer, speed))
    {
        MoveTetrominoDown(tetromino, stage, score, speed, blinking, completedLinesAmount);
    }

    if(*blinking)
    {
        *blinking = ManageBlinkingTimer(blinkingTimer, speed);
    }
    else
    {
        *blinkingTick = BLINKING_TICK_MAX;
    }

    return;
}

void Draw(const Tetromino tetromino, Stage* stage, const unsigned long long int score, const bool blinking, const int completedLinesAmount, float* blinkingTick, const float speed)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawStage(*stage);
    DrawTetromino(tetromino, *stage);
    DrawScore(score);

    if(blinking)
    {
        DrawBlinks(completedLinesAmount, stage, blinkingTick, speed);
    }

    EndDrawing();

    return;
}

unsigned long long int CalculateScore(const int completedLinesAmount)
{
    unsigned long long int score = 0;

    switch(completedLinesAmount)
    {
        case 1:
            score += 100;
            break;
        case 2:
            score += 800;
            break;
        case 3:
            score += 2700;
            break;
        case 4:
            score += 6400;
            break;
        default:
            TraceLog(LOG_ERROR, "%d completed lines is not possible, something went wrong!", completedLinesAmount);
            break;
    }

    return score;
}

int InitScore(void)
{
    return 0;
}

void DrawScore(const unsigned long long int score)
{
    DrawText(TextFormat("Score: %llu", score), 25, 25, 40, WHITE);

    return;
}

void InitAudio(Music* mainTheme)
{
    InitAudioDevice();

    if(!IsAudioDeviceReady())
    {
        TraceLog(LOG_ERROR, "Audio device cannot be initialized!");
        Exit(mainTheme);
    }

    *mainTheme = LoadMusicStream(MAIN_THEME_PATH);
    if(!IsMusicReady(*mainTheme))
    {
        TraceLog(LOG_ERROR, "Music cannot be initialized!");
        Exit(mainTheme);
    }

    mainTheme->looping = true;

    PlayMusicStream(*mainTheme);

    return;
}

void Exit(Music* mainTheme)
{
    UnloadMusicStream(*mainTheme);
    CloseAudioDevice();
    CloseWindow();

    return;
}

bool ManageBlinkingTimer(float* blinkingTimer, float* speed)
{
    *blinkingTimer -= GetFrameTime() * (*speed);
    if(*blinkingTimer <= 0.0f)
    {
        *blinkingTimer = BLINKING_TIMER_MAX;
        return false;
    }

    return true;
}

void DrawBlinks(const int completedLinesAmount, Stage* stage, float* blinkingTick, const float speed)
{
    /*for(int y = STAGE_HEIGHT - 1; y >= 0; y--)
    {
        for(int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            int stageOffset = y * STAGE_WIDTH + x;

            Blink(&stage->blocks[stageOffset], blinkingTick, speed);
        }
    }*/

    return;
}

void Blink(int* box, float* blinkingTick, const float speed)
{
    *blinkingTick -= GetFrameTime() * speed;
    if(*blinkingTick <= 0.0f)
    {
        *blinkingTick = BLINKING_TICK_MAX;
        *box = !(*box);
    }

    return;
}
#pragma endregion