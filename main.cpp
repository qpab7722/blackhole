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
#define B_GBOARD_HEIGHT  31
#define B_GBOARD_WIDTH  31

COORD PC_pos = { 10,10 };
COORD MT_pos = { 0,0 };
COORD Mirr_pos[8] = { 0 };//반사경 위치
COORD Switch_pos[4] = { 0 };//스위치 위치
COORD Boss_pos = { 0,0 }; //보스 위치

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];

int speed_laser = 20;
int speed = 30;
int check = 0; // 스위치후 delete
int PCLife = 30;	//PC의 체력

int ObTime = 0;	//올라가는 간격	(장애물과 장애물사이 간격)
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
int count = 0; // 레이저 간격 변수
int L;//레이저 모델 번호
bool reflect = false;//반사체크 변수

int BossLife = 10;	//보스의 체력
int checkStage = 1;	//현재 Stage

int Mirr_num[4] = { 0 };//미러에 누적된 횟수 변수
int Mirr_overheattime[4] = { -1 };//과열 상태가 시작된 시간을 체크하는 변수
int B_time = 0;//보스맵입장한지 몇분짼지

bool clear_N = false; //노말맵 도착점 위해 호출 변수


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


//Boss를 그리는 함수
void DrawBoss(char BossInfo[7][15])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<7; y++)
	{
		for (x = 0; x<15; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] == 1)
				printf("▣");
			if (BossInfo[y][x] == 2)
				printf("♣");
			if (BossInfo[y][x] == 3)
				printf("▲");
			if (BossInfo[y][x] == 4)
				printf("▼");
			if (BossInfo[y][x] == 5)
				printf("■");

		}
	}
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

int DetectCollision_Laser(int posX, int posY, char LaserInfo[5][5], char GBInfo_B[31][31])//레이저랑 반사경이랑 부딪힐때 함수
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY + 1;

	int nonotime = 3;//몇번 범출껀지
	int mindex=-1;

	for (x = 0; x < 5; x++)
		for (y = 0; y < 5; y++) {
			if (LaserInfo[y][x] == 1)
			{
				SetCurrentCursorPos(62, 15);
				printf("충돌");
				switch (GBInfo_B[arrY + y][arrX + x])
				{
					//1
				case 'u' :
					mindex = 0;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirr3 %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
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
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
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
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
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
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10초간 과열 후 초기화
						{
							Mirr_overheattime[mindex] = -1;
							Mirr_num[mindex] = 0;
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

//Laser를 그리는 함수
void DrawLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
				printf("º");

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);

}

//Laser를 지우는 함수
void DeleteLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
				printf(" ");
		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//Laser를 쏘는는 함수 (Draw & Delete) 
void ShootLaser()
{
	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		int len = 21;
		for (int i = 0; i<len; i++)
		{
			if (reflect)// 반사레이저 쏘기 
			{
				if (i != 0)
				{
					SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i + 1);
					DeleteLaser_B(LaserInfo[5]);
				}
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i);
				DrawLaser_B(LaserInfo[5]);	//반사레이저 
			}

			if (i != 0)//첨엔 레이저 지울꺼 없당
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + i - 1);
				DeleteLaser_B(LaserInfo[L]);	//레이저 지움
			}
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + i);
			DrawLaser_B(LaserInfo[L]);	//레이저 쏘기		

			if (i == len - 1)	//초기화
			{
				DeleteLaser_B(LaserInfo[L]);
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i);
				DeleteLaser_B(LaserInfo[5]);
				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + i, LaserInfo[L], GBInfo_B[Switch_B % 4]))//다음 포문에서 반사 레이저를 그려줌
				reflect = true;

			if (i == len - 1)	//초기화
			{
				L = (rand() % 4) + 1;
			}
			Sleep(speed_laser);
		}

	}

	if (Switch_B % 2 == 1)	//마름모 맵
	{
		int len = 16;
		for (int i = 0; i<len; i++)
		{
			if (reflect)// 반사레이저 쏘기 
			{
				if (i != 0)
				{
					SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i + 1);
					DeleteLaser_B(LaserInfo[5]);
				}
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i);
				DrawLaser_B(LaserInfo[5]);	//반사레이저 

			}

			if (i != 0)//첨엔 레이저 지울꺼 없당
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + i - 1);
				DeleteLaser_B(LaserInfo[L]);	//레이저 지움
			}
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + i);
			DrawLaser_B(LaserInfo[L]);	//레이저 쏘기		



			if (i == len - 1)	//초기화
			{
				DeleteLaser_B(LaserInfo[L]);
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + len + 3 - 1 - i);
				DeleteLaser_B(LaserInfo[5]);
				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + i, LaserInfo[L], GBInfo_B[Switch_B % 4]))//다음 포문에서 반사 레이저를 그려줌
				reflect = true;
			if (i == len - 1)	//초기화
			{
				//L = (rand() % 4) + 1;
				L = (++L % 4) + 1;//테스트용
			}
			Sleep(speed_laser);
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
				printf("■");
			}

			//반사경그리기
			if (GBInfo_B[y][x] == 'u' || GBInfo_B[y][x] == 'i' || GBInfo_B[y][x] == 'o' || GBInfo_B[y][x] == 'p')
			{
				printf("@");

				Mirr_pos[mnb].X = x;
				Mirr_pos[mnb].Y = y;
				mnb++;

			}

			//스위치그리기
			if (GBInfo_B[y][x] == 's')
			{
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

		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[1][y][x] == 1)
				{
					printf("◆");
				}

				//반사경그리기
				if (GBInfo_B[1][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//스위치그리기
				if (GBInfo_B[1][y][x] == 's')
				{
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

					DrawBoss(BossInfo[0]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);
	}

	else if ((Switch_B % 4) == 2) // 90도 이동
	{
		for (y = 0; y < B_GBOARD_HEIGHT; y++)
		{
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[2][y][x] == 1)
				{
					printf("■");
				}

				//반사경그리기
				if (GBInfo_B[2][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//스위치그리기
				if (GBInfo_B[2][y][x] == 's')
				{
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

		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//벽그리기
				if (GBInfo_B[3][y][x] == 1)
				{
					printf("◆");
				}

				//반사경그리기
				if (GBInfo_B[3][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//스위치그리기
				if (GBInfo_B[3][y][x] == 's')
				{
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

					DrawBoss(BossInfo[0]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);


	}

	else if ((Switch_B % 4) == 0) //180도 부터는 다시 처음부터!
	{
		drawGB_B(GBInfo_B[0]);
	}
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

void deleteGB_N() //일반맵 지우는 함수
{

		for (int y = 0; y < GBOARD_HEIGHT; y++)
		{
			for (int x = 0; x < GBOARD_HEIGHT; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				printf("　");
			}
		}
	
}

void DrawMap_Switch()	//맵을 그리는 함수 - 스위치의 변화에 따른 변화까지 그려줌
{
	int x, y;

	if (Switch_N && changeMap_Normal)//일반맵에서 스위치를 건들였을때
	{

		if (check_N == 0)//스위치 처음 눌렀을때 지우자
		{
			deleteGB_N();
			MT_pos.X == 28;
			MT_pos.Y == 15;
		}
		check_N++;
		if (check_N == 11)//10번 뒤에 다시 돌림
		{
			deleteGB_N();
			starttime = 0;
			check_N = 0;
			Switch_N = false;
			MT_pos.X == 15;
			MT_pos.Y == 24;
		}
	}

	if (changeMap_Boss)	//보스맵 그리기
	{
		if (check_B == 0)//스위치 처음 눌렀을때 지우자
		{
			DeleteOb();
		}

		check_B++;

		if ((Switch_B % 4) == 0)//보스맵 모델 4개중에 0번째 모델 그리기
			drawGB_B(GBInfo_B[0]);
		else
			Rotate_BossMap();
	}
	else if (changeMap_Normal) 	//일반맵 그리기
	{

		for (int y = 0; y < GBOARD_HEIGHT; y++)
		{
			GBInfo_N[y][0] = 2;
			GBInfo_N[y][GBOARD_WIDTH - 1] = 2;

		}


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
					printf("▲");
				if (GBInfo_N[y][x] == 2)
					printf("■");
				if (GBInfo_N[y][x] == 3)
					printf("★");
				if (GBInfo_N[y][x] == 5)
					printf("ⓒ");
				else
					printf("　");

			}

		}
	}

}

void UpOB()	//돌출 지형을 일정 간격마다 위로 올려주는 함수 
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT-1; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH * sizeof(int));//내장함수,복사할 떄 많이들 사용
		for (x = 0; x < GBOARD_WIDTH; x++)
			GBInfo_N[GBOARD_HEIGHT - 1][x] = 0;
	}
}

void MakeOb()	//돌출 지형을 GBInfo_N에 생성해주는 함수 
{
	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 6) * 2 + 2; // 1~23
	Ran = (rand() % 8) * 2 + 4*2 ;

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
			DrawMap_Switch();
		}

		MT_pos.Y -= 2;
		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);
		DrawMT(MeteoInfo[0]);

		return 1;



	}
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

		printf("Game Clear!!");
		Sleep(1000);

		//랭킹함수 여기에다 놓으시면 되시고요~
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
		changeMap_Boss = false;	//보스맵 끄기
		changeMap_Normal = true;	//일반맵 켜기

	}

}

int Physical_PC(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		SetCurrentCursorPos(30, 0);

		printf("Game Over!\n");
		Sleep(50);
		getchar();
		exit(0);
	}
	else if (attacked && attacked_Boss)//보스 맵에서 보스에게 아팠을때
		nowLife -= 2;

	else if (attacked && attacked_Boss == false)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

int Physical_Boss(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		isB_Clear();
	}

	//else if (attacked && attacked_Boss == false)	//아팠을때	//레이저 수정 받아야 할듯	//그래서 일단 계속 감소하도록했음
	//nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_B[31][31])	//충돌 함수
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
				if (GBInfo_N[arrY + y][arrX + x] == 2)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 1)	//장애물이랑 부딪혔을때
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
					DeleteMT(MeteoInfo[0]);
					MT_pos.Y = 0;
				} //충돌시 1로 돌아가서 int main문의 조건성립 -> y가 1일때 재생성


			}

			///전환맵
			if (PCInfo[x][y] == 1 && changeMap_Normal == true && Switch_N == true)	//pc가 1이고, 일반맵 맞고, 스위치는 맞음
			{

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 2)	//벽이랑 부딪혔을때
					return 0;

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 1)	//장애물이랑 부딪혔을때
				{
					attacked = true;
					return 0;
				}


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
					if (Switch_B % 2 == 0)
						PC_pos.Y = 20;
					else if (Switch_B % 2 == 1)
					{
						PC_pos.Y = 6;
						PC_pos.X = 20;
					}

					deleteGB_B();
				}

				//if (Boss_pos.X == PC_pos.X &&Boss_pos.Y + 3 + count == PC_pos.Y);
				//{
				//	//printf("레이저");


				//}



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
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0)//부딪힘
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
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 || PC_pos.Y == 0)//부딪힘
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
	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == false)	//부딪힘	//일반맵 올라오는 벽	//스위치 X
	{
		PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//부딪힘	//전환맵 아래 벽//스위치 O
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//부딪힘	//전환맵 옆으로 다가오는 벽//스위치 O
	{
		PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == false && changeMap_Boss == true)//부딪힘		//보스맵	O	//GBInfo_B[Switch_B % 4]에서 Switch_B % 4는 보스맵 모델 번호임
		return 0;

	if (Switch_B % 2 == 1)
	{
		deletePC(PCInfo[0]);
		PC_pos.Y += 1;
		if (PC_pos.X<28)
			PC_pos.X += 2;
		else if (PC_pos.X>28)
			PC_pos.X -= 2;
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);
		Sleep(50);
	}
	else
	{
		deletePC(PCInfo[0]);
		PC_pos.Y += 1;
		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);
		Sleep(50);
	}
	return 1;
}

void isN_clear()//클리어(일반)
{
	SetCurrentCursorPos(30, 10);
	printf("Stage %d 일반맵 클리어", checkStage);	//일단 출력
	Sleep(1000);


	check_B = 0;//콘솔창 한번 지워주기 위해서 
	deleteGB_B();	//stage 글자 안지워져서 지우려고 
	changeMap_Normal = false;//일반맵 아님
	changeMap_Boss = true;//보스맵 맞음
	PC_pos.Y = 10;//PC위치 옮겨줌

	Physical_Boss(BossLife);	// 탄마다 보스 체력 다르게 해줌

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
				break;
			}
		}
		Sleep(speed);
	}
}


int main(void)
{
	srand((unsigned int)time(NULL));
	L = (rand() % 4) + 1;	//레이저 모델 선택

	RemoveCursor();
	DrawMap_Switch();

	MT_pos.Y = 28;
	MT_pos.X = (rand() % 5) * 2 + 10;

	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 6) * 2 + 2; // 1~23
	Ran = (rand() % 8) * 2 + 4 * 2;

	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
	{
		GBInfo_N[18][x+4] = 1;
		GBInfo_N[24][x+7] = 1;
	} //처음 시작시 화면에 강제로 돌출지형 그려주는 것, 시작부터 장애물 채우고 시작!
	

	while (1)
	{

		if (changeMap_Normal == true && changeMap_Boss == false && clear_N == false)
		{
			
			UpOB();
			

		}
		DrawMap_Switch();
		ObTime++;

		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);

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
		printf("Boss : %3d, %3d", Mirr_pos[0].X, Mirr_pos[0].Y);
		SetCurrentCursorPos(62, 8);
		printf("Boss : %3d, %3d", Mirr_pos[1].X, Mirr_pos[1].Y);
		SetCurrentCursorPos(62, 9);
		printf("Boss : %3d, %3d", Mirr_pos[2].X, Mirr_pos[2].Y);
		SetCurrentCursorPos(62, 10);
		printf("Boss : %3d, %3d", Mirr_pos[3].X, Mirr_pos[3].Y);



		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (!Shoot_MT() && Switch_N == false) {
			MT_pos.X = (rand() % 20) * 2 +2;
			MT_pos.Y = 26;
		}

		else if (!Shoot_MT() && Switch_N == true) {
			MT_pos.X = (rand() % 20);
			MT_pos.Y = 26 * 2;
		}



		if (changeMap_Normal == true && changeMap_Boss == false && clear_N == false)	//일반맵 O	일때//보스맵 X
		{
			if (ObTime % 6 == 0) //돌출지형 간격 조건문
				MakeOb();

			if (ObTime == 5) // 스위치 호출 조건문 ( 임시 )
			{
				GBInfo_N[10][3] = 3;
			}

			/*if (SkTime % 15 == 0)
			DrawSk();*/
		}

		if (changeMap_Normal == true && changeMap_Boss == false && ObTime == 50)
		{
			clear_N = true;
			DrawClear_N();
			/*isN_clear();*/
		}//50번째 줄일 때 클리어(일반)
			

		if (changeMap_Normal == false && changeMap_Boss == true && check_B>0)	//레이저를 쏘쟛	//check_B는 보스맵을 다 그리고서 레이저를 쏘려고
		{
			ShootLaser();//레이저를 쏘는 함수 (Draw & Delete)
			B_time++;//보스맵 경과 시간 증가시키기
		}

		SetCurrentCursorPos(62, 12);
		printf("Boss time %3d",B_time);
	}

	getchar();
	return 0;
}

//전역변수 clear_N 은 일정 시간 지나면 일반맵 클리어 동시에 맨밑에 도착점이 생겨서 깃발(도착점) 나타낼려고 선언한 변수
//5번으로 동그라미 c모양으로 출력되는 게 도착점이니까 충돌로 클리어 부탁드려요!
//일단 돌출된 지형은 4칸이상으로 그려지게 해놨고, 하다가 이상하면 다시 말해주세용
//메테오 벽에 박히는것도 수정했습니당 ㅎ
//