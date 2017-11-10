
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h> 
#include "info.h"

#define LEFT 75
#define RIGHT 77
#define SPACE 32

COORD PC_pos = {12,0};
int curPosX ;
int curPosY ;
int speed = 30;

int PCLife = 30;


bool attacked = false;	//공격받았는지 알려주는 함수

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}


int isCrach(int posX, int posY, char PCinfo[4][4], char ObInfo[25][25])
{
	int x,y;
	int arrX=(posX)/2;
	int arrY=posY;
	for(x=0; x<4; x++)
	{
		for(y=0; y<4; y++)
		{
			if(PCinfo[y][x]==1)
			{
				if(ObInfo[arrY + y][arrX + x] == 1 )
					return 0;

				if(ObInfo[arrY + y][arrX + x] == 2 )
				{
					attacked= true;
					return 0;
				}
			}
		}
	}
	return 1;
}

void drawPC(char PCinfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();	
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCinfo[y][x] == 1)
			{
				if (y == 1) printf("◎");
				if (y == 2)	printf("▲");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deletePC(char PCinfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCinfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

int ShiftRight()
{
	if(isCrach(curPosX+2, curPosY, PCInfo[0],ObInfo[0])==0 )
	{
		return 0;
	}

	deletePC(PCInfo[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;
	
}
int ShiftLeft()
{	
	if(isCrach(curPosX-2, curPosY, PCInfo[0],ObInfo[0])==0)
	{
		return 0;
	}

	deletePC(PCInfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;	
}

int Jump()
{
	if(isCrach(curPosX, curPosY-1, PCInfo[0],ObInfo[0])==0)
	{
		return 0;
	}

	deletePC(PCInfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	
	return 1;
}

int Gravity_N()
{

	if(isCrach(curPosX, curPosY+1, PCInfo[0],ObInfo[0])==0)
	{
		return 0;
	}
	deletePC(PCInfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(25);
	/*deletePC(PCInfo[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(25);*/					//여기가 따닥따닥 (엄청 빨리 되서) 어찌해야 할지 모르겟음


	return 1;
}

void ProcessKeyInput()
{
	int key;
	
	for (int i = 0;i<20;i++)
	{		
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
				case LEFT:
					ShiftLeft();
					break;
				case RIGHT:
					ShiftRight();
					break;
				case SPACE:					
					Jump();
					break;
			}
		}		
		Sleep(speed);
	}	
}


//민수
void DrawOb(char ObInfo[25][25]) {
	int x, y;

	for (y = 0; y<25; y++)
	{
		for (x = 0; x<25; x++)
		{
			SetCurrentCursorPos((x * 2), y);
			if (ObInfo[y][x] == 1)
				printf("□");
			if (ObInfo[y][x] == 2)
				printf("△");
		}
	}
}



int Physical(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if(nowLife == 0)
	{
		SetCurrentCursorPos(20, curPosY);
		printf("Game Over!\n");
		Sleep(100);
		exit(0);
	}

	 else if (attacked)
		 nowLife--;

	 attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}




void main()
{
	RemoveCursor();
	
	DrawOb(ObInfo[0]);

	while (1)
	{		
		SetCurrentCursorPos(60, curPosY-1);
		printf("                 ");
		SetCurrentCursorPos(60, curPosY);
		printf("PC 체력: %3d",Physical(PCLife));

		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();		
	}

	getchar();
}