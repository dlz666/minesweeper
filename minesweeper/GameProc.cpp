#include "declare.h"
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

extern Cell board[CellNum][CellNum];

// Step 1: Initialize the board
void InitBoard()
{
	for (int i = 0; i < CellNum; i++)
	{
		for(int j = 0; j < CellNum; j++)
		{
			board[i][j].isRevealed = false;
			board[i][j].isMine = false;
			board[i][j].adjacentMines = 0;
			board[i][j].isFlagged = false;
		}
	}
	srand((unsigned int)time(NULL));
	// Use random number generator to place mines. From 0 to CellNum*CellNum - 1.
	for (int i = 0; i < MineNum; )
	{
		int r = rand() % (CellNum * CellNum);
		int x = r / CellNum;
		int y = r % CellNum;
		if(!board[x][y].isMine)
		{
			board[x][y].isMine = true;
			i++;
		}
	}
	for (int i = 0; i < CellNum; i++)
	{
		for(int j = 0; j < CellNum; j++)
		{
			if(board[i][j].isMine)
				continue;
			int count = 0;
			for(int dx = -1; dx <= 1; dx++)
			{
				for(int dy = -1; dy <= 1; dy++)
				{
					if(dx == 0 && dy == 0)
						continue;
					int nx = i + dx;
					int ny = j + dy;
					if(nx >= 0 && nx < CellNum && ny >= 0 && ny < CellNum)
					{
						if(board[nx][ny].isMine)
							count++;
					}
				}
			}
			board[i][j].adjacentMines = count;
		}
	}
}

// Step 2: deal with the click event

// Reveal
void RevealCell(int x, int y)
{
	if(x < 0 || x >= CellNum || y < 0 || y >= CellNum)
		return;
	if(board[x][y].isRevealed)
		return;
	board[x][y].isRevealed = true;
	
	if(board[x][y].adjacentMines == 0 && !board[x][y].isMine)
	{
		for(int dx = -1; dx <= 1; dx++)
		{
			for(int dy = -1; dy <= 1; dy++)
			{
				if (dx == 0 && dy == 0) 
					continue;
				RevealCell(x + dx, y + dy);
			}
		}
	}
}

// Handle left click
void HandleLeftClick(int x, int y)
{
	int cellX = x / CellSize;
	int cellY = y / CellSize;
	if(cellX < 0 || cellX >= CellNum || cellY < 0 || cellY >= CellNum)
		return;
	if (board[cellX][cellY].isMine)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(NULL, L"Game Over! You clicked on a mine.", L"Minesweeper", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	RevealCell(cellX, cellY);
}

// Handle right click (Insert Flag)
void InsertFlag(int x, int y)
{
	if (x < 0 || x >= CellNum * CellSize || y < 0 || y >= CellNum * CellSize)
		return;
	int cellx = x / CellSize;
	int celly = y / CellSize;
	if (board[cellx][celly].isRevealed == true) return;
	else
	{
		board[cellx][celly].isFlagged = !board[cellx][celly].isFlagged;
	}

}
// Step 3: Draw the board
void DrawBoard(HDC hdc)
{
	for (int i = 0; i < CellNum; i++)
	{
		for (int j = 0; j < CellNum; j++)
		{
			RECT cellRect = { i * CellSize, j * CellSize, (i + 1) * CellSize, (j + 1) * CellSize };
			if (board[i][j].isRevealed)
			{
				FillRect(hdc, &cellRect, (HBRUSH)(COLOR_3DFACE + 1));
				DrawEdge(hdc, &cellRect, EDGE_RAISED, BF_RECT);
				//if (board[i][j].isMine)
				//{
				//	// Draw mine
				//	Ellipse(hdc, i * CellSize + 10, j * CellSize + 10, (i + 1) * CellSize - 10, (j + 1) * CellSize - 10);
				//}
				if (board[i][j].adjacentMines > 0)
				{
					// Draw number
					wchar_t numStr[2];
					swprintf_s(numStr, L"%d", board[i][j].adjacentMines);
					TextOut(hdc, i * CellSize + CellSize / 2 - 5, j * CellSize + CellSize / 2 - 10, numStr, 1);
				}
			}
			else if(!board[i][j].isFlagged)
			{
				FillRect(hdc, &cellRect, (HBRUSH)(COLOR_3DFACE + 3));
				DrawEdge(hdc, &cellRect, EDGE_RAISED, BF_RECT);
			}
			else
			{
				// Draw the unrevealed button background first
				FillRect(hdc, &cellRect, (HBRUSH)(COLOR_3DFACE + 3));
				DrawEdge(hdc, &cellRect, EDGE_RAISED, BF_RECT);

				// --- FLAG DRAWING START ---

				// 1. Define the center coordinates of the current cell
				int centerX = i * CellSize + CellSize / 2;
				int centerY = j * CellSize + CellSize / 2;

				// 2. Define the coordinates for the flag shape (a triangle)
				POINT flagPoints[3];
				// Triangle vertices are calculated relative to centerX and centerY
				flagPoints[0] = { centerX + 5, centerY - 10 }; // Top point
				flagPoints[1] = { centerX + 5, centerY };      // Bottom right point
				flagPoints[2] = { centerX - 5, centerY - 5 };  // Left point

				// ... rest of the GDI code to draw the flag cloth and pole ...

				HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hRedBrush);

				Polygon(hdc, flagPoints, 3);

				HPEN hBlackPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
				HPEN hOldPen = (HPEN)SelectObject(hdc, hBlackPen);

				MoveToEx(hdc, centerX + 5, centerY + 10, NULL);
				LineTo(hdc, centerX + 5, centerY - 10);

				// 3. Cleanup GDI objects
				SelectObject(hdc, hOldBrush);
				DeleteObject(hRedBrush);
				SelectObject(hdc, hOldPen);
				DeleteObject(hBlackPen);

				// --- FLAG DRAWING END ---
			}
		}
	}
}

// Step 4: Check Win Condition
bool CheckWin()
{
	for (int i = 0; i < CellNum; i++)
	{
		for (int j = 0; j < CellNum; j++)
		{
			if (!board[i][j].isMine && !board[i][j].isRevealed)
				return false;
		}
	}
	return true;
}