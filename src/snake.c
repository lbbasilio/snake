#include "snake.h"
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 60
#define HEIGHT 60
#define SCREEN WIDTH * HEIGHT

#define GROUND -1
#define WALL -20
#define TARGET -30

int main()
{
	if(Init(WIDTH, HEIGHT)) return 1;

	char* pxChars = malloc (WIDTH * HEIGHT);
	WORD* pxColor = malloc (WIDTH * HEIGHT * sizeof(WORD));
	GameLoop (pxChars, pxColor);

	return 0;
}

int Init (int nScreenWidth, int nScreenHeight)
{
	// Free current console
	if (!FreeConsole())
	{
		printf ("Failed to release parent console\n");
		return 1;
	}

	// Allocate new console window
	if (!AllocConsole())
	{
		printf ("Failed to allocate new console\n)");
		return 1;
	}

	// Get console output handle
	HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);

	// Set font info
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 10;
	cfi.dwFontSize.Y = 10;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = 400;
	SetCurrentConsoleFontEx (hConsole, FALSE, &cfi);

	// Set cursor info
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cci);

	// Set console size
	SMALL_RECT rSize = {0, 0, nScreenWidth - 1, nScreenHeight - 1};
	SetConsoleWindowInfo (hConsole, TRUE, &rSize);

	// Set buffer size
	COORD coordBufferSize = {nScreenWidth, nScreenHeight};
	SetConsoleScreenBufferSize (hConsole, coordBufferSize); 

	// Bind buffer to console
	SetConsoleTitle("Console Engine");
	SetConsoleActiveScreenBuffer (hConsole);

	return 0;
}

void GameLoop (char* pxChars, WORD* pxColors)
{
	srand(time(NULL));

	memset(pxChars, 219, SCREEN);
	memset(pxColors, 0, SCREEN * sizeof(WORD));

	int* game = malloc (SCREEN * sizeof(int)); 

	int nHeadX = WIDTH / 2;
	int nHeadY = HEIGHT / 2;
	int nSize = 5;
	int nCount = nSize;

	int nTgtX = nHeadX + WIDTH / 8;
	int nTgtY = nHeadY + HEIGHT / 8;

	// Initial game state
	int i;
	for (i = 0; i < SCREEN; ++i)
	{
		if (i < WIDTH) game[i] = WALL;
		else if (i > SCREEN - WIDTH - 1) game[i] = WALL;
		else if (i % WIDTH == WIDTH - 1) game[i] = WALL;
		else if (i % WIDTH == 0) game[i] = WALL;
		else game[i] = GROUND;
		
	}
	for (i = 0; i < nSize; ++i)
		game[nHeadX + (nHeadY - i) * WIDTH] = i;

	game[nTgtY * WIDTH + nTgtX] = TARGET;


	while (1)
	{
		// Process Input		
		ProcessInput (game, &nHeadX, &nHeadY);

		// Game logic
		if (nHeadY == nTgtY && nHeadX == nTgtX)
		{
			int available = 0;
			do
			{
				nTgtY = rand() % (HEIGHT - 2);
				nTgtX = rand() % (WIDTH - 2);
				
				
			} while (!available);

			nSize++;
		}

		// Draw
		DrawGame (game, pxChars, pxColors);
	}
}

void ProcessInput (int* game, int* nHeadX, int* nHeadY)
{
	if (GetAsyncKeyState('A') & 0x8000)	*nHeadX = (*nHeadX - 1) % WIDTH;
	else if (GetAsyncKeyState('W') & 0x8000) *nHeadY = (*nHeadY - 1) % HEIGHT;
	else if (GetAsyncKeyState('D') & 0x8000) *nHeadX = (*nHeadX + 1) % WIDTH;
	else if (GetAsyncKeyState('S') & 0x8000) *nHeadY = (*nHeadY + 1) % HEIGHT;
	else return;
	
	// Move Snake
	MoveSnakeRecursive(game, *nHeadX, *nHeadY);	
}

void MoveSnakeRecursive (int* game, int x, int y)
{
	char aux = game[y * WIDTH + x] + 1;
	if (game[y * WIDTH + x - 1] ==  aux) MoveSnakeRecursive(game, x - 1, y);
	else if (game[(y - 1) * WIDTH + x] ==  aux) MoveSnakeRecursive(game, x, y - 1);
	else if (game[(y + 1) * WIDTH + x] ==  aux) MoveSnakeRecursive(game, x, y + 1);
	else if (game[y * WIDTH + x + 1] ==  aux) MoveSnakeRecursive(game, x + 1, y);
	else
	{
		game[y * WIDTH + x] = GROUND;
		return;
	}

	game[y * WIDTH + x]++;
}

void DrawGame (int* game, char* pxChars, WORD* pxColors)
{
	int i;
	for (i = 0; i < SCREEN; ++i)
	{
		switch (game[i])	
		{
			case WALL: pxColors[i] = FOREGROUND_INTENSITY; break;
			case GROUND: pxColors[i] = 0; break;
			case TARGET: pxColors[i] = FOREGROUND_RED | FOREGROUND_GREEN; break;
			case 0: pxColors[i] = FOREGROUND_RED; break;
			default: pxColors[i] = FOREGROUND_GREEN; break;
		}
	}

	HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD cZero = {0, 0};
	DWORD nCharsWritten;
	WriteConsoleOutputCharacter (hConsole, pxChars, SCREEN, cZero, &nCharsWritten);
	WriteConsoleOutputAttribute (hConsole, pxColors, SCREEN, cZero, &nCharsWritten);


}
