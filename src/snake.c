#include "snake.h"
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Colors
// format: 0x00BBGGRR (thanks microsoft u_u)
#define CBODY 0x0000FF00
#define CHEAD 0x004000FF
#define CWALL 0x00888888
#define CTGT  0x001966FF
#define CGND  0x00000000

// Board Size
#define PIXEL 20
#define WIDTH 30
#define HEIGHT 30
#define SCREEN WIDTH * HEIGHT

// Board Values
#define GROUND -1
#define WALL -20
#define TARGET -30

// Speed
#define FPS 15.0f

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
	cfi.dwFontSize.X = PIXEL;
	cfi.dwFontSize.Y = PIXEL;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = 400;
	SetCurrentConsoleFontEx (hConsole, FALSE, &cfi);

	// Set color info
	CONSOLE_SCREEN_BUFFER_INFOEX csbi;
	csbi.cbSize = sizeof (CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx (hConsole, &csbi);
	csbi.ColorTable[0] = CBODY;
	csbi.ColorTable[1] = CHEAD;
	csbi.ColorTable[2] = CWALL;
	csbi.ColorTable[3] = CTGT;
	csbi.ColorTable[4] = CGND;
	SetConsoleScreenBufferInfoEx (hConsole, &csbi);

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
	SetConsoleTitle("Snake");
	SetConsoleActiveScreenBuffer (hConsole);

	return 0;
}

void GameInit (int* game)
{
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
}

void GameLoop (char* pxChars, WORD* pxColors)
{
	// Initiate rng
	srand(time(NULL));

	// Set character buffers
	memset(pxChars, 219, SCREEN);
	memset(pxColors, 0, SCREEN * sizeof(WORD));

	// Initiate board
	int* game = malloc (SCREEN * sizeof(int)); 
	GameInit (game);

	// Snake variables
	int nHeadX = WIDTH / 2;
	int nHeadY = HEIGHT / 2;
	int nSize = 5;
	
	// Insert Snake
	int i;
	for (i = 0; i < nSize; ++i)
		game[nHeadX + (nHeadY - i) * WIDTH] = i;

	// Target variables
	int nTgtX = nHeadX + WIDTH / 8;
	int nTgtY = nHeadY + HEIGHT / 8;

	// Insert Target
	game[nTgtY * WIDTH + nTgtX] = TARGET;

	// Timing variables
	LARGE_INTEGER startTime, endTime, uSecs;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency (&frequency);
	QueryPerformanceCounter (&startTime);

	// Input variables
	Direction current = NONE;
	Direction previous = DOWN;

	while (1)
	{
		// Read Input		
		current = ReadInput ();
		if (current == QUIT) break;

		// Game logic
		if (current == NONE) current = previous;
		switch (current)
		{
			case UP:	if (previous == DOWN)	current = previous; break;	
			case LEFT:	if (previous == RIGHT) 	current = previous; break;	
			case DOWN:	if (previous == UP) 	current = previous; break;	
			case RIGHT:	if (previous == LEFT) 	current = previous; break;	
			default: break;
		}

		switch (current)
		{
			case UP:	nHeadY--; break;
			case LEFT:	nHeadX--; break;
			case DOWN:	nHeadY++; break;
			case RIGHT: nHeadX++; break;
			default: break;
		}
		previous = current;

		// Collision Check
		int nextSpace = game[nHeadY * WIDTH + nHeadX];
		if (nextSpace != GROUND && nextSpace != TARGET) break; // Collided with wall or itself
		else if (nextSpace == TARGET)
		{	
			int available = 0;
			do
			{
				// Generate new random target position
				nTgtY = (rand() % (HEIGHT - 2)) + 1;
				nTgtX = (rand() % (WIDTH  - 2)) + 1;

				// Check if spot is available
				if (game[nTgtY * WIDTH + nTgtX] ==  GROUND)
				{
					game[nTgtY * WIDTH + nTgtX] = TARGET;
					available = 1;
				}		

			} while (!available);

			game[nHeadY * WIDTH + nHeadX] = GROUND;
			nSize++;
		}
		MoveSnakeRecursive (game, nHeadX, nHeadY, nSize);

		// Draw
		DrawGame (game, pxChars, pxColors);

		// Timing
		uSecs.QuadPart = 0;
		while (uSecs.QuadPart < 1000000 / FPS) 
		{
			QueryPerformanceCounter (&endTime);
			uSecs.QuadPart = endTime.QuadPart - startTime.QuadPart;
			uSecs.QuadPart *= 1000000;
			uSecs.QuadPart /= frequency.QuadPart;
		}
		startTime.QuadPart = endTime.QuadPart;
	}
}

Direction ReadInput ()
{
	if (GetAsyncKeyState('A') & 0x8000)	return LEFT;
	else if (GetAsyncKeyState('W') & 0x8000) return UP;
	else if (GetAsyncKeyState('D') & 0x8000) return RIGHT;
	else if (GetAsyncKeyState('S') & 0x8000) return DOWN;
	else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return QUIT;
	else return NONE;
}

void MoveSnakeRecursive (int* game, int x, int y, int nSize)
{
	char aux = game[y * WIDTH + x] + 1;
	if (game[y * WIDTH + x - 1] ==  aux) MoveSnakeRecursive(game, x - 1, y, nSize);
	else if (game[(y - 1) * WIDTH + x] ==  aux) MoveSnakeRecursive(game, x, y - 1, nSize);
	else if (game[(y + 1) * WIDTH + x] ==  aux) MoveSnakeRecursive(game, x, y + 1, nSize);
	else if (game[y * WIDTH + x + 1] ==  aux) MoveSnakeRecursive(game, x + 1, y, nSize);
	else
	{
		if (nSize == aux + 1) game[y * WIDTH + x] = aux;
		else game[y * WIDTH + x] = GROUND;
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
			case WALL: pxColors[i] = 0x2; break;
			case GROUND: pxColors[i] = 0x4; break;
			case TARGET: pxColors[i] = 0x3; break;
			case 0: pxColors[i] = 0x1; break;
			default: pxColors[i] = 0x0; break;
		}
	}

	HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD cZero = {0, 0};
	DWORD nCharsWritten;
	WriteConsoleOutputCharacter (hConsole, pxChars, SCREEN, cZero, &nCharsWritten);
	WriteConsoleOutputAttribute (hConsole, pxColors, SCREEN, cZero, &nCharsWritten);
}
