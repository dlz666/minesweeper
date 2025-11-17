#pragma once
#include <windows.h>
const int CellSize = 50;
const int CellNum = 9;
const int MineNum = 10;
struct Cell
{
	bool isRevealed;
	bool isMine;
	int adjacentMines;
	bool isFlagged;

};

void InsertFlag(int x, int y);
void InitBoard();
void HandleLeftClick(int x, int y);
void RevealCell(int x, int y);
void DrawBoard(HDC hdc);
bool CheckWin();
