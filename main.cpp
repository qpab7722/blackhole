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
COORD Mirr_pos[4] = { 0 };//반사경 위치
COORD Switch_pos[4] = { 0 };//스위치 위치
COORD Boss_pos = { 0,0 }; //보스 위치

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];


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
bool reflect = false;



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

int DetectCollision_MTPC(int posX, int posY, char MeteoInfo[4][4], char PCInfo[4][4])//메테오랑 PC충돌 검사 함수
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY;

	for (x = 0; x < 5; x++)
		for (y = 0; y < 5; y++) {
			if (MeteoInfo[y][x] == 1)
			{
				if (PCInfo[arrY + y][arrX + x] == 1)
				{
					return 0;
				}
			}
		}

	return 1;

}


int DetectCollision_Laser(int posX, int posY, char LaserInfo[5][5], char GBInfo_B[31][31])//레이저랑 반사경이랑 부딪힐때 함수
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY;

	for (x = 0; x < 5; x++)
		for (y = 0; y < 5; y++) {
			if (LaserInfo[y][x] == 1)
			{
				if (GBInfo_B[arrY + y][arrX + x] == 'm')
				{
					SetCurrentCursorPos(62, 16);
					printf("반사");
					return 1;
				}
			}
		}

	return 0;

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
void DrawBoss(char BossInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (BossInfo[y][x] == 2)
				printf("Π");
			if (BossInfo[y][x] == 3)
				printf("<");
			if (BossInfo[y][x] == 4)
				printf(">");
			if (BossInfo[y][x] == 5)
				printf("Θ");
			if (BossInfo[y][x] == 6)
				printf("▼");
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


//Laser를 그리는 함수
void DrawLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{
		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
			{
				printf("♥");
			}

		}

	}
	SetCurrentCursorPos(curPos.X, curPos.Y);

}

//Laser를 지우는 함수
void DeleteLaser_B(char LaserInfo[5][5])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y<5; y++)
	{

		for (x = 0; x<5; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (LaserInfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);


}

//Laser를 쏘는는 함수 (Draw & Delete) 
void ShootLaser()
{
	if (Switch_B % 2 == 0)	//직사각형 맵
	{
		for (int i = 0; i<22; i++)
		{
			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + count, LaserInfo[L], GBInfo_B[Switch_B % 4]))
				reflect = true;

			if (reflect&&count != 21)
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 25 - count);
				DrawLaser_B(LaserInfo[5]);	//반사레이저 쏘기
				if (count == 22)
					reflect = false;
				Sleep(50);
				DeleteLaser_B(LaserInfo[5]);
			}

			SetCurrentCursorPos(62, 12);
			printf("Boss : %3d, %3d", Boss_pos.X, Boss_pos.Y + 3 + count);
			count++;
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + count);
			DrawLaser_B(LaserInfo[L]);	//레이저 쏘기
			if (reflect)
				Sleep(50);
			else
				Sleep(100);
			DeleteLaser_B(LaserInfo[L]);	//레이저 지움
		}
		if (count == 22)	//20번째가 되면 그만 쏘고 다시 시작
		{
			count = 0;
			L = (rand() % 4) + 1;
		}


	}

	if (Switch_B % 2 == 1)	//마름모 맵
	{
		for (int i = 0; i<16; i++)
		{

			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + count, LaserInfo[L], GBInfo_B[Switch_B % 4]))
				reflect = true;

			if (reflect && count != 15)
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + 16 - count);
				DrawLaser_B(LaserInfo[5]);	//반사레이저 쏘기
				if (count == 16)
					reflect = false;
				Sleep(50);
				DeleteLaser_B(LaserInfo[5]);
			}

			count++;
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + count);
			DrawLaser_B(LaserInfo[L]);	//레이저 쏘기
			if (reflect)
				Sleep(50);
			else
				Sleep(100);
			DeleteLaser_B(LaserInfo[L]);	//레이저 지움
		}

		if (count == 16)	//20번째가 되면 그만 쏘고 다시 시작
		{
			count = 0;
			L = (rand() % 4) + 1;
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
			if (GBInfo_B[y][x] == 'm')
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

void DrawMap_Switch()	//맵을 그리는 함수 - 스위치의 변화에 따른 변화까지 그려줌
{
	int x, y;

	if (Switch_N && changeMap_Normal)//일반맵에서 스위치를 건들였을때
	{

		if (check_N == 0)//스위치 처음 눌렀을때 지우자
		{
			DeleteOb();
		}
		check_N++;
		if (check_N == 11)//10번 뒤에 다시 돌림
		{
			MT_pos.Y == 28;
			Switch_N = false;
			starttime = 0;
			check_N = 0;
			Switch_N = false;
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
				if (GBInfo_N[y][x] == 4)
					printf("♨");
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
	Check_Ob = (rand() % 6) * 2;
	Ran = (rand() % 8) * 2;

	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
		GBInfo_N[28][x] = 1;
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
	if (changeMap_Boss == false)
	{
		COORD curPos = GetCurrentCursorPos();

		DeleteMT(MeteoInfo[0]);

		if (MT_pos.Y == 0 ) {

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
			Sleep(10);
			return 1;
	
		
		
	}
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
	else if (attacked && attacked_Boss)//보스 맵에서 보스에게 아팠을때
		nowLife -= 2;

	else if (attacked && attacked_Boss == false)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}


int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_B[31][31], char MeteoInfo[4][4])	//충돌 함수
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
					DeleteMT(MeteoInfo);
					MT_pos.Y = 1;
				} //충돌시 1로 돌아가서 int main문의 조건성립 -> y가 1일때 재생성

			}

			///전환맵
			if (PCInfo[x][y] == 1 && changeMap_Normal == true && Switch_N == true)	//pc가 1이고, 일반맵 맞고, 스위치는 맞음
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

			//보스 맵
			if (PCInfo[y][x] == 1 && changeMap_Normal == false && changeMap_Boss == true)//일반맵 아니고, 보스 맵은 맞음
			{
				if (GBInfo_B[arrY + y][arrX + x] == 1)	//벽이랑 부딪혔을때
				{
					return 0;
				}

				////if(GBInfo_B[arrY + y][arrX + x] == 'b' || GBInfo_B[arrY + y][arrX + x -1] == 'b' || GBInfo_B[arrY + y][arrX + x -2] == 'b' || GBInfo_B[arrY + y][arrX + x - 3] == 'b' || GBInfo_B[arrY + y][arrX + x - 4] == 'b')
				//if(GBInfo_B[arrY + y -1][arrX + x ] == 'b' || GBInfo_B[arrY + y -2][arrX + x ] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x-1] == 'b' || GBInfo_B[arrY + y - 2][arrX + x-1] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x - 2] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 2] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x - 3] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 3] == 'b' || 
				//	GBInfo_B[arrY + y - 1][arrX + x - 4] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 4] == 'b'  )	//보스랑 부딪혔을때	
				////if(BossInfo[arrY + y][arrX + x] == 1)
				//{
				//	//printf("보스");
				//	attacked = true;
				//	attacked_Boss = true;
				//	return 0;
				//}

				/*if (GBInfo_B[arrY + y][arrX + x] == 'm')	//거울이랑 부딪혔을때
				{
				printf("거울");
				//return 0;
				}*/

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


				//if (GBInfo_N[arrY + y][arrX + x] == 3)	//일반맵 스위치
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
				}

			}


		}
	}
	return 1;
}

int ShiftRight()
{
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0)//부딪힘
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
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4],MeteoInfo[0]) == 0)//부딪힘
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
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 || PC_pos.Y == 0)//부딪힘
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
	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == false)	//부딪힘	//일반맵 올라오는 벽	//스위치 X
	{
		PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//부딪힘	//전환맵 아래 벽//스위치 O
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//부딪힘	//전환맵 옆으로 다가오는 벽//스위치 O
	{
		PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == false && changeMap_Boss == true)//부딪힘		//보스맵	O	//GBInfo_B[Switch_B % 4]에서 Switch_B % 4는 보스맵 모델 번호임
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
	printf("일반맵 클리어");	//일단 출력
	Sleep(1000);

	changeMap_Normal = false;//일반맵 아님
	changeMap_Boss = true;//보스맵 맞음
	check_B = 0;//콘솔창 한번 지워주기 위해서 
	PC_pos.Y = 10;//PC위치 옮겨줌

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

	while (1)
	{

		if (changeMap_Normal == true && changeMap_Boss == false)
		{

			UpOB();

		}
		DrawMap_Switch();
		ObTime++;

		SetCurrentCursorPos(PC_pos.X, PC_pos.Y);
		drawPC(PCInfo[0]);

		Gravity_N();
		ProcessKeyInput();

		SetCurrentCursorPos(62, 1);
		printf("PC 체력: %3d", Physical(PCLife));

		SetCurrentCursorPos(62, 3);
		printf("PC : %3d, %3d", PC_pos.X, PC_pos.Y);
		SetCurrentCursorPos(62, 4);
		printf("Boss : %3d, %3d", Boss_pos.X, Boss_pos.Y);
		SetCurrentCursorPos(62, 5);
		printf("count: %3d", count);


		SetCurrentCursorPos(62, 7);
		printf("Boss : %3d, %3d", Mirr_pos[0].X, Mirr_pos[0].Y);
		SetCurrentCursorPos(62, 8);
		printf("Boss : %3d, %3d", Mirr_pos[1].X, Mirr_pos[1].Y);
		SetCurrentCursorPos(62, 9);
		printf("Boss : %3d, %3d", Mirr_pos[2].X, Mirr_pos[2].Y);
		SetCurrentCursorPos(62, 10);
		printf("Boss : %3d, %3d", Mirr_pos[3].X, Mirr_pos[3].Y);



		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (!Shoot_MT() && Switch_N==false) {
			MT_pos.X = (rand() % 20) * 2;
			MT_pos.Y = 28;
		}

		else if(!Shoot_MT() && Switch_N==true) {
			MT_pos.X = (rand() % 20);
			MT_pos.Y = 28*2;
		}
	


		if (changeMap_Normal == true && changeMap_Boss == false)	//일반맵 O	일때//보스맵 X
		{
			if (ObTime % 6 == 0)
				MakeOb();

			if (ObTime == 5)
			{
				GBInfo_N[10][3] = 3;
			}

			/*if (SkTime % 15 == 0)
				DrawSk();*/
		}

		if (changeMap_Normal == true && changeMap_Boss == false && ObTime == 50)	//5번째 줄일 때 클리어(일반) ->줄 수 바꿔야함
			isN_clear();

		if (changeMap_Normal == false && changeMap_Boss == true && check_B>0)	//레이저를 쏘쟛	//check_B는 보스맵을 다 그리고서 레이저를 쏘려고
		{
			ShootLaser();//레이저를 쏘는 함수 (Draw & Delete)
		}

	}

	getchar();
	return 0;
}