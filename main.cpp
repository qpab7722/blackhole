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
COORD Mirr_pos[4] = { 0 };//반사경 위치
COORD Switch_pos[4] = { 0 };//스위치 위치
COORD Boss_pos = { 0,0 };

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];


int speed = 30;
int PCLife = 30;	//PC의 체력
int ObTime;	//돌출된 지형 만들어지는 X좌표
int Check_Ob = 0;	//올라가는 간격	(장애물과 장애물사이 간격)
int Ran;	//돌출된 지형 길이
int check_N;//전환시간 검사
int starttime = 0;//전환 시작 시가

bool attacked = false;	//공격받았는지 알려주는 함수
bool Switch_N = false; //스위치 충돌 알려주는 함수
bool B_OK=false;//보스맵 전환 신호(임시)


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
			if (Switch_N)
				SetCurrentCursorPos(curPos.Y + (y * 2), curPos.X + x);
			else
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
			if (Switch_N)
				SetCurrentCursorPos(curPos.Y + (y * 2), curPos.X + x);
			else
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

//반사경을 그리는 함수
void drawMirr(char MirrInfo[2][2])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	
	for (y = 0; y<2; y++)	
		for (x = 0; x<2; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
			if (MirrInfo[y][x] == 1)
				printf("＠");			
		}	
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//스위치를 그리는 함수
void drawSwitch(char SwitchInfo[2][2])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<2; y++)
		for (x = 0; x<2; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			if (SwitchInfo[y][x] == 1)
				printf("☎");
		}
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//Boss를 그리는 함수
void drawBoss(char BossInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<4; y++)
	{
		for (x = 0; x<4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] == 1)
			{
				if (y == 0) printf("◐");
				if (y == 1)	printf("※");
				else  printf("｜");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//보스맵을 그리는 함수
void drawGB_B(char GBInfo_B[30][30])
{
	int x, y, mn=0,sn=0;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0;y<B_GBOARD_HEIGHT;y++)
		for (x = 0;x < B_GBOARD_WIDTH;x++)
		{
			SetCurrentCursorPos((x * 2), y);

			//벽그리기
			if (GBInfo_B[y][x] == 1)
			{
				if (y == 0) //위벽
				{
					if (x == 0) { printf("┌"); continue; }
					if (x == B_GBOARD_WIDTH - 1) { printf("┐"); continue; }
					printf("─");
				}
				if (y == B_GBOARD_HEIGHT - 1)//아래벽
				{
					if (x == 0) { printf("└"); continue; }
					if (x == B_GBOARD_WIDTH - 1) { printf("┘"); continue; }
					printf("─");
				}
				if (x == 0 )//왼쪽벽
					printf("│");
				
				if (x == B_GBOARD_WIDTH - 1)//오른벽
					printf("│");			
			}

			//반사경그리기
			if (GBInfo_B[y][x] == 'm')
			{	
				if(x == 1 || x == B_GBOARD_WIDTH - 2)//세로
					drawMirr(MirrInfo[1]);
				
				else
					drawMirr(MirrInfo[0]);
				
				Mirr_pos[mn].X = curPos.X;
				Mirr_pos[mn].Y = curPos.Y;
				mn++;
			}

			//스위치그리기
			if (GBInfo_B[y][x] == 's')
			{
				if (x == 1 || x == B_GBOARD_WIDTH - 2)//세로
					drawSwitch(SwitchInfo[1]);

				else
					drawSwitch(SwitchInfo[0]);

				Switch_pos[sn].X = curPos.X;
				Switch_pos[sn].Y = curPos.Y;
				sn++;
			}

			//Boss그리기
			if (GBInfo_B[y][x] == 'b')
			{
				drawBoss(BossInfo[0]);
				Boss_pos.X = curPos.X;
				Boss_pos.Y = curPos.Y;
			}

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}


void DrawOb()	//돌출 지형을 그리는 함수
{
	int x, y;
	
	if (Switch_N)
	{
		if (check_N == 0)		
			DeleteOb();			
		
		check_N++;

		if (check_N== 11)
		{
			Switch_N = false;
			starttime = 0;
			check_N = 0;
			Switch_N = false;
		}
	}

	if (B_OK)	//보스맵 그리기
	{
		drawGB_B(GBInfo_B[0]);
	}
	else 	//일반맵 그리기
	{
		for (y = 0; y < 29; y++)
		{
			for (x = 0; x < 25; x++)
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
		SetCurrentCursorPos(20, 30);
		printf("Game Over!\n");
		Sleep(50);
		getchar();
		exit(0);
	}

	else if (attacked)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

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

	return 1;
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

	RemoveCursor();
	DrawOb();

	MT_pos.Y = 25;
	srand((unsigned int)time(NULL));
	MT_pos.X = (rand() % 5) * 2 + 10;

	while (1)
	{

		if (B_OK == false)
		{
			for (int y = 0; y < GBOARD_HEIGHT; y++)
			{
				GBInfo_N[y][0] = 1;
				GBInfo_N[y][GBOARD_WIDTH - 1] = 1;
			}
			UpOB();
		}

		Sleep(50);
		DrawOb();
		Check_Ob++;
		
		
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();

		SetCurrentCursorPos(60, 1);
		printf("PC 체력: %3d", Physical(PCLife));

		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (DrawMeteo() == 0) {
			getchar;
			srand((unsigned int)time(NULL));
			MT_pos.X = (rand() % 5) * 2 + 10;
			MT_pos.Y = 25;
		}

		if (B_OK == false)
		{
			if (Check_Ob % 6 == 0)
				MakeOb();

			if (Check_Ob == 5)
			{
				GBInfo_N[10][3] = 3;
			}
		}

	}

	getchar();
	return 0;
}
