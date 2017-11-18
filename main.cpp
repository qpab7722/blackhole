#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 
#include <time.h> 
#include <conio.h>
#include "info.h" 


#define LEFT 75
#define RIGHT 77
#define SPACE 32
/*
#define bool int
#define true 1
#define false 0
*/

COORD PC_pos = { 10,0 };
COORD MT_pos = { 0,0 };
int curPosX;
int curPosY;
int speed = 30;
int check = 0; // ����ġ�� delete

int PCLife = 30;	//PC�� ü��
bool attacked = false;	//���ݹ޾Ҵ��� �˷��ִ� �Լ�
bool Switch_N = false; //����ġ �浹 �˷��ִ� �Լ�

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
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

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int DetectCollision(int posX, int posY, char MeteoInfo[4][4], char GBInfo_N[25][25])
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY;
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++) {
			if (MeteoInfo[y][x] == 1 || MeteoInfo[y][x] == 2)
			{
				if (GBInfo_N[arrY + y + 1][arrX + (x * 2)] == 1 || GBInfo_N[arrY + y + 1][arrX + (x * 2)] == 2)
					return 0;
			}

		}
	}
	return 1;
}

void ShowMT(char MeteoInfo[4][4]) {
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (MeteoInfo[y][x] == 1)
				printf("��");
			if (MeteoInfo[y][x] == 2)
				printf("��");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteMT(char MeteoInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (MeteoInfo[y][x] == 1 || MeteoInfo[y][x] == 2)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteOb(char GBInfo_N[29][25])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<29; y++)
	{
		for (x = 0; x<25; x++)
		{
			SetCurrentCursorPos( (x * 2), y);
			if (GBInfo_N[y][x]!=0)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}
//PC�� ����� �Լ�
void deletePC(char PCInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCInfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DrawOb(char GBinfo_N[29][25]) {
	int x, y;

	if (Switch_N)
	{
		if (check == 0)
		{
			DeleteOb(GBInfo_N[0]);
			//deletePC(PCInfo[0]);

		}
		check++;
	}
	
	for (y = 0; y<29; y++)
	{
		for (x = 0; x<25; x++)
		{
			if (Switch_N)
			{				
				SetCurrentCursorPos((y*2),x);		
				
			}
			else
			
				SetCurrentCursorPos((x * 2), y);

				if (GBinfo_N[y][x] == 1)
					printf("��");
				if (GBinfo_N[y][x] == 2)
					printf("��");
				if (GBinfo_N[y][x] == 3)
					printf("��");
			
		}
	}
}

int DrawMeteo()
{
	COORD curPos = GetCurrentCursorPos();

	DeleteMT(MeteoInfo[0]);

	if (curPos.Y == 1) {

		return 0;
	}
	if (DetectCollision(curPos.X, curPos.Y + 1, MeteoInfo[0], GBInfo_N[0]) == 0)
	{
		//DeleteOb(GBInfo_N[0]);
		DrawOb(GBInfo_N[0]);
	}

	MT_pos.Y -= 1;
	SetCurrentCursorPos(MT_pos.X, MT_pos.Y);
	ShowMT(MeteoInfo[0]);
	Sleep(50);
	return 1;
}

int Physical(int maxLife)	//ü���Լ�(ĳ������ �ִ� ü���� �޾Ƽ� ���� ü���� ����)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//ü���� 0�϶� game over
	{
		SetCurrentCursorPos(20, curPosY);
		printf("Game Over!\n");
		Sleep(100);
		exit(0);
	}

	else if (attacked)	//��������
		nowLife--;

	attacked = false;	//�ٽ� attacked�� false (������)�� �����ش�.
	return nowLife;	//���� ü���� �����Ѵ�.

}

//PC�� �׸��� �Լ�
void drawPC(char PCInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (PCInfo[y][x] == 1)
			{
				if (y == 1) printf("��");
				if (y == 2)	printf("��");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}



int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_N[29][25], char MeteoInfo[4][4])	//�浹 �Լ�
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;
	if (Switch_N)
	{
		arrX = posY+1;
		arrY = posX/2-1;
	}
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			if (PCInfo[y][x] == 1)
			{

				if (GBInfo_N[arrY + y][arrX + x] == 1)	//���̶� �ε�������
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 2)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}
				if (!Switch_N && GBInfo_N[arrY + y][arrX + x] == 3)
				{
					Switch_N = true;
					deletePC(PCInfo);
					PC_pos.Y = 13;
					MT_pos.X = 28;
					MT_pos.Y = 3;
					return 0;
				}
				/*
				//���׿� ��ġ �ֱ�
				if (MeteoInfo[arrY + y +1][arrX + (x )] !=0 )	//��̶� �ε�������
				{
				attacked = true;

				}
				*/
			}
		}
	}
	return 1;
}

int ShiftRight()
{
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0], GBInfo_N[0], MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	//DeleteOb(GBInfo_N[0]);
	DrawOb(GBInfo_N[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;

}
int ShiftLeft()
{
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_N[0], MeteoInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	//DeleteOb(GBInfo_N[0]);
	DrawOb(GBInfo_N[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;
}

int Jump()
{
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_N[0], MeteoInfo[0]) == 0 || PC_pos.Y == 0)
		return 0;
	deletePC(PCInfo[0]);
	DeleteOb(GBInfo_N[0]);
	DrawOb(GBInfo_N[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);

	return 1;
}

int Gravity_N()
{
	//ª�� �������� ��ĭ�� ������ ��ĭ ������
		if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_N[0], MeteoInfo[0]) == 0)
			return 0;
	deletePC(PCInfo[0]);
	//DeleteOb(GBInfo_N[0]);
	DrawOb(GBInfo_N[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	
	/*
	if (isCrash(curPosX, curPosY + 1, PCInfo[0], GBInfo_N[0]) == 0)
	return 0;
	deletePC(PCInfo[0]);
	DrawOb(GBInfo_N[0]);
	PC_pos.Y += 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(25);
	*/
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


int main(void)
{

	RemoveCursor();
	//DeleteOb(GBInfo_N[0]);
	DrawOb(GBInfo_N[0]);

	MT_pos.Y = 25;
	srand((unsigned int)time(NULL));
	MT_pos.X = (rand() % 5) * 2 + 10;

	while (1)
	{
		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();

		SetCurrentCursorPos(60, 0);
		printf("PC ü��: %3d", Physical(PCLife));
		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (DrawMeteo() == 0) {
			getchar;
			srand((unsigned int)time(NULL));
			MT_pos.X = (rand() % 5) * 2 + 10;
			MT_pos.Y = 25;
		}
	}

	getchar();
	return 0;
}
