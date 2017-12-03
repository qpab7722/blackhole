#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> 
#include <time.h> 
#include <conio.h>
#include "info.h" 


#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32

#define GBOARD_HEIGHT  29
#define GBOARD_WIDTH  25

//BOSS MAP length
#define B_GBOARD_HEIGHT  31
#define B_GBOARD_WIDTH  31

COORD PC_pos = { 10,10 };
COORD MT_pos = { 0,0 };
COORD Mirr_pos[8] = { 0 };//반사경 위치
COORD Switch_pos[4] = { 0 };//스위치 위치
COORD Boss_pos = { 0,0 }; //보스 위치
COORD Laser_pos = { 0,0 };	//레이저 위치
COORD ReflectLaser_pos = { 0,0 };	//레이저 위치



int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];

int speed_laser = 30;
int speed = 15;
int check = 0; // 스위치후 delete
int PCLife = 30;	//PC의 체력

int ObTime_o = 0;//올라가는 간격	(장애물과 장애물사이 간격)
int ObTime_t = 3;//올라가는 간격
				 //장애물이 단조로워서 두번 그리게 바꿔봄

int Check_Ob = 0;	//돌출된 지형 만들어지는 X좌표
int Ran;	//돌출된 지형 길이

int check_N;//전환시간 검사
int check_B;//보스맵 모델 화면 지우기
int starttime = 0;//전환 시작 시간

bool attacked = false;	//공격받았는지 알려주는 변수

bool Switch_N = false; //스위치 충돌 알려주는 변수
int Switch_B = 0;//보스맵 스위치 눌릴 때 마다 각도 조절해주는 변수 (맵 모델 선택 4개)

bool changeMap_Boss = false;//보스맵 전환 신호
bool changeMap_Normal = true;//일반맵 전환 신호
bool attacked_Boss = false;	//보스에게 공격 받았는지 알려주는 변수

int L;//레이저 모델 번호
bool reflect = false;//반사체크 변수
int BossLife =1;//보스의 체력

int checkStage = 1;	//현재 Stage

int Mirr_num[4] = { 0 };//미러에 누적된 횟수 변수
int Mirr_overheattime[4] = { -1 };//과열 상태가 시작된 시간을 체크하는 변수
int B_time = 0;//보스맵입장한지 몇분짼지

bool overheat[4] = { false };//과열됐는지 

bool clear_N = false; //노말맵 도착점 위해 호출 변수

int len; //레이저 길이
int count;//레이저 이동 (ShootLaser에 있던 i를 바꿈)
int StoreBoard[B_GBOARD_HEIGHT][B_GBOARD_WIDTH];	//레이저와 보스를 임시로 저장(움직여서 비교 못해서)

bool ba = false;//랭크 테스트 용
bool gameover = false;// 블랙홀용 게임오버 변수
int StoreHeight;//보스맵 임시 게임판의 세로
bool attackLaserBoss = false;

int* Password;	//암호(끈끈이) 배열
int PW_size;
int InputWord = 0;	//입력키
bool sticky = false;

int countBossAttack = 0;	//보스체력 횟수 변화
int countPCAttack = 0;	//pc 체력 횟수 변화

int Ro = 0; //보스맵 회전에서 회전 직후 만 딜리트 게임보드 하기 위해서 사용
int move = 0; //보스맵 회전에서 회전 직후 PC 좌표를 설정하기 위해서 사용
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

int DetectCollision_Meteo(int posX, int posY, char MeteoInfo[4][4])	//맵 랜덤으로 나오면 손 봐야 할듯
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
			if (changeMap_Normal == false && changeMap_Boss)//일반맵 아니고, 보스맵일때 장애물 지워줌
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
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				if (y == 1) printf("◎");
				if (y == 2)	printf("△");
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
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

//임시 게임판을 지우는 함수
void DeleteStoreBoard()
{
	if (count == 0)

		for (int i = 0; i<B_GBOARD_HEIGHT; i++)
			for (int j = 0; j<B_GBOARD_WIDTH; j++)
				StoreBoard[i][j] = 0;	//임시 게임판을 지움


}

//임시 게임판에 레이저 지우는 함수
void DeleteStoreLaser()
{
	int arrY;

	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//마름모 맵
	{
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 2;
	}

	for (int i = 8; i<arrY; i++)
		for (int j = 0; j<B_GBOARD_WIDTH; j++)
		{
			if (StoreBoard[i][j] == 2 || StoreBoard[i][j] == 3)
				StoreBoard[i][j] = 0;
		}

	for (int i = arrY + 5; i<B_GBOARD_HEIGHT; i++)
		for (int j = 0; j<B_GBOARD_WIDTH; j++)
		{
			if (StoreBoard[i][j] == 2 || StoreBoard[i][j] == 3)
				StoreBoard[i][j] = 0;
		}

}

//Boss를 그리는 함수	
void DrawBoss(char BossInfo[7][15])
{
	int x, y;
	int arrX = Boss_pos.X / 2;
	int arrY = Boss_pos.Y;

	COORD curPos = GetCurrentCursorPos();

	for (y = 0; y<7; y++)
	{
		for (x = 0; x<15; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
				printf("▣");
				StoreBoard[arrY + y][arrX + x] = 1;	//크래쉬가 안되서 임시 게임판 안에 넣어놓음
			}

			if (BossInfo[y][x] == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				printf("♣");
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			if (BossInfo[y][x] == 3)
				printf("▲");
			if (BossInfo[y][x] == 4)
				printf("▼");
			if (BossInfo[y][x] == 5)
				printf("■");

		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//Boss를 지워주는 함수
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

int DetectCollision_Laser(int posX, int posY, char LaserInfo[5][20], char GBInfo_B[31][31])//레이저랑 반사경이랑 부딪힐때 함수
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY + 1;

	int nonotime = 10;//몇번 범출껀지
	int mindex = -1;

	for (x = 0; x < 20; x++)
		for (y = 0; y < 5; y++) {
			if (LaserInfo[y][x] == 1)
			{
				SetCurrentCursorPos(62, 15);
				printf("충돌");
				switch (GBInfo_B[arrY + y][arrX + x])
				{
					//1
				case 'u':
					mindex = 0;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirr3 %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
							overheat[mindex] = false;
							return 1;
						}
						else
							return 0;
					}
					return 1;
					break;
					//2
				case 'i':
					mindex = 1;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirr3 %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
							overheat[mindex] = false;
							return 1;
						}
						else
							return 0;
					}
					return 1;
					break;
					//3
				case 'o':
					mindex = 2;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirr3 %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
							overheat[mindex] = false;
							return 1;
						}
						else
							return 0;
					}
					return 1;

					break;
					//4
				case 'p':
					mindex = 3;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirr3 %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
							overheat[mindex] = false;
							return 1;
						}
						else
							return 0;
					}
					return 1;
					break;

				}

			}
		}

	return 0;

}

int ddd = 0;
int DetectCollision_Boss(int posX, int posY, char LaserInfo[5][20])	//(int posX, int posY, char LaserInfo[5][20], char GBInfo_B[31][31])//보스랑 반사 레이저랑 부딪힐때 함수
{
	int x, y;

	int arrX = posX / 2;
	int arrY = posY;

	for (x = 0; x<20; x++)
		for (y = 0; y < 5; y++)
		{
			if (LaserInfo[y][x] == 2)	//반사 레이저 충돌
			{
				if (StoreBoard[arrY + y][arrX + x] == 1)
				{
					return 1;
				}
			}
		}

	return 0;
}

//Laser를 그리는 함수
void DrawLaser(char LaserInfo[5][20])
{
	int x, y;
	int arrX;
	int arrY;

	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		arrX = (Boss_pos.X - 6) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//마름모 맵
	{
		arrX = (Boss_pos.X - 10) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 2;
	}

	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
		for (x = 0; x<20; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);

				printf("º");
				StoreBoard[arrY + y][arrX + x] = 2;	//크래쉬가 안되서 보스 게임판 안에 넣어놓음

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (LaserInfo[y][x] == 2)
			{
				printf("º");
				StoreBoard[arrY + y][arrX + x] = 3;	//크래쉬가 안되서 보스 게임판 안에 넣어놓음
			}


		}
	SetCurrentCursorPos(curPos.X, curPos.Y);


	//DeleteStoreLaser();
}

//Laser를 지우는 함수
void DeleteLaser(char LaserInfo[5][20])
{
	int x, y;
	int arrX;
	int arrY;

	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		arrX = (Boss_pos.X - 6) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//마름모 맵
	{
		arrX = (Boss_pos.X - 10) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 2;
	}

	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
		for (x = 0; x<20; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
			{
				StoreBoard[arrY + y][arrX + x] = 0;	//크래쉬가 안되서 보스 게임판 안에 넣어놓음
				printf(" ");
			}

			if (LaserInfo[y][x] == 2)
			{
				printf(" ");
				StoreBoard[arrY + y][arrX + x] = 0;	//크래쉬가 안되서 보스 게임판 안에 넣어놓음
			}

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deleteGB_B() //보스맵 지우는 함수
{
	for (int y = 0; y < B_GBOARD_HEIGHT; y++)
	{
		for (int x = 0; x < B_GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			printf("　");
		}
	}
}

//보스맵을 그리는 함수
void drawGB_B(char GBInfo_B[31][31])
{
	int x, y, mnb = 0, snb = 0;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0; y<B_GBOARD_HEIGHT; y++)
		for (x = 0; x < B_GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			//벽그리기
			if (GBInfo_B[y][x] == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				printf("■");
			}

			//반사경그리기
			if (GBInfo_B[y][x] == 'u')
			{
				if (overheat[0])
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
				else
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				printf("@");
			}
			if (GBInfo_B[y][x] == 'i')
			{
				if (overheat[1])
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
				else
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				printf("@");
			}
			if (GBInfo_B[y][x] == 'o')
			{
				if (overheat[2])
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
				else
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				printf("@");
			}
			if (GBInfo_B[y][x] == 'p')
			{
				if (overheat[3])
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
				else
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				printf("@");
			}

			//스위치그리기
			if (GBInfo_B[y][x] == 's')
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("☎");

				Switch_pos[snb].X = curPos.X;
				Switch_pos[snb].Y = curPos.Y;
				snb++;
			}

			//Boss그리기
			if (GBInfo_B[y][x] == 'b')
			{

				Boss_pos.X = x * 2;
				Boss_pos.Y = y;
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);

				DrawBoss(BossInfo[0]);
			}

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void Rotate_BossMap() //맵 돌려주는 함수
{
	int x, y, mn = 0, sn = 0;
	COORD curPos = GetCurrentCursorPos();


	if ((Switch_B % 4) == 1) // 45도 이동
	{
		if (move == 0)
		{
			PC_pos.Y = 20;
			move++;
		}


		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[1][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("◆");
				}

				//반사경그리기
				if (GBInfo_B[1][y][x] == 'u')
				{
					if (overheat[0])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[1][y][x] == 'i')
				{
					if (overheat[1])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[1][y][x] == 'o')
				{
					if (overheat[2])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[1][y][x] == 'p')
				{
					if (overheat[3])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}

				//스위치그리기
				if (GBInfo_B[1][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("☎");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss그리기
				if (GBInfo_B[1][y][x] == 'b')
				{
					Boss_pos.X = x * 2;
					Boss_pos.Y = y;
					SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);

					DrawBoss(BossInfo[1]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);
	}

	else if ((Switch_B % 4) == 2) // 90도 이동
	{
		if (move == 0)
		{
			PC_pos.X = 10;
			PC_pos.Y = 10;
			move++;
		}
		for (y = 0; y < B_GBOARD_HEIGHT; y++)
		{
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[2][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("■");
				}

				//반사경그리기
				if (GBInfo_B[2][y][x] == 'u')
				{
					if (overheat[0])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[2][y][x] == 'i')
				{
					if (overheat[1])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[2][y][x] == 'o')
				{
					if (overheat[2])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[2][y][x] == 'p')
				{
					if (overheat[3])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}

				//스위치그리기
				if (GBInfo_B[2][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("☎");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss그리기
				if (GBInfo_B[2][y][x] == 'b')
				{
					Boss_pos.X = x * 2;
					Boss_pos.Y = y;
					SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);

					DrawBoss(BossInfo[0]);
				}

			}
		}

		SetCurrentCursorPos(curPos.X, curPos.Y);
	}


	else if ((Switch_B % 4) == 3) // 135도 이동
	{
		if (move == 0)
		{
			PC_pos.X = 40;
			PC_pos.Y = 18;
			move++;
		}


		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[3][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("◆");
				}

				//반사경그리기
				if (GBInfo_B[3][y][x] == 'u')
				{
					if (overheat[0])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[3][y][x] == 'i')
				{
					if (overheat[1])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[3][y][x] == 'o')
				{
					if (overheat[2])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}
				if (GBInfo_B[3][y][x] == 'p')
				{
					if (overheat[3])
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
					else
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					printf("@");
				}

				//스위치그리기
				if (GBInfo_B[3][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("☎");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss그리기
				if (GBInfo_B[3][y][x] == 'b')
				{
					Boss_pos.X = x * 2;
					Boss_pos.Y = y;
					SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);

					DrawBoss(BossInfo[1]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);


	}

	else if ((Switch_B % 4) == 0) //180도 부터는 다시 처음부터!
	{
		deleteGB_B();
		drawGB_B(GBInfo_B[0]);
	}
}

void deleteGB_N() //일반맵 지우는 함수
{

	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (int x = 0; x < GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			printf("　");
		}
	}

}

void DeleteStage()
{
	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (int x = 0; x < GBOARD_WIDTH; x++)
		{
			GBInfo_N[y][x] = 0;
		}
	}
} // 스테이지가 올라갈때 마다 게임보드 초기화


void DrawStart()
{
	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
	{
		GBInfo_N[18][x + 4] = 1;
		GBInfo_N[24][x + 7] = 1;
	} //처음 시작시 화면에 강제로 돌출지형 그려주는 것, 시작부터 장애물 채우고 시작!
}

void UpOB()	//돌출 지형을 일정 간격마다 위로 올려주는 함수 
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT - 1; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH * sizeof(int));//내장함수,복사할 떄 많이들 사용
		for (x = 0; x < GBOARD_WIDTH; x++)
			GBInfo_N[GBOARD_HEIGHT - 1][x] = 0;
	}
}

void MakeOb_one()	//돌출 지형을 GBInfo_N에 생성해주는 함수 
{
	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 10); // 1~23    
	Ran = (rand() % 8) + 4;

	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
		GBInfo_N[27][x] = 1;
}

void MakeOb_two()	//돌출 지형을 GBInfo_N에 생성해주는 함수 
{
	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 20) + 10; // 1~23    
	Ran = (rand() % 8) + 4;

	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
		GBInfo_N[27][x] = 1;
}

void DrawMT(char MeteoInfo[4][4]) {
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
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				printf("◈");
			}

			if (MeteoInfo[y][x] == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				printf("˚");
			}
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

void DrawGame(char Game[6][24])
{
	int x, y;

	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<6; y++)
		for (x = 0; x < 24; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if( Game[y][x] == 1)
			{
				printf("■");

			}
		}

	SetCurrentCursorPos(curPos.X, curPos.Y);
}


void isB_Clear()//클리어(보스)
{
	SetCurrentCursorPos(30, 10);
	printf("Stage %d 보스맵 클리어", checkStage);	//일단 출력
	
	Sleep(1000);

	if (checkStage == 4)	//4탄 클리어
	{
		SetCurrentCursorPos(30, 11);
		printf("Stage %d 클리어", checkStage);	//일단 출력
		Sleep(1000);

		SetCurrentCursorPos(30, 10);
		printf("                      ");
		SetCurrentCursorPos(30, 11);
		printf("                      ");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
		SetCurrentCursorPos(10, 10);
		DrawGame(Game[0]);//game
		Sleep(10);
		SetCurrentCursorPos(10, 20);
		DrawGame(Game[1]);//game
		Sleep(2000);
		exit(0);
		
	}

	else
	{
		SetCurrentCursorPos(30, 11);
		printf("Stage %d 클리어", checkStage);	//일단 출력
		Sleep(1000);

		SetCurrentCursorPos(30, 12);
		printf("다음 Stage 계속...!");
		Sleep(1000);

		deleteGB_B();	//보스맵 지우기

		checkStage++;	//탄 수 올리기

		countBossAttack = 0;
		countPCAttack = 0;

		//보스체력
		if (checkStage == 1)
			BossLife = 10;
		if (checkStage == 2)
			BossLife = 20;
		if (checkStage == 3)
			BossLife = 30;
		if (checkStage == 4)
			BossLife = 40;


		changeMap_Boss = false;	//보스맵 끄기
		changeMap_Normal = true;	//일반맵 켜기

		PC_pos.X = 10;
		PC_pos.Y = 10;

		ObTime_o = 0;
		ObTime_t = 0;
		clear_N = false;

		DeleteStage();
		

	}

}

int Physical_PC(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	int nowLife = maxLife - countPCAttack;

	if (nowLife < 0)	//음수일때
		nowLife = 0;

	if (gameover) nowLife = 0;
	if (nowLife == 0)	//체력이 0일때 game over
	{
		SetCurrentCursorPos(30, 0);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		SetCurrentCursorPos(10, 10);
		DrawGame(Game[0]);//game
		Sleep(10);
		SetCurrentCursorPos(10, 20);
		DrawGame(Game[2]);//over
		
		Sleep(1000);
		getchar();
		exit(0);
	}
	else if (attacked && attacked_Boss)//보스 맵에서 보스에게 아팠을때
	{
		nowLife -= 2;
		countPCAttack += 2;
	}
		

	else if (attacked && attacked_Boss == false)	//아팠을때
	{
		nowLife--;
		countPCAttack++;
	}
		
	attacked_Boss = false;
	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

void Rank()
{
	int S = 10;
	int A = 20;
	int B = 30;
	int C = 40;

	for (int x = 0; x<31 * 2; x++)
		for (int y = 0; y < 31; y++)
		{
			SetCurrentCursorPos(x, y);
			printf(" ");
		}

	//랭크 예쁘게 출력하기는 시간되면 하장~
	if (B_time < S)
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[0][y][x] == 1)
						printf("■");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");

	}
	else if (B_time < A)
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[1][y][x] == 1)
						printf("■");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");
	}

	else if (B_time < B)
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[2][y][x] == 1)
						printf("■");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");
	}
	else if (B_time < C)
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[3][y][x] == 1)
						printf("■");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");
	}
	else
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[4][y][x] == 1)
						printf("■");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");
	}

	Sleep(1000);
}

int Physical_Boss(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	int nowLife = maxLife - countBossAttack;

	if (nowLife < 0)	//음수일때
		nowLife = 0;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		Rank();
		isB_Clear();
	}

	else if (attackLaserBoss)	//아팠을때
	{
		nowLife--;
		countBossAttack++;
	}

	attackLaserBoss = false;	//다시 attacked을 false (원상태)로 돌려준다.

	return nowLife;	//현재 체력을 리턴한다.

}

void isN_clear()//클리어(일반)
{
	for (int y = 0; y < 30; y++)
		for (int x = 0; x < 80; x++)
		{
			SetCurrentCursorPos(x, y);
			printf(" ");
		}

	SetCurrentCursorPos(30, 15);
	printf("Stage %d 일반맵 클리어", checkStage);	//일단 출력
	Sleep(1000);

	check_B = 0;//콘솔창 한번 지워주기 위해서 
	deleteGB_B();	//stage 글자 안지워져서 지우려고 
	changeMap_Normal = false;//일반맵 아님
	changeMap_Boss = true;//보스맵 맞음
	PC_pos.Y = 20;//PC위치 옮겨줌

	Physical_Boss(BossLife);	// 탄마다 보스 체력 다르게 해줌

}



int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_B[B_GBOARD_HEIGHT][B_GBOARD_WIDTH])	//충돌 함수
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;

	if (Switch_N)	//일반맵에서 스위치 일때
	{
		arrX = posY + 1;
		arrY = posX / 2 - 1;
	}
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			///일반맵
			if (PCInfo[y][x] == 1 && changeMap_Normal == true && Switch_N == false)	//pc가 1이고, 일반맵 맞고, 스위치는 아님
			{
				if (GBInfo_N[arrY + y][arrX + x] == 1)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}

				if (GBInfo_N[arrY + y][arrX + x] == 2)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 5)	//도착
					isN_clear();

				if (GBInfo_N[arrY + y][arrX + x] == 4)	//끈끈이
				{
					sticky = true;
					//DrawPassword();	//암호 생성
					//DeletePassword();	//암호 해제

				}
				if (GBInfo_N[arrY + y][arrX + x] == 3)	//일반맵 스위치
				{
					Switch_N = true;
					deletePC(PCInfo);
					//PC_pos.Y = 13;
					MT_pos.X = 28;
					MT_pos.Y = 3;
					return 0;
				}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//운석 충돌 (서로 뚫고 지나감)
				{
					attacked = true;
					DeleteMT(MeteoInfo[0]);
					MT_pos.Y = 0;
				} //충돌시 1로 돌아가서 int main문의 조건성립 -> y가 1일때 재생성


			}

			///전환맵
			if (PCInfo[x][y] == 1 && changeMap_Normal == true && Switch_N == true)	//pc가 1이고, 일반맵 맞고, 스위치는 맞음
			{

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 2)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y + 1][arrX + x ] == 1)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}

				if (GBInfo_N[arrY + y + 2][arrX + x - 1] == 5)	//도착
					isN_clear();

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 4)	//끈끈이
					sticky = true;


				if (((PC_pos.Y == MT_pos.X) || (PC_pos.Y + 1 == MT_pos.X)) && ((PC_pos.X + 3 == MT_pos.Y) || (PC_pos.X + 2 == MT_pos.Y) || (PC_pos.X + 1 == MT_pos.Y)))	//운석 충돌 (서로 뚫고 지나감)
				{
					attacked = true;
					DeleteMT(MeteoInfo[0]);
					MT_pos.Y = 0;;
				}

			}

			//보스 맵
			if (PCInfo[y][x] == 1 && changeMap_Normal == false && changeMap_Boss == true)//일반맵 아니고, 보스 맵은 맞음
			{
				if (GBInfo_B[arrY + y][arrX + x] == 1)	//벽이랑 부딪혔을때
				{
					return 0;
				}

				if (GBInfo_B[arrY + y][arrX + x] == 'm')	//거울이랑 부딪혔을때
				{
					//printf("거울");
					return 0;
				}

				if (GBInfo_B[arrY + y][arrX + x] == 's')	//스위치랑 부딪혔을때
				{
					Switch_B++;
					check_B = 0;
					DeleteStoreBoard();	//임시맵 초기화

					if (Switch_B % 2 == 0)
						PC_pos.Y = 20;
					else if (Switch_B % 2 == 1)
					{
						PC_pos.Y = 20;
						PC_pos.X = 20;
					}

					deleteGB_B();
					Ro = 0;
				}

				if (StoreBoard[arrY + y][arrX + x] == 1)	//보스랑 충돌 ( 보스를 임시 맵에 넣어버렸음)
				{
					//printf("보스");
					attacked_Boss = true;
					attacked = true;
					return 0;
				}

				if (StoreBoard[arrY + y][arrX + x] == 2)	//레이저 충돌 ( 레이저를 임시 맵에 넣어버렸음)
				{
					//printf("레이저");
					attacked = true;
					//return 0;
				}

				if ((PC_pos.Y == MT_pos.X) && ((PC_pos.X + 3 == MT_pos.Y) || (PC_pos.X + 2 == MT_pos.Y) || (PC_pos.X + 1 == MT_pos.Y)))	//운석 충돌 (서로 뚫고 지나감)
				{
					attacked = true;
				}

			}


		}
	}
	return 1;
}



int ShiftRight()
{
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0)//부딪힘
	{
		if (changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//전환맵에서 부딪힘(밀림)
		{
			PC_pos.X -= 2;
			Sleep(speed);
		}
		return 0;
	}
	if (changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//전환맵 블랙홀
		if (PC_pos.X / 2 > GBOARD_WIDTH)
		{
			gameover = true;
			Physical_PC(30);
			return 0;
		}
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	deletePC(PCInfo[0]);
	PC_pos.X += 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(speed);
	return 1;

}
int ShiftLeft()
{
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0)//부딪힘
		return 0;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	deletePC(PCInfo[0]);
	PC_pos.X -= 2;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(speed);
	return 1;
}

int Jump()
{
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 || PC_pos.Y == 0)//부딪힘
		return 0;
	else if (PC_pos.Y - 1 < 0)//일반맵 이동제한
		return 0;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	deletePC(PCInfo[0]);
	PC_pos.Y -= 1;
	SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
	drawPC(PCInfo[0]);
	Sleep(speed);

	return 1;
}

int Gravity_N()
{

	if (changeMap_Boss == false && changeMap_Normal == true && PC_pos.Y + 2 + 1 > GBOARD_HEIGHT)//블랙홀
	{
		gameover = true;
		Physical_PC(30);
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == false)	//부딪힘	//일반맵 올라오는 벽	//스위치 X	
	{
		if (PC_pos.Y > 0)//위로 못넘어가게
			PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//부딪힘	//전환맵 아래 벽//스위치 O
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//부딪힘	//전환맵 옆으로 다가오는 벽//스위치 O
	{
		if (PC_pos.X - 2 > 0)//이동제한
			PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == false && changeMap_Boss == true)//부딪힘		//보스맵	O	//GBInfo_B[Switch_B % 4]에서 Switch_B % 4는 보스맵 모델 번호임
		return 0;

	if (Switch_B % 2 == 1)
	{
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		deletePC(PCInfo[0]);
		PC_pos.Y += 1;
		if (PC_pos.X<28)
			PC_pos.X += 2;
		else if (PC_pos.X>28)
			PC_pos.X -= 2;
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);

		Sleep(speed);
	}
	else
	{
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		deletePC(PCInfo[0]);
		PC_pos.Y += 1;
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);

		Sleep(speed);
	}
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
				if (sticky == true)
					InputWord = LEFT;
				else
					ShiftLeft();
				break;
			case RIGHT:
				if (sticky == true)
					InputWord = RIGHT;
				else
					ShiftRight();
				break;
			case SPACE:
				if (sticky == true)
					InputWord = SPACE;
				else
					Jump();
				break;
			case '1':	//추가
				Switch_N = true;
				break;
			case '2': //탭 추가
				Switch_B++;
				check_B = 0;
				if (Switch_B % 2 == 0)
					PC_pos.Y = 20;
				else if (Switch_B % 2 == 1)
				{
					PC_pos.Y = 6;
					PC_pos.X = 20;
				}
				deleteGB_B();

				Ro = 0;
				break;

				//끈끈이 하려고
			case UP:
				if (sticky == true)
					InputWord = UP;
				break;

			case DOWN:
				if (sticky == true)
					InputWord = DOWN;
				break;

			}
		}
		Sleep(speed);
	}
}


void DrawPassword()	//끈끈이(암호만듦)
{
	//Stage에 따라서 암호길이 조정
	if (checkStage == 1)
		PW_size = 1;
	if (checkStage == 2)
		PW_size = 3;
	if (checkStage == 3)
		PW_size = 5;
	if (checkStage == 4)
		PW_size = 7;

	Password = (int*)malloc(sizeof(int)*PW_size);	//Password(암호배열)를 PW_size만큼 만듦

	srand(time(NULL));

	int word = 0;	//암호값

	for (int i = 0; i< PW_size; i++)
	{
		int W = rand() % 5;	//암호 뽑음

		if (W == 0)
			word = LEFT;
		if (W == 1)
			word = RIGHT;
		if (W == 2)
			word = UP;
		if (W == 3)
			word = DOWN;
		if (W == 4)
			word = SPACE;

		*(Password + i) = word;	//암호값을 Password(암호배열)에 넣음

	}

}

void DeletePassword()	//끈끈이(암호해제)
{
	int i = 0;
	int j = 0;

	while (i<PW_size)
	{
		SetCurrentCursorPos(20, 19);
		printf("%3d", ++j);	//몇번째 키보드인지 알기 위해서
		SetCurrentCursorPos(20, 20);
		printf("                                             ");	//이전 키보드 입력 출력 지우기

		SetCurrentCursorPos(20, 20);
		if (*(Password + i) == LEFT)
			printf("Left←  ");
		if (*(Password + i) == RIGHT)
			printf("Right→  ");
		if (*(Password + i) == UP)
			printf("Up↑  ");
		if (*(Password + i) == DOWN)
			printf("Down↓  ");
		if (*(Password + i) == SPACE)
			printf("Space bar  ");

		while (1)	//같은 값을 입력할때까지 무한루프
		{
			InputWord = 0;	//키보드값 초기화

			ProcessKeyInput();

			if (*(Password + i) == InputWord)
				break;
		}

		i++;
	}

	free(Password);	//Password 해제
	printf("Good!");
	sticky = false;
}

void DrawAllMap()	//모든 맵을 그리는 함수 - 스위치의 변화에 따른 변화까지 그려줌
{
	int x, y;
	int reversetime = 30;

	if (Switch_N && changeMap_Normal)//일반맵에서 스위치를 건들였을때
	{
		if (sticky)	//끈끈이 일때
		{
			deleteGB_N();	//지우자
			DeleteOb();
			deletePC(PCInfo[0]);


			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
			DrawPassword();
			DeletePassword();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}


		if (check_N == 0)//스위치 처음 눌렀을때 지우자
		{
			int tempx = PC_pos.X;
			int tempy = PC_pos.Y;
			deleteGB_N();
			MT_pos.X == 28;
			MT_pos.Y == 15;
			PC_pos.X = tempy * 2 - 1;
			PC_pos.Y = tempx / 2 + 1;
		}
		check_N++;

		if (check_N == reversetime)//10번 뒤에 다시 돌림
		{
			int tempx = PC_pos.X;
			int tempy = PC_pos.Y;
			deleteGB_N();
			starttime = 0;
			check_N = 0;
			Switch_N = false;
			MT_pos.X == 15;
			MT_pos.Y == 24;
			PC_pos.X = tempy * 2;
			PC_pos.Y = tempx / 2 - 1;
		}
		

	}

	if (changeMap_Boss)	//보스맵 그리기
	{
		if (check_B == 0)//스위치 처음 눌렀을때 지우자
		{
			DeleteOb();
		}

		check_B++;

		SetCurrentCursorPos(62, 10);
		printf("map: %3d", Switch_B % 4);

		if ((Switch_B % 4) == 0)//보스맵 모델 4개중에 0번째 모델 그리기
		{
			if (Ro == 0)
			{
				deleteGB_B();
				Ro++;
			}
			drawGB_B(GBInfo_B[0]);
		}
			

		else
		{
			if (Ro == 0)
			{
				deleteGB_B();
				Ro++;
			}
			Rotate_BossMap();
		}
			

	}
	else if (changeMap_Normal) 	//일반맵 그리기
	{

		if (sticky)	//끈끈이 일때
		{
			deleteGB_N();	//지우자
			DeleteOb();
			deletePC(PCInfo[0]);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
			DrawPassword();
			DeletePassword();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

		}
		else
		{
			for (int y = 0; y < GBOARD_HEIGHT; y++)
			{
				GBInfo_N[y][0] = 2;
				GBInfo_N[y][GBOARD_WIDTH - 1] = 2;
			}

			for (y = 0; y < 29; y++)
				for (x = 0; x < 25; x++)
				{
					if (Switch_N)
						SetCurrentCursorPos((y * 2), x);

					else
						SetCurrentCursorPos((x * 2), y);

					if (GBInfo_N[y][x] == 1)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
						printf("▲");
					}

					if (GBInfo_N[y][x] == 2)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
						printf("■");
					}

					if (GBInfo_N[y][x] == 3)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
						printf("★");
					}

					if (GBInfo_N[y][x] == 4)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
						printf("▒");
					}

					if (GBInfo_N[y][x] == 5)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
						printf("ⓒ");
					}

					else
						printf("　");

				}

		}

	}

}

//Laser를 쏘는 함수 (Draw & Delete) 
void ShootLaser()
{
	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		len = 17;

		for (count = 0; count<len; count++)
		{
			DrawAllMap();	//맵그리기
			DeleteStoreLaser();	//레이저 지우기(한줄씩)
			ProcessKeyInput();
			isCrash(PC_pos.X, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]);
			SetCurrentCursorPos(62, 0);
			printf("PC 체력: %3d", Physical_PC(PCLife));

			if (count == 0)	//storeBoard 전체 삭제하고 보스 그리기
			{
				DeleteStoreBoard();	//임시 게임판 전체 지우기
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);
				DrawBoss(BossInfo[0]);
			}

			if (count % 2 == 0)
				Gravity_N();

			if (reflect)// 반사레이저 쏘기 
			{
				if (count != 0)
				{
					SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 1 + 4 - 1);

					if (Switch_B % 4 == 0)	//0번 맵
						DeleteLaser(LaserInfo[14]);

					if (Switch_B % 4 == 2)	//2번 맵
						DeleteLaser(LaserInfo[16]);
				}

				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 4 - 1);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				if (Switch_B % 4 == 0)	//0번 맵
					DrawLaser(LaserInfo[14]);	//반사레이저 
				if (Switch_B % 4 == 2)	//2번 맵
					DrawLaser(LaserInfo[16]);	//반사레이저 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (count != 0)//첨엔 레이저 지울꺼 없당
			{
				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + 3 + count - 1 + 4);
				DeleteLaser(LaserInfo[L]);	//레이저 지움
			}
			SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + 3 + count + 4);
			DrawLaser(LaserInfo[L]);	//레이저 쏘기	

			if (Switch_B % 4 == 0)	//0번 맵
				if (DetectCollision_Boss(Boss_pos.X - 6, Boss_pos.Y + len + 5 - count, LaserInfo[14]) && (count == len - 1) && reflect == true)	//보스 아야
				{
					attackLaserBoss = true;
				}

			if (Switch_B % 4 == 2)	//2번 맵
				if (DetectCollision_Boss(Boss_pos.X - 6, Boss_pos.Y + len + 5 - count, LaserInfo[16]) && (count == len - 1) && reflect == true)	//보스 아야
				{
					attackLaserBoss = true;
				}


			if (count == len - 1)	//초기화
			{
				DeleteLaser(LaserInfo[L]);


				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 4);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				if (Switch_B % 4 == 0)	//0번 맵
					DeleteLaser(LaserInfo[14]);
				if (Switch_B % 4 == 2)	//2번 맵
					DeleteLaser(LaserInfo[16]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X - 6, Boss_pos.Y + 3 + count + 4, LaserInfo[L], GBInfo_B[Switch_B % 4]))//다음 포문에서 반사 레이저를 그려줌
				reflect = true;

			if (count == len - 1)	//초기화
			{
				L = (rand() % 4) + 1;
				//L = (++L % 14);//테스트용
				//L = (++L % 2);
			}

			Sleep(speed_laser);

		}



	}

	if (Switch_B % 2 == 1)	//마름모 맵
	{
		len = 15;
		for (count = 0; count<len; count++)
		{
			DrawAllMap();	//맵그리기
			DeleteStoreLaser();	//레이저 지우기(한줄씩)
			ProcessKeyInput();
			isCrash(PC_pos.X, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]);
			SetCurrentCursorPos(62, 0);
			printf("PC 체력: %3d", Physical_PC(PCLife));

			if (count == 0)	//storeBoard 전체 삭제하고 보스 그리기
			{
				DeleteStoreBoard();	//임시 게임판 전체 지우기
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);
				DrawBoss(BossInfo[1]);
			}

			if (count % 2 == 0)
				Gravity_N();

			if (reflect)// 반사레이저 쏘기 
			{
				if (count != 0)
				{
					SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 1 + 2 - 1);
					DeleteLaser(LaserInfo[15]);
				}
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 2 - 1);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				DrawLaser(LaserInfo[15]);	//반사레이저 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (count != 0)//첨엔 레이저 지울꺼 없당
			{
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + 3 + count - 1 + 2);
				DeleteLaser(LaserInfo[L]);	//레이저 지움
			}
			SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + 3 + count + 2);
			DrawLaser(LaserInfo[L]);	//레이저 쏘기		

			if (DetectCollision_Boss(Boss_pos.X - 10, Boss_pos.Y + len + 3 - count, LaserInfo[14]) && (count == len - 1) && reflect == true)	//보스 아야
			{
				attackLaserBoss = true;
			}

			if (count == len - 1)	//초기화
			{
				DeleteLaser(LaserInfo[L]);
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 2);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				DeleteLaser(LaserInfo[15]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X - 10, Boss_pos.Y + 3 + count + 2, LaserInfo[L], GBInfo_B[Switch_B % 4]))//다음 포문에서 반사 레이저를 그려줌
				reflect = true;
			if (count == len - 1)	//초기화
			{
				L = (rand() % 4) + 1;
				//L = (++L % 14);//테스트용
			}
			Sleep(speed_laser);
		}

	}
}


int Shoot_MT() //showMT의 역활 메테오 움직여주는 것
{
	if (changeMap_Boss == false && clear_N == false)
	{
		COORD curPos = GetCurrentCursorPos();

		if (MT_pos.Y == 0) {

			return 0;
		} //y가 1일때 메테오를 다시 아래부터 그려주기

		if (DetectCollision_Meteo(curPos.X, curPos.Y + 1, MeteoInfo[0]) == 0)
		{
			//DeleteOb(GBInfo_N[0]);
			DrawAllMap();
		}

		MT_pos.Y -= 2;
		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);
		DrawMT(MeteoInfo[0]);

		return 1;

	}
}

void DrawClear_N()
{
	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (int x = 0; x < GBOARD_WIDTH; x++)
		{
			if (y > GBOARD_HEIGHT - 8 && y != GBOARD_HEIGHT - 1)
				GBInfo_N[y][x] = 0;
			else if (y == GBOARD_HEIGHT - 1)
			{
				GBInfo_N[y][x] = 2;
			}

		}
	}

	GBInfo_N[GBOARD_HEIGHT - 2][12] = 5;//목적지 깃발?
}

void DrawSwitch_N()
{
	if (ObTime_o % 10 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][GBOARD_WIDTH - 2] = 3;
	}


	else if (ObTime_t % 15 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][1] = 3;
	}
}

void DrawSk()
{
	if (ObTime_t % 10 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 3][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 4][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 5][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 6][1] = 4;
	}


	else if (ObTime_t % 12 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 3][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 4][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 5][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 6][GBOARD_WIDTH - 2] = 4;
	}

}

int main(void)
{
	srand((unsigned int)time(NULL));
	L = (rand() % 4) + 1;	//레이저 모델 선택

	RemoveCursor();
	DrawAllMap();

	MT_pos.Y = 28;
	MT_pos.X = (rand() % 5) * 2 + 10;

	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 6) * 2 + 2; // 1~23
	Ran = (rand() % 8) * 2 + 4 * 2;

	DrawStart();

	while (1)
	{	
		if (changeMap_Normal == true && changeMap_Boss == false && clear_N == false)
		{

			UpOB();


		}
		DrawAllMap();
		ObTime_o++;
		ObTime_t++;

		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);
		if (!changeMap_Boss)
			Gravity_N();
		ProcessKeyInput();

		SetCurrentCursorPos(62, 0);
		printf("PC 체력: %3d", Physical_PC(PCLife));
		SetCurrentCursorPos(62, 1);
		printf("Boss 체력: %3d", Physical_Boss(BossLife));

		SetCurrentCursorPos(62, 3);
		printf("PC : %3d, %3d", PC_pos.X, PC_pos.Y);
		SetCurrentCursorPos(62, 4);
		printf("Boss : %3d, %3d", Boss_pos.X, Boss_pos.Y);
		SetCurrentCursorPos(62, 5);
		printf("MT: %3d, %3d", MT_pos.X, MT_pos.Y);


		SetCurrentCursorPos(62, 7);
		printf("checktime %d %d", ObTime_o, ObTime_t);




		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (!Shoot_MT() && Switch_N == false) {
			MT_pos.X = (rand() % 20) * 2 + 2;
			MT_pos.Y = 26;
		}

		else if (!Shoot_MT() && Switch_N == true) {
			MT_pos.X = (rand() % 20);
			MT_pos.Y = 26 * 2;
		}



		if (changeMap_Normal == true && changeMap_Boss == false && clear_N == false)	//일반맵 O	일때//보스맵 X
		{
			if (ObTime_o % 9 == 0) //돌출지형 간격 조건문
				MakeOb_one();

			if (ObTime_t % 4 == 0)
				MakeOb_two();


			DrawSk();
			DrawSwitch_N();

			/*if (SkTime % 15 == 0)
			DrawSk();*/
		}

		if (changeMap_Normal == true && changeMap_Boss == false && ObTime_o == 20)
		{
			clear_N = true;
			DrawClear_N();

			//isN_clear();
		}//50번째 줄일 때 클리어(일반)


		if (changeMap_Normal == false && changeMap_Boss == true && check_B>0)	//레이저를 쏘쟛	//check_B는 보스맵을 다 그리고서 레이저를 쏘려고
		{
			ShootLaser();//레이저를 쏘는 함수 (Draw & Delete)

			B_time++;//보스맵 경과 시간 증가시키기


		}

		SetCurrentCursorPos(62, 12);
		printf("Boss time %3d", B_time);
		//Sleep(200);
	}

	getchar();
	return 0;
}
