#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 
#include <time.h> 
#include <conio.h>
#include "info.h" 


#define LEFT 75
#define RIGHT 77
#define SPACE 32

#define GBOARD_HEIGHT  29
#define GBOARD_WIDTH  25

//BOSS MAP length
#define B_GBOARD_HEIGHT  30
#define B_GBOARD_WIDTH  30

COORD PC_pos = { 10,0 };
COORD MT_pos = { 0,0 };
COORD Boss_pos = { 30,10 };
COORD Mirr_pos[4] = { 0 };//반사경 위치
COORD Switch_pos[4] = { 0 };//스위치 위치

int curPosX;
int curPosY;
int speed = 30;
int check = 0; // 스위치후 delete

int PCLife = 30;	//PC의 체력
bool attacked = false;	//공격받았는지 알려주는 함수
bool Switch_N = false; //스위치 충돌 알려주는 함수

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];
int ObTime;	//돌출된 지형 만들어지는 X좌표
int Check_Ob = 0;	//올라가는 간격	(장애물과 장애물사이 간격)
int Ran;	//돌출된 지형 길이

bool changeMap_Boss = false;	//bool B_OK = true;//보스맵 전환 신호(임시)
int checkStage = 1;	//현재 스테이지
int count = 0;	//레이저 커서 증가

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

int DetectCollision(int posX, int posY, char MeteoInfo[4][4])	//맵 랜덤으로 나오면 손 봐야 할듯
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
				printf("◈");
			if (MeteoInfo[y][x] == 2)
				printf("˚");
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

void DeleteOb()
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<29; y++)
	{
		for (x = 0; x<25; x++)
		{
			SetCurrentCursorPos((x * 2), y);
			if (GBInfo_N[y][x] != 0)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//PC를 지우는 함수
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

void DrawOb()	//돌출 지형을 그리는 함수
{
	int x, y;

	if (Switch_N)
	{
		if (check == 0)
		{
			DeleteOb();
		}
		check++;
	}

	for (y = 0; y<29; y++)
	{
		for (x = 0; x<25; x++)
		{
			if (Switch_N)
			{
				SetCurrentCursorPos((y * 2), x);
			}
			else
				SetCurrentCursorPos((x * 2), y);

			if (GBInfo_N[y][x] == 1)
				printf("■");
			if (GBInfo_N[y][x] == 2)
				printf("▲");
			if (GBInfo_N[y][x] == 3)
				printf("★");
			else
				printf("　");

		}
	}
}

void UpOB()	//돌출 지형을 일정 간격마다 위로 올려주는 함수 
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH * sizeof(int));//내장함수,복사할 떄 많이들 사용
	}
}

void MakeOb()	//돌출 지형을 GBInfo_N에 생성해주는 함수 
{
	srand((unsigned int)time(NULL));
	ObTime = (rand() % 6) * 2;
	Ran = (rand() % 8) * 2;

	for (int x = ObTime; x < ObTime + Ran; x++)
		GBInfo_N[28][x] = 1;
}

int DrawMeteo()
{
	COORD curPos = GetCurrentCursorPos();

	DeleteMT(MeteoInfo[0]);

	if (curPos.Y == 1) {

		return 0;
	}
	if (DetectCollision(curPos.X, curPos.Y + 1, MeteoInfo[0]) == 0)
	{
		//DeleteOb(GBInfo_N[0]);
		DrawOb();
	}

	MT_pos.Y -= 1;
	SetCurrentCursorPos(MT_pos.X, MT_pos.Y);
	ShowMT(MeteoInfo[0]);
	Sleep(50);
	return 1;
}

int Physical(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		SetCurrentCursorPos(20, curPosY);
		printf("Game Over!\n");
		Sleep(100);
		getchar();
		exit(0);
	}

	else if (attacked)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

//PC를 그리는 함수
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
				if (y == 1) printf("◎");
				if (y == 2)	printf("△");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DrawBoss(char BossInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] == 1)
				printf("Π");
			if (BossInfo[y][x] == 2)
				printf("<");
			if (BossInfo[y][x] == 3)
				printf(">");
			if (BossInfo[y][x] == 4)
				printf("Θ");
			if (BossInfo[y][x] == 5)
				printf("▼");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteBoss(char BossInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] != 0)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void ShootLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
				printf("*");
		}

		Sleep(100);
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);

}

void MoveLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
				printf("*");
		}

	}
	SetCurrentCursorPos(curPos.X, curPos.Y);



}



int isCrash(int posX, int posY, char PCInfo[4][4])	//충돌 함수
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;

	if (Switch_N)
	{
		arrX = posY + 1;
		arrY = posX / 2 - 1;
	}
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			///일반맵
			if (PCInfo[y][x] == 1 && Switch_N == false)
			{

				if (GBInfo_N[arrY + y][arrX + x] == 1)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 2)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}
				if (GBInfo_N[arrY + y][arrX + x] == 3)	//일반맵 스위치
				{
					Switch_N = true;
					deletePC(PCInfo);
					PC_pos.Y = 13;
					MT_pos.X = 28;
					MT_pos.Y = 3;
					return 0;
				}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//운석 충돌 (서로 뚫고 지나감)
				{
					attacked = true;

				}

			}

			///전환맵
			if (PCInfo[x][y] == 1 && Switch_N == true)
			{
				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 1)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 2)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}

				//if (GBInfo_N[arrY + y][arrX + x] == 3)	//전환맵 스위치
				//{
				//	Switch_N = true;
				//	deletePC(PCInfo);
				//	PC_pos.Y = 13;
				//	MT_pos.X = 28;
				//	MT_pos.Y = 3;
				//	return 0;
				//}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//운석 충돌 (서로 뚫고 지나감)
				{
					attacked = true;
					return 0;
				}

			}

		}
	}
	return 1;
}

int ShiftRight()
{
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0]) == 0)
	{
		if (Switch_N == true)
		{
			PC_pos.X -= 2;
			Sleep(50);
		}
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
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0]) == 0)
		return 0;
	deletePC(PCInfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);
	return 1;
}

int Jump()
{
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0]) == 0 || PC_pos.Y == 0)
		return 0;
	deletePC(PCInfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(50);

	return 1;
}

int Gravity_N()
{
	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == false)	//일반맵 올라오는 벽
	{
		PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == true)	//전환맵 아래 벽
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == true)	//전환맵 옆으로 다가오는 벽
	{
		PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	deletePC(PCInfo[0]);
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

void isN_clear()//클리어(일반)
{
	changeMap_Boss = true;

	printf("일반맵 클리어");	//일단 출력
	Sleep(1000);
}

void isB_clear()//클리어(보스)
{
	//checkStage: 현재 스테이지

	if (checkStage == 5)
	{ 
		printf("Game Clear!");
		//bool Ranking = true;
	}
		
	else
	{
		printf("Next Stage");
		Sleep(1000);

		checkStage++;

		//bool stage_2 = true;	다음 스테이지로! 연결
	}
}


void ProcessKeyInput()
{
	int key;

	for (int i = 0; i<20; i++)
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
			case '1':	//추가
				Switch_N = true;
				break;
			}
		}
		Sleep(speed);
	}
}


int main(void)
{
	srand((unsigned int)time(NULL));
	int L = (rand() % 4) + 1;

	RemoveCursor();
	//DeleteOb(GBInfo_N[0]);
	DrawOb();

	MT_pos.Y = 25;
	MT_pos.X = (rand() % 5) * 2 + 10;

	while (1)
	{
		for (int y = 0; y < GBOARD_HEIGHT; y++)
		{
			GBInfo_N[y][0] = 1;
			GBInfo_N[y][GBOARD_WIDTH - 1] = 1;
		}

		UpOB();
		//Sleep(50);
		DrawOb();
		Check_Ob++;

		curPosX = PC_pos.X;
		curPosY = PC_pos.Y;
		SetCurrentCursorPos(curPosX, curPosY);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();

		SetCurrentCursorPos(60, 1);
		printf("PC 체력: %3d", Physical(PCLife));

		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (DrawMeteo() == 0) {
			getchar;
			MT_pos.X = (rand() % 5) * 2 + 10;
			MT_pos.Y = 25;
		}

		if (Check_Ob % 6 == 0)
			MakeOb();

		if (Check_Ob == 10)
		{
			GBInfo_N[10][3] = 3;
		}

		if (Check_Ob == 30)	//클리어(일반)
			isN_clear();

		if (Check_Ob == 5)	//임의로 보스 불러옴
		{
			curPosX = Boss_pos.X;
			curPosY = Boss_pos.Y;
			SetCurrentCursorPos(curPosX, curPosY);
			DrawBoss(BossInfo[0]);	//보스 그림

			SetCurrentCursorPos(curPosX, curPosY + 4 );
			ShootLaser_B(LaserInfo[L]);	//보스 레이저

			

			while(1)
			{
				count++;
				SetCurrentCursorPos(curPosX, curPosY + 4 + count);
				MoveLaser_B(LaserInfo[L]);
				Sleep(100);

				if (curPosY + 4 + count > 28)
					break;
			}
			

			Sleep(3000);

		}


	}

	getchar();
	return 0;
}