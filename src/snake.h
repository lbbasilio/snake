#ifndef SNAKE_H
#define SNAKE_H

#include <Windows.h>

typedef enum {UP, DOWN, LEFT, RIGHT, NONE, QUIT} Direction;

int Init (int nScreenWidth, int nScreenHeight);

Direction ReadInput ();
void GameInit (int* game);
void GameLoop (char* pxChars, WORD* pxColors);

void MoveSnakeRecursive (int* game, int x, int y, int nSize);

void DrawGame (int* game, char* pxChars, WORD* pxColors);

#endif
