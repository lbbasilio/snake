#ifndef SNAKE_H
#define SNAKE_H

#include <Windows.h>

int Init (int nScreenWidth, int nScreenHeight);
void GameLoop (char* pxChars, WORD* pxColors);
void ProcessInput (int* game, int* nHeadX, int* nHeadY);
void MoveSnakeRecursive (int* game, int x, int y);
void DrawGame (int* game, char* pxChars, WORD* pxColors);

#endif
