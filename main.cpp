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
COORD Mirr_pos[8] = { 0 };//�ݻ�� ��ġ
COORD Switch_pos[4] = { 0 };//����ġ ��ġ
COORD Boss_pos = { 0,0 }; //���� ��ġ
COORD Laser_pos = { 0,0 };	//������ ��ġ
COORD ReflectLaser_pos = { 0,0 };	//������ ��ġ



int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];

int speed_laser = 30;
int speed = 15;
int check = 0; // ����ġ�� delete
int PCLife = 30;	//PC�� ü��

int ObTime_o = 0;//�ö󰡴� ����	(��ֹ��� ��ֹ����� ����)
int ObTime_t = 3;//�ö󰡴� ����
				 //��ֹ��� �����ο��� �ι� �׸��� �ٲ㺽

int Check_Ob = 0;	//����� ���� ��������� X��ǥ
int Ran;	//����� ���� ����

int check_N;//��ȯ�ð� �˻�
int check_B;//������ �� ȭ�� �����
int starttime = 0;//��ȯ ���� �ð�

bool attacked = false;	//���ݹ޾Ҵ��� �˷��ִ� ����

bool Switch_N = false; //����ġ �浹 �˷��ִ� ����
int Switch_B = 0;//������ ����ġ ���� �� ���� ���� �������ִ� ���� (�� �� ���� 4��)

bool changeMap_Boss = false;//������ ��ȯ ��ȣ
bool changeMap_Normal = true;//�Ϲݸ� ��ȯ ��ȣ
bool attacked_Boss = false;	//�������� ���� �޾Ҵ��� �˷��ִ� ����

int L;//������ �� ��ȣ
bool reflect = false;//�ݻ�üũ ����
int BossLife = 1;//������ ü��

int checkStage = 1;	//���� Stage

int Mirr_num[4] = { 0 };//�̷��� ������ Ƚ�� ����
int Mirr_overheattime[4] = { -1 };//���� ���°� ���۵� �ð��� üũ�ϴ� ����
int B_time = 0;//�������������� ���²��

bool overheat[4] = { false };//�����ƴ��� 

bool clear_N = false; //�븻�� ������ ���� ȣ�� ����

int len; //������ ����
int count;//������ �̵� (ShootLaser�� �ִ� i�� �ٲ�)
int StoreBoard[B_GBOARD_HEIGHT][B_GBOARD_WIDTH];	//�������� ������ �ӽ÷� ����(�������� �� ���ؼ�)

bool ba = false;//��ũ �׽�Ʈ ��
bool gameover = false;// ��Ȧ�� ���ӿ��� ����
int StoreHeight;//������ �ӽ� �������� ����
bool attackLaserBoss = false;

int* Password;	//��ȣ(������) �迭
int PW_size;
int InputWord = 0;	//�Է�Ű
bool sticky = false;

int countBossAttack = 0;	//����ü�� Ƚ�� ��ȭ
int countPCAttack = 0;	//pc ü�� Ƚ�� ��ȭ

int Ro = 0; //������ ȸ������ ȸ�� ���� �� ����Ʈ ���Ӻ��� �ϱ� ���ؼ� ���
int move = 0; //������ ȸ������ ȸ�� ���� PC ��ǥ�� �����ϱ� ���ؼ� ���

int lefttime = 20;// �Ϲݸ��� ���� ���� ���߾��ϴ� �ð�

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

int DetectCollision_Meteo(int posX, int posY, char MeteoInfo[4][4])	//�� �������� ������ �� ���� �ҵ�
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


void DeleteLaser_B(char GBInfo_B[31][31])
{
	//15//14

	COORD curPos = GetCurrentCursorPos();

	//����
	for (int y = 16; y<B_GBOARD_HEIGHT; y++)
		for (int x = 0; x < y - 16; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			if (GBInfo_B[y][x] == 0)
			{
				printf(" ");
			}

		}

	SetCurrentCursorPos(curPos.X, curPos.Y);

	//������
	for (int y = 16; y<B_GBOARD_HEIGHT; y++)
		for (int x = 46 - y; x<B_GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			if (GBInfo_B[y][x] == 0)
			{
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
			if (changeMap_Normal == false && changeMap_Boss)//�Ϲݸ� �ƴϰ�, �������϶� ��ֹ� ������
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
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
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				if (y == 1) printf("��");
				if (y == 2)	printf("��");
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
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

//�ӽ� �������� ����� �Լ�
void DeleteStoreBoard()
{
	if (count == 0)

		for (int i = 0; i<B_GBOARD_HEIGHT; i++)
			for (int j = 0; j<B_GBOARD_WIDTH; j++)
				StoreBoard[i][j] = 0;	//�ӽ� �������� ����


}

//�ӽ� �����ǿ� ������ ����� �Լ�
void DeleteStoreLaser()
{
	int arrY;

	if (Switch_B % 2 == 0)	//���簢�� ��
	{
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//������ ��
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

//Boss�� �׸��� �Լ�	
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
				printf("��");
				StoreBoard[arrY + y][arrX + x] = 1;	//ũ������ �ȵǼ� �ӽ� ������ �ȿ� �־����
			}

			if (BossInfo[y][x] == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
				printf("��");
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			if (BossInfo[y][x] == 3)
				printf("��");
			if (BossInfo[y][x] == 4)
				printf("��");
			if (BossInfo[y][x] == 5)
				printf("��");

		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//Boss�� �����ִ� �Լ�
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

int DetectCollision_Laser(int posX, int posY, char LaserInfo[5][20], char GBInfo_B[31][31])//�������� �ݻ���̶� �ε����� �Լ�
{
	int x, y;
	int arrX = posX / 2;
	int arrY = posY + 1;

	int nonotime = 10;//��� ���ⲫ��
	int mindex = -1;

	for (x = 0; x < 20; x++)
		for (y = 0; y < 5; y++) 
		{
			if (LaserInfo[y][x] == 1)
			{
				SetCurrentCursorPos(62, 15);
				//printf("�浹");
				switch (GBInfo_B[arrY + y][arrX + x])
				{
					//1
				case 'u':
					mindex = 0;
					Mirr_num[mindex] += 1;
					SetCurrentCursorPos(62, 13);
					printf("mirru %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10�ʰ� ���� �� �ʱ�ȭ
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
					printf("mirri %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10�ʰ� ���� �� �ʱ�ȭ
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
					printf("mirro %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10�ʰ� ���� �� �ʱ�ȭ
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
					printf("mirrp %d  %d", Mirr_num[mindex], Mirr_overheattime[mindex]);

					if (Mirr_num[mindex] == 4) Mirr_overheattime[mindex] = B_time;
					else if (Mirr_num[mindex] > 4)
					{
						overheat[mindex] = true;
						if (Mirr_overheattime[mindex]>0 && B_time - Mirr_overheattime[mindex] > nonotime)//10�ʰ� ���� �� �ʱ�ȭ
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
				default:
					break;

				}

			}
		}

	for (int nm = 0; nm < 4; nm++)
	{
		mindex = nm;
		SetCurrentCursorPos(62, 19+nm);
		printf("mirr %d %d  %d\n",nm, Mirr_num[mindex], Mirr_overheattime[mindex]);
		
		if (Mirr_overheattime[mindex] > 0 && B_time - Mirr_overheattime[mindex] > nonotime)//10�ʰ� ���� �� �ʱ�ȭ
		{
			Mirr_overheattime[mindex] = -1;
			Mirr_num[mindex] = 0;
			overheat[mindex] = false;
			return 1;
		}
	}
	return 0;

}

int ddd = 0;
int DetectCollision_Boss(int posX, int posY, char LaserInfo[5][20])	//(int posX, int posY, char LaserInfo[5][20], char GBInfo_B[31][31])//������ �ݻ� �������� �ε����� �Լ�
{
	int x, y;

	int arrX = posX / 2;
	int arrY = posY;

	for (x = 0; x<20; x++)
		for (y = 0; y < 5; y++)
		{
			if (LaserInfo[y][x] == 2)	//�ݻ� ������ �浹
			{
				if (StoreBoard[arrY + y][arrX + x] == 1)
				{
					return 1;
				}
			}
		}

	return 0;
}

//Laser�� �׸��� �Լ�
void DrawLaser(char LaserInfo[5][20])
{
	int x, y;
	int arrX;
	int arrY;

	if (Switch_B % 2 == 0)	//���簢�� ��
	{
		arrX = (Boss_pos.X - 6) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//������ ��
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

				printf("��");
				StoreBoard[arrY + y][arrX + x] = 2;	//ũ������ �ȵǼ� ���� ������ �ȿ� �־����

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (LaserInfo[y][x] == 2)
			{
				printf("��");
				StoreBoard[arrY + y][arrX + x] = 3;	//ũ������ �ȵǼ� ���� ������ �ȿ� �־����
			}


		}
	SetCurrentCursorPos(curPos.X, curPos.Y);


	//DeleteStoreLaser();
}

//Laser�� ����� �Լ�
void DeleteLaser(char LaserInfo[5][20])
{
	int x, y;
	int arrX;
	int arrY;

	if (Switch_B % 2 == 0)	//���簢�� ��
	{
		arrX = (Boss_pos.X - 6) / 2;
		arrY = Boss_pos.Y + len + 3 - 1 - count + 1 + 4;
	}
	if (Switch_B % 2 == 1)	//������ ��
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
				StoreBoard[arrY + y][arrX + x] = 0;	//ũ������ �ȵǼ� ���� ������ �ȿ� �־����
				printf(" ");
			}

			if (LaserInfo[y][x] == 2)
			{
				printf(" ");
				StoreBoard[arrY + y][arrX + x] = 0;	//ũ������ �ȵǼ� ���� ������ �ȿ� �־����
			}

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deleteGB_B() //������ ����� �Լ�
{
	for (int y = 0; y < B_GBOARD_HEIGHT; y++)
	{
		for (int x = 0; x < B_GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			printf("��");
		}
	}
}

//�������� �׸��� �Լ�
void drawGB_B(char GBInfo_B[31][31])
{
	int x, y, mnb = 0, snb = 0;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0; y<B_GBOARD_HEIGHT; y++)
		for (x = 0; x < B_GBOARD_WIDTH; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			//���׸���
			if (GBInfo_B[y][x] == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				printf("��");
			}

			//�ݻ��׸���
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

			//����ġ�׸���
			if (GBInfo_B[y][x] == 's')
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				printf("��");

				Switch_pos[snb].X = curPos.X;
				Switch_pos[snb].Y = curPos.Y;
				snb++;
			}

			//Boss�׸���
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

void Rotate_BossMap() //�� �����ִ� �Լ�
{
	int x, y, mn = 0, sn = 0;
	COORD curPos = GetCurrentCursorPos();


	if ((Switch_B % 4) == 1) // 45�� �̵�
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

				//���׸���
				if (GBInfo_B[1][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("��");
				}

				//�ݻ��׸���
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

				//����ġ�׸���
				if (GBInfo_B[1][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("��");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss�׸���
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

	else if ((Switch_B % 4) == 2) // 90�� �̵�
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

				//���׸���
				if (GBInfo_B[2][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("��");
				}

				//�ݻ��׸���
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

				//����ġ�׸���
				if (GBInfo_B[2][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("��");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss�׸���
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


	else if ((Switch_B % 4) == 3) // 135�� �̵�
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

				//���׸���
				if (GBInfo_B[3][y][x] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					printf("��");
				}

				//�ݻ��׸���
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

				//����ġ�׸���
				if (GBInfo_B[3][y][x] == 's')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					printf("��");

					Switch_pos[sn].X = curPos.X;
					Switch_pos[sn].Y = curPos.Y;
					sn++;
				}

				//Boss�׸���
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

	else if ((Switch_B % 4) == 0) //180�� ���ʹ� �ٽ� ó������!
	{
		deleteGB_B();
		drawGB_B(GBInfo_B[0]);
	}
}

void deleteGB_N() //�Ϲݸ� ����� �Լ�
{

	for (int y = 0; y < 31; y++)
	{
		for (int x = 0; x < 31; x++)
		{
			SetCurrentCursorPos((x * 2), y);

			printf("��");
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
} // ���������� �ö󰥶� ���� ���Ӻ��� �ʱ�ȭ


void DrawStart()
{
	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
	{
		GBInfo_N[18][x + 4] = 1;
		GBInfo_N[24][x + 7] = 1;
	} //ó�� ���۽� ȭ�鿡 ������ �������� �׷��ִ� ��, ���ۺ��� ��ֹ� ä��� ����!
}

void UpOB()	//���� ������ ���� ���ݸ��� ���� �÷��ִ� �Լ� 
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT - 1; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH * sizeof(int));//�����Լ�,������ �� ���̵� ���
		for (x = 0; x < GBOARD_WIDTH; x++)
			GBInfo_N[GBOARD_HEIGHT - 1][x] = 0;
	}
}

void MakeOb_one()	//���� ������ GBInfo_N�� �������ִ� �Լ� 
{
	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 10); // 1~23    

	if (checkStage == 1)
	{
		Ran = (rand() % 3) + 4;
	}
		
	else if (checkStage != 1)
	{
		Ran = (rand() % 8) + 4;
	}

	for (int x = Check_Ob; x < Check_Ob + Ran; x++)
		GBInfo_N[27][x] = 1;
}

void MakeOb_two()	//���� ������ GBInfo_N�� �������ִ� �Լ� 
{
	srand((unsigned int)time(NULL));
	Check_Ob = (rand() % 20) + 10; // 1~23   

	if (checkStage == 1)
	{	
		Ran = (rand() % 3) + 4;
	}

	else if (checkStage != 1)
	{
		Ran = (rand() % 8) + 4;
	}

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
				printf("��");
			}

			if (MeteoInfo[y][x] == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				printf("��");
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
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
			if (Game[y][x] == 1)
			{
				printf("��");

			}
		}

	SetCurrentCursorPos(curPos.X, curPos.Y);
}


void isB_Clear()//Ŭ����(����)
{
	SetCurrentCursorPos(30, 10);
	printf("Stage %d ������ Ŭ����", checkStage);	//�ϴ� ���

	Sleep(1000);

	if (checkStage == 4)	//4ź Ŭ����
	{
		SetCurrentCursorPos(30, 11);
		printf("Stage %d Ŭ����", checkStage);	//�ϴ� ���
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
		printf("Stage %d Ŭ����", checkStage);	//�ϴ� ���
		Sleep(1000);

		SetCurrentCursorPos(30, 12);
		printf("���� Stage ���...!");
		Sleep(1000);

		deleteGB_B();	//������ �����

		checkStage++;	//ź �� �ø���

		countBossAttack = 0;
		countPCAttack = 0;

		//����ü��
		if (checkStage == 1)
		{
			BossLife = 1;
			lefttime = 1 * 20;
		}		
		if (checkStage == 2)
		{
			BossLife = 2;
			lefttime = 2 * 20;
		}
		if (checkStage == 3)
		{
			BossLife = 3;
			lefttime = 3 * 20;
		}
		if (checkStage == 4)
		{
			BossLife = 4;
			lefttime = 4 * 20;
		}


		changeMap_Boss = false;	//������ ����
		changeMap_Normal = true;	//�Ϲݸ� �ѱ�

		PC_pos.X = 10;
		PC_pos.Y = 10;

		ObTime_o = 0;
		ObTime_t = 0;
		clear_N = false;
		Switch_B = 0;
		for (int i = 0; i < 4; i++)
		{
			Mirr_num[i] =  0 ;//�̷��� ������ Ƚ�� ����
			Mirr_overheattime[i] = -1 ;//���� ���°� ���۵� �ð��� üũ�ϴ� ����
			overheat[i] = false;
		}
		B_time = 0;//�������������� ���²��

		

		DeleteStage();


	}

}

int Physical_PC(int maxLife)	//ü���Լ�(ĳ������ �ִ� ü���� �޾Ƽ� ���� ü���� ����)
{
	int nowLife = maxLife - countPCAttack;

	if (nowLife < 0)	//�����϶�
		nowLife = 0;

	if (gameover) nowLife = 0;
	if (nowLife == 0)	//ü���� 0�϶� game over
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
	else if (attacked && attacked_Boss)//���� �ʿ��� �������� ��������
	{
		nowLife -= 2;
		countPCAttack += 2;
	}


	else if (attacked && attacked_Boss == false)	//��������
	{
		nowLife--;
		countPCAttack++;
	}

	attacked_Boss = false;
	attacked = false;	//�ٽ� attacked�� false (������)�� �����ش�.
	return nowLife;	//���� ü���� �����Ѵ�.

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

	//��ũ ���ڰ� ����ϱ�� �ð��Ǹ� ����~
	if (B_time < S)
	{
		COORD curPos = GetCurrentCursorPos();
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
			{
				SetCurrentCursorPos(10 + (x * 2), 3 + y);
				{
					if (rank[0][y][x] == 1)
						printf("��");
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
						printf("��");
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
						printf("��");
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
						printf("��");
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
						printf("��");
				}
			}
		SetCurrentCursorPos(22, 7);
		printf("Rank");
	}

	Sleep(1000);
}

int Physical_Boss(int maxLife)	//ü���Լ�(ĳ������ �ִ� ü���� �޾Ƽ� ���� ü���� ����)
{
	int nowLife = maxLife - countBossAttack;

	if (nowLife < 0)	//�����϶�
		nowLife = 0;

	if (nowLife == 0)	//ü���� 0�϶� game over
	{
		Rank();
		isB_Clear();
	}

	else if (attackLaserBoss)	//��������
	{
		nowLife--;
		countBossAttack++;
	}

	attackLaserBoss = false;	//�ٽ� attacked�� false (������)�� �����ش�.

	return nowLife;	//���� ü���� �����Ѵ�.

}

void isN_clear()//Ŭ����(�Ϲ�)
{
	for (int y = 0; y < 30; y++)
		for (int x = 0; x < 80; x++)
		{
			SetCurrentCursorPos(x, y);
			printf(" ");
		}

	SetCurrentCursorPos(30, 15);
	printf("Stage %d �Ϲݸ� Ŭ����", checkStage);	//�ϴ� ���
	Sleep(1000);

	check_B = 0;//�ܼ�â �ѹ� �����ֱ� ���ؼ� 
	deleteGB_B();	//stage ���� ���������� ������� 
	changeMap_Normal = false;//�Ϲݸ� �ƴ�
	Switch_N = false;
	changeMap_Boss = true;//������ ����
	PC_pos.Y = 20;//PC��ġ �Ű���

	Physical_Boss(BossLife);	// ź���� ���� ü�� �ٸ��� ����

	for (int i = 0; i < 4; i++)
	{
		Mirr_num[i] = 0;//�̷��� ������ Ƚ�� ����
		Mirr_overheattime[i] = -1;//���� ���°� ���۵� �ð��� üũ�ϴ� ����
	}
	B_time = 0;//�������������� ���²��
//	reflect = false;

}



int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_B[B_GBOARD_HEIGHT][B_GBOARD_WIDTH])	//�浹 �Լ�
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;

	if (Switch_N)	//�Ϲݸʿ��� ����ġ �϶�
	{
		arrX = posY + 1;
		arrY = posX / 2 - 1;
	}
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			///�Ϲݸ�
			if (PCInfo[y][x] == 1 && changeMap_Normal == true && Switch_N == false)	//pc�� 1�̰�, �Ϲݸ� �°�, ����ġ�� �ƴ�
			{
				if (GBInfo_N[arrY + y][arrX + x] == 1)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}

				if (GBInfo_N[arrY + y][arrX + x] == 2)	//���̶� �ε�������
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 5)	//����
					isN_clear();

				if (GBInfo_N[arrY + y][arrX + x] == 4)	//������
				{
					sticky = true;
					//DrawPassword();	//��ȣ ����
					//DeletePassword();	//��ȣ ����

				}
				if (GBInfo_N[arrY + y][arrX + x] == 3)	//�Ϲݸ� ����ġ
				{
					Switch_N = true;
					deletePC(PCInfo);
					//PC_pos.Y = 13;
					MT_pos.X = 13;
					//MT_pos.Y = 3;
					return 0;
				}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
				{
					attacked = true;
					DeleteMT(MeteoInfo[0]);
					MT_pos.Y = 0;
				} //�浹�� 1�� ���ư��� int main���� ���Ǽ��� -> y�� 1�϶� �����


			}

			///��ȯ��
			if (PCInfo[x][y] == 1 && changeMap_Normal == true && Switch_N == true)	//pc�� 1�̰�, �Ϲݸ� �°�, ����ġ�� ����
			{

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 2)	//���̶� �ε�������
					return 0;

				if (GBInfo_N[arrY + y + 1][arrX + x] == 1)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}

				if (GBInfo_N[arrY + y + 2][arrX + x - 1] == 5)	//����
					isN_clear();

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 4)	//������
					sticky = true;


				if (((PC_pos.Y == MT_pos.X) || (PC_pos.Y + 1 == MT_pos.X)) && ((PC_pos.X + 3 == MT_pos.Y) || (PC_pos.X + 2 == MT_pos.Y) || (PC_pos.X + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
				{
					attacked = true;
					DeleteMT(MeteoInfo[0]);
					MT_pos.Y = 0;;
				}

			}

			//���� ��
			if (PCInfo[y][x] == 1 && changeMap_Normal == false && changeMap_Boss == true)//�Ϲݸ� �ƴϰ�, ���� ���� ����
			{
				if (GBInfo_B[arrY + y][arrX + x] == 1)	//���̶� �ε�������
				{
					return 0;
				}

				if (GBInfo_B[arrY + y][arrX + x] == 'm')	//�ſ��̶� �ε�������
				{
					//printf("�ſ�");
					return 0;
				}

				if (GBInfo_B[arrY + y][arrX + x] == 's')	//����ġ�� �ε�������
				{
					Switch_B++;
					check_B = 0;
					DeleteStoreBoard();	//�ӽø� �ʱ�ȭ

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

				if (StoreBoard[arrY + y][arrX + x] == 1)	//������ �浹 ( ������ �ӽ� �ʿ� �־������)
				{
					//printf("����");
					attacked_Boss = true;
					attacked = true;
					return 0;
				}

				if (StoreBoard[arrY + y][arrX + x] == 2)	//������ �浹 ( �������� �ӽ� �ʿ� �־������)
				{
					//printf("������");
					attacked = true;
					//return 0;
				}

				if ((PC_pos.Y == MT_pos.X) && ((PC_pos.X + 3 == MT_pos.Y) || (PC_pos.X + 2 == MT_pos.Y) || (PC_pos.X + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
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
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0)//�ε���
	{
		if (changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//��ȯ�ʿ��� �ε���(�и�)
		{
			PC_pos.X -= 2;
			Sleep(speed);
		}
		return 0;
	}
	if (changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//��ȯ�� ��Ȧ
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
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0)//�ε���
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
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 || PC_pos.Y == 0)//�ε���
		return 0;
	else if (PC_pos.Y - 1 < 0)//�Ϲݸ� �̵�����
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

	if (changeMap_Boss == false && changeMap_Normal == true && PC_pos.Y + 2 + 1 > GBOARD_HEIGHT)//��Ȧ
	{
		gameover = true;
		Physical_PC(30);
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == false)	//�ε���	//�Ϲݸ� �ö���� ��	//����ġ X	
	{
		if (PC_pos.Y > 0)//���� ���Ѿ��
			PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//�ε���	//��ȯ�� �Ʒ� ��//����ġ O
		return 0;

	if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//�ε���	//��ȯ�� ������ �ٰ����� ��//����ġ O
	{
		if (PC_pos.X - 2 > 0)//�̵�����
			PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4]) == 0 && changeMap_Normal == false && changeMap_Boss == true)//�ε���		//������	O	//GBInfo_B[Switch_B % 4]���� Switch_B % 4�� ������ �� ��ȣ��
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
			case '1':	//�߰�
				Switch_N = true;
				break;
			case '2': //�� �߰�
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

				//������ �Ϸ���
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
		if (changeMap_Boss)
			Sleep(2);
		else
			Sleep(speed);
		//Sleep(speed);
	}
}


void DrawPassword()	//������(��ȣ����)
{
	//Stage�� ���� ��ȣ���� ����
	if (checkStage == 1)
		PW_size = 1;
	if (checkStage == 2)
		PW_size = 3;
	if (checkStage == 3)
		PW_size = 5;
	if (checkStage == 4)
		PW_size = 7;

	Password = (int*)malloc(sizeof(int)*PW_size);	//Password(��ȣ�迭)�� PW_size��ŭ ����

	srand(time(NULL));

	int word = 0;	//��ȣ��

	for (int i = 0; i< PW_size; i++)
	{
		int W = rand() % 5;	//��ȣ ����

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

		*(Password + i) = word;	//��ȣ���� Password(��ȣ�迭)�� ����

	}

}

void DeletePassword()	//������(��ȣ����)
{
	int i = 0;
	int j = 0;

	while (i<PW_size)
	{
		SetCurrentCursorPos(20, 19);
		printf("%3d", ++j);	//���° Ű�������� �˱� ���ؼ�
		SetCurrentCursorPos(20, 20);
		printf("                                             ");	//���� Ű���� �Է� ��� �����

		SetCurrentCursorPos(20, 20);
		if (*(Password + i) == LEFT)
			printf("Left��  ");
		if (*(Password + i) == RIGHT)
			printf("Right��  ");
		if (*(Password + i) == UP)
			printf("Up��  ");
		if (*(Password + i) == DOWN)
			printf("Down��  ");
		if (*(Password + i) == SPACE)
			printf("Space bar  ");

		while (1)	//���� ���� �Է��Ҷ����� ���ѷ���
		{
			InputWord = 0;	//Ű���尪 �ʱ�ȭ

			ProcessKeyInput();

			if (*(Password + i) == InputWord)
				break;
		}

		i++;
	}

	free(Password);	//Password ����
	printf("Good!");
	sticky = false;
}

void DrawAllMap()	//��� ���� �׸��� �Լ� - ����ġ�� ��ȭ�� ���� ��ȭ���� �׷���
{
	int x, y;
	int reversetime = 30;

	if (Switch_N && changeMap_Normal)//�Ϲݸʿ��� ����ġ�� �ǵ鿴����
	{
		if (sticky)	//������ �϶�
		{
			deleteGB_N();	//������
			DeleteOb();
			deletePC(PCInfo[0]);


			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
			DrawPassword();
			DeletePassword();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}


		if (check_N == 0)//����ġ ó�� �������� ������
		{
			int tempx = PC_pos.X;
			int tempy = PC_pos.Y;
			deleteGB_N();
			MT_pos.X == 28;
			MT_pos.Y == 15;
			if (PC_pos.X>40)
				PC_pos.X = 20;
			else
				PC_pos.X = tempy * 2;

			if (PC_pos.Y>15)
				PC_pos.Y = tempx / 2 + 4;
			else
				PC_pos.Y = tempx / 2 - 2;
		}
		check_N++;

		if (check_N == reversetime)//10�� �ڿ� �ٽ� ����
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

	if (changeMap_Boss)	//������ �׸���
	{
		if (check_B == 0)//����ġ ó�� �������� ������
		{
			DeleteOb();
		}

		check_B++;

		SetCurrentCursorPos(62, 10);
		printf("map: %3d", Switch_B % 4);

		if ((Switch_B % 4) == 0)//������ �� 4���߿� 0��° �� �׸���
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
			if (Switch_B % 2 == 1)
				DeleteLaser_B(GBInfo_B[Switch_B % 4]);
			Rotate_BossMap();
		}


	}
	else if (changeMap_Normal) 	//�Ϲݸ� �׸���
	{

		if (sticky)	//������ �϶�
		{
			deleteGB_N();	//������
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
						printf("��");
					}

					if (GBInfo_N[y][x] == 2)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
						printf("��");
					}

					if (GBInfo_N[y][x] == 3)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
						printf("��");
					}

					if (GBInfo_N[y][x] == 4)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
						printf("��");
					}

					if (GBInfo_N[y][x] == 5)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
						printf("��");
					}

					else
						printf("��");

				}

		}

	}

}

//Laser�� ��� �Լ� (Draw & Delete) 
void ShootLaser()
{
	if (Switch_B % 2 == 0)	//���簢�� ��
	{
		len = 17;

		for (count = 0; count<len; count++)
		{
			DrawAllMap();	//�ʱ׸���
			DeleteStoreLaser();	//������ �����(���پ�)
			ProcessKeyInput();
			isCrash(PC_pos.X, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]);
			SetCurrentCursorPos(62, 0);
			printf("PC ü��: %3d", Physical_PC(PCLife));

			if (count == 0)	//storeBoard ��ü �����ϰ� ���� �׸���
			{
				DeleteStoreBoard();	//�ӽ� ������ ��ü �����
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);
				DrawBoss(BossInfo[0]);
			}

			if (count % 2 == 0)
				Gravity_N();

			if (reflect)// �ݻ緹���� ��� 
			{
				if (count != 0)
				{
					SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 1 + 4 - 1);

					if (Switch_B % 4 == 0)	//0�� ��
						DeleteLaser(LaserInfo[14]);

					if (Switch_B % 4 == 2)	//2�� ��
						DeleteLaser(LaserInfo[16]);
				}

				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 4 - 1);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				if (Switch_B % 4 == 0)	//0�� ��
					DrawLaser(LaserInfo[14]);	//�ݻ緹���� 
				if (Switch_B % 4 == 2)	//2�� ��
					DrawLaser(LaserInfo[16]);	//�ݻ緹���� 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (count != 0)//÷�� ������ ���ﲨ ����
			{
				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + 3 + count - 1 + 4);
				DeleteLaser(LaserInfo[L]);	//������ ����
			}
			SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + 3 + count + 4);
			DrawLaser(LaserInfo[L]);	//������ ���	

			if (Switch_B % 4 == 0)	//0�� ��
				if (DetectCollision_Boss(Boss_pos.X - 6, Boss_pos.Y + len + 5 - count, LaserInfo[14]) && (count == len - 1) && reflect == true)	//���� �ƾ�
				{
					attackLaserBoss = true;
				}

			if (Switch_B % 4 == 2)	//2�� ��
				if (DetectCollision_Boss(Boss_pos.X - 6, Boss_pos.Y + len + 5 - count, LaserInfo[16]) && (count == len - 1) && reflect == true)	//���� �ƾ�
				{
					attackLaserBoss = true;
				}


			if (count == len - 1)	//�ʱ�ȭ
			{
				DeleteLaser(LaserInfo[L]);


				SetCurrentCursorPos(Boss_pos.X - 6, Boss_pos.Y + len + 3 - 1 - count + 4);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				if (Switch_B % 4 == 0)	//0�� ��
					DeleteLaser(LaserInfo[14]);
				if (Switch_B % 4 == 2)	//2�� ��
					DeleteLaser(LaserInfo[16]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X - 6, Boss_pos.Y + 3 + count + 4, LaserInfo[L], GBInfo_B[Switch_B % 4]))//���� �������� �ݻ� �������� �׷���
				reflect = true;

			if (count == len - 1)	//�ʱ�ȭ
			{
				L = (rand() % 4) + 1;
				//L = (++L % 14);//�׽�Ʈ��
				//L = (++L % 2);
			}

			Sleep(speed_laser);

		}



	}

	if (Switch_B % 2 == 1)	//������ ��
	{
		len = 15;
		for (count = 0; count<len; count++)
		{
			DeleteLaser_B(GBInfo_B[Switch_B % 4]);

			DrawAllMap();	//�ʱ׸���
			DeleteStoreLaser();	//������ �����(���پ�)
			ProcessKeyInput();
			isCrash(PC_pos.X, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4]);
			SetCurrentCursorPos(62, 0);
			printf("PC ü��: %3d", Physical_PC(PCLife));

			if (count == 0)	//storeBoard ��ü �����ϰ� ���� �׸���
			{
				DeleteStoreBoard();	//�ӽ� ������ ��ü �����
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y);
				DrawBoss(BossInfo[1]);
			}

			if (count % 4 == 0)
				Gravity_N();

			if (reflect)// �ݻ緹���� ��� 
			{
				if (count != 0)
				{
					SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 1 + 2 - 1);
					DeleteLaser(LaserInfo[15]);
				}
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 2 - 1);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				DrawLaser(LaserInfo[15]);	//�ݻ緹���� 
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}

			if (count != 0)//÷�� ������ ���ﲨ ����
			{
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + 3 + count - 1 + 2);
				DeleteLaser(LaserInfo[L]);	//������ ����
			}
			SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + 3 + count + 2);
			DrawLaser(LaserInfo[L]);	//������ ���		

			if (DetectCollision_Boss(Boss_pos.X - 10, Boss_pos.Y + len + 3 - count, LaserInfo[14]) && (count == len - 1) && reflect == true)	//���� �ƾ�
			{
				attackLaserBoss = true;
			}

			if (count == len - 1)	//�ʱ�ȭ
			{
				DeleteLaser(LaserInfo[L]);
				SetCurrentCursorPos(Boss_pos.X - 10, Boss_pos.Y + len + 3 - 1 - count + 2);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				DeleteLaser(LaserInfo[15]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

				reflect = false;
			}
			if (DetectCollision_Laser(Boss_pos.X - 10, Boss_pos.Y + 3 + count + 2, LaserInfo[L], GBInfo_B[Switch_B % 4]))//���� �������� �ݻ� �������� �׷���
				reflect = true;
			if (count == len - 1)	//�ʱ�ȭ
			{
				L = (rand() % 4) + 1;
				//L = (++L % 14);//�׽�Ʈ��
			}
			Sleep(speed_laser);
		}

	}
}


int Shoot_MT() //showMT�� ��Ȱ ���׿� �������ִ� ��
{
	if (changeMap_Boss == false && clear_N == false)
	{
		COORD curPos = GetCurrentCursorPos();

		if (MT_pos.Y == 0) {

			return 0;
		} //y�� 1�϶� ���׿��� �ٽ� �Ʒ����� �׷��ֱ�

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

	GBInfo_N[GBOARD_HEIGHT - 2][12] = 5;//������ ���?
}

void DrawSwitch_N()
{
	srand((unsigned int)time(NULL));
	int Ran_Switch = (rand() % 22) +1 ;

	if (checkStage == 1 && ObTime_o % 10 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][Ran_Switch] = 3;
	}

	else if(checkStage != 1 && ObTime_o % 18 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][Ran_Switch] = 3;
	}
}

void DrawSk()
{
	if (ObTime_t % 10 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 3][1] = 4;
		GBInfo_N[GBOARD_HEIGHT - 4][1] = 4;

	}


	else if (ObTime_t % 22 == 0)
	{
		GBInfo_N[GBOARD_HEIGHT - 2][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 3][GBOARD_WIDTH - 2] = 4;
		GBInfo_N[GBOARD_HEIGHT - 4][GBOARD_WIDTH - 2] = 4;

	}

}

void printstory()
{
	SetCurrentCursorPos(2, 10);
	printf("���ָ� Ž���ϴ� ���ΰ�,\n");
	Sleep(1000);
	SetCurrentCursorPos(2, 11);
	printf("Ž�絵�� ��Ȧ�� ���� ������ �� �� ���� ���� ���ǰ� ����.\n");
	Sleep(2000);
	SetCurrentCursorPos(2, 12);
	printf("�� ���� ����� ���ؼ��� �ڽ��� ���ּ��� ã�ƾ� �Ѵ�!\n");
	Sleep(2000);
	SetCurrentCursorPos(2, 13);
	printf("������ �� �̻��� ������ �� �� ���� ���� ��Ģ�� �ۿ��ϰ�, \n");
	Sleep(1000);
	SetCurrentCursorPos(2, 14);
	printf("�Ŵ��� ������� ���� ���θ��� �ִ�.\n");
	Sleep(2000);
	SetCurrentCursorPos(2, 15);
	printf("���ΰ��� ������ ���ּ��� �����Ͽ� ��Ƴ��� �� ���� ���ΰ�!!\n");
	Sleep(2000);
	SetCurrentCursorPos(2, 20);
	printf("�̰��� ���������� �ʹٸ� ��Ȧ�� �������� �ʵ��� �����Ͽ���......\n");
	//Sleep(5000);
	SetCurrentCursorPos(20, 30);
	system("PAUSE");
	for (int is = 0; is<100; is++)
		for (int js = 0; js < 40; js++)
		{
			SetCurrentCursorPos(is, js);
			printf(" ");

		}

}

int main(void)
{

	//printstory();
	srand((unsigned int)time(NULL));
	L = (rand() % 4) + 1;	//������ �� ����

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
		printf("PC ü��: %3d", Physical_PC(PCLife));
		SetCurrentCursorPos(62, 1);
		printf("Boss ü��: %3d", Physical_Boss(BossLife));


		//SetCurrentCursorPos(62, 3);
		//printf("PC : %3d, %3d", PC_pos.X, PC_pos.Y);
		SetCurrentCursorPos(62, 4);
		//printf("Boss : %3d, %3d", Boss_pos.X, Boss_pos.Y);
		//SetCurrentCursorPos(62, 5);
		//printf("MT: %3d, %3d", MT_pos.X, MT_pos.Y);


		//SetCurrentCursorPos(62, 7);
		//printf("checktime %d %d", ObTime_o, ObTime_t);




		SetCurrentCursorPos(MT_pos.X, MT_pos.Y);

		if (!Shoot_MT() && Switch_N == false) {
			MT_pos.X = (rand() % 20) * 2 + 2;
			MT_pos.Y = 26;
		}

		else if (!Shoot_MT() && Switch_N == true) {
			MT_pos.X = (rand() % 18) + 2;
			MT_pos.Y = 26 * 2;
		}



		if (changeMap_Normal == true && changeMap_Boss == false && clear_N == false)	//�Ϲݸ� O	�϶�//������ X
		{
			if (ObTime_o % 9 == 0) //�������� ���� ���ǹ�
				MakeOb_one();

			if (ObTime_t % 4 == 0)
				MakeOb_two();


			DrawSk();
			DrawSwitch_N();

			/*if (SkTime % 15 == 0)
			DrawSk();*/
		}
		if (changeMap_Normal == true)
		{
			SetCurrentCursorPos(62, 3);
			if (lefttime - ObTime_o < 0)
				printf("��߿� �����ϼ���!", lefttime - ObTime_o);
			else
				printf("�����ð� : %d     ", lefttime - ObTime_o);
		}
		else
		{
			SetCurrentCursorPos(62, 3);
			printf("           ", lefttime - ObTime_o);
		}

		if (changeMap_Normal == true && changeMap_Boss == false && ObTime_o == lefttime)
		{
			clear_N = true;
			DrawClear_N();

			//isN_clear();
		}//50��° ���� �� Ŭ����(�Ϲ�)
		

		if (changeMap_Normal == false && changeMap_Boss == true && check_B>0)	//�������� ��Y	//check_B�� �������� �� �׸��� �������� �����
		{
			ShootLaser();//�������� ��� �Լ� (Draw & Delete)

			B_time++;//������ ��� �ð� ������Ű��


		}

		SetCurrentCursorPos(62, 12);
		printf("Boss time %3d", B_time);
		//Sleep(200);
	}

	getchar();
	return 0;
}
