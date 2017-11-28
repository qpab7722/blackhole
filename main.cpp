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
COORD Mirr_pos[4] = { 0 };//�ݻ�� ��ġ
COORD Switch_pos[4] = { 0 };//����ġ ��ġ
COORD Boss_pos = { 0,0 }; //���� ��ġ

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];


int speed = 30;
int check = 0; // ����ġ�� delete
int PCLife = 30;	//PC�� ü��

int ObTime = 0;	//�ö󰡴� ����	(��ֹ��� ��ֹ����� ����)
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
int count = 0; // ������ ���� ����
int L;//������ �� ��ȣ
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

int DetectCollision_MTPC(int posX, int posY, char MeteoInfo[4][4], char PCInfo[4][4])//���׿��� PC�浹 �˻� �Լ�
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


int DetectCollision_Laser(int posX, int posY, char LaserInfo[5][5], char GBInfo_B[31][31])//�������� �ݻ���̶� �ε����� �Լ�
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
					printf("�ݻ�");
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
				if (y == 1) printf("��");
				if (y == 2)	printf("��");
			}
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


//Boss�� �׸��� �Լ�
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
				printf("��");
			if (BossInfo[y][x] == 3)
				printf("<");
			if (BossInfo[y][x] == 4)
				printf(">");
			if (BossInfo[y][x] == 5)
				printf("��");
			if (BossInfo[y][x] == 6)
				printf("��");
		}
	}
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


//Laser�� �׸��� �Լ�
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
				printf("��");
			}

		}

	}
	SetCurrentCursorPos(curPos.X, curPos.Y);

}

//Laser�� ����� �Լ�
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

//Laser�� ��´� �Լ� (Draw & Delete) 
void ShootLaser()
{
	if (Switch_B % 2 == 0)	//���簢�� ��
	{
		for (int i = 0; i<22; i++)
		{
			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + count, LaserInfo[L], GBInfo_B[Switch_B % 4]))
				reflect = true;

			if (reflect&&count != 21)
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 25 - count);
				DrawLaser_B(LaserInfo[5]);	//�ݻ緹���� ���
				if (count == 22)
					reflect = false;
				Sleep(50);
				DeleteLaser_B(LaserInfo[5]);
			}

			SetCurrentCursorPos(62, 12);
			printf("Boss : %3d, %3d", Boss_pos.X, Boss_pos.Y + 3 + count);
			count++;
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + count);
			DrawLaser_B(LaserInfo[L]);	//������ ���
			if (reflect)
				Sleep(50);
			else
				Sleep(100);
			DeleteLaser_B(LaserInfo[L]);	//������ ����
		}
		if (count == 22)	//20��°�� �Ǹ� �׸� ��� �ٽ� ����
		{
			count = 0;
			L = (rand() % 4) + 1;
		}


	}

	if (Switch_B % 2 == 1)	//������ ��
	{
		for (int i = 0; i<16; i++)
		{

			if (DetectCollision_Laser(Boss_pos.X, Boss_pos.Y + 3 + count, LaserInfo[L], GBInfo_B[Switch_B % 4]))
				reflect = true;

			if (reflect && count != 15)
			{
				SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + 16 - count);
				DrawLaser_B(LaserInfo[5]);	//�ݻ緹���� ���
				if (count == 16)
					reflect = false;
				Sleep(50);
				DeleteLaser_B(LaserInfo[5]);
			}

			count++;
			SetCurrentCursorPos(Boss_pos.X, Boss_pos.Y + 3 + count);
			DrawLaser_B(LaserInfo[L]);	//������ ���
			if (reflect)
				Sleep(50);
			else
				Sleep(100);
			DeleteLaser_B(LaserInfo[L]);	//������ ����
		}

		if (count == 16)	//20��°�� �Ǹ� �׸� ��� �ٽ� ����
		{
			count = 0;
			L = (rand() % 4) + 1;
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
				printf("��");
			}

			//�ݻ��׸���
			if (GBInfo_B[y][x] == 'm')
			{
				printf("@");

				Mirr_pos[mnb].X = x;
				Mirr_pos[mnb].Y = y;
				mnb++;

			}

			//����ġ�׸���
			if (GBInfo_B[y][x] == 's')
			{
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

		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//���׸���
				if (GBInfo_B[1][y][x] == 1)
				{
					printf("��");
				}

				//�ݻ��׸���
				if (GBInfo_B[1][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//����ġ�׸���
				if (GBInfo_B[1][y][x] == 's')
				{
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

					DrawBoss(BossInfo[0]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);
	}

	else if ((Switch_B % 4) == 2) // 90�� �̵�
	{
		for (y = 0; y < B_GBOARD_HEIGHT; y++)
		{
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//���׸���
				if (GBInfo_B[2][y][x] == 1)
				{
					printf("��");
				}

				//�ݻ��׸���
				if (GBInfo_B[2][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//����ġ�׸���
				if (GBInfo_B[2][y][x] == 's')
				{
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

		for (y = 0; y<B_GBOARD_HEIGHT; y++)
			for (x = 0; x < B_GBOARD_WIDTH; x++)
			{
				SetCurrentCursorPos((x * 2), y);

				//���׸���
				if (GBInfo_B[3][y][x] == 1)
				{
					printf("��");
				}

				//�ݻ��׸���
				if (GBInfo_B[3][y][x] == 'm')
				{
					printf("@");

					Mirr_pos[mn].X = curPos.X;
					Mirr_pos[mn].Y = curPos.Y;
					mn++;
				}

				//����ġ�׸���
				if (GBInfo_B[3][y][x] == 's')
				{
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

					DrawBoss(BossInfo[0]);
				}

			}

		SetCurrentCursorPos(curPos.X, curPos.Y);


	}

	else if ((Switch_B % 4) == 0) //180�� ���ʹ� �ٽ� ó������!
	{
		drawGB_B(GBInfo_B[0]);
	}
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

void DrawMap_Switch()	//���� �׸��� �Լ� - ����ġ�� ��ȭ�� ���� ��ȭ���� �׷���
{
	int x, y;

	if (Switch_N && changeMap_Normal)//�Ϲݸʿ��� ����ġ�� �ǵ鿴����
	{

		if (check_N == 0)//����ġ ó�� �������� ������
		{
			DeleteOb();
		}
		check_N++;
		if (check_N == 11)//10�� �ڿ� �ٽ� ����
		{
			MT_pos.Y == 28;
			Switch_N = false;
			starttime = 0;
			check_N = 0;
			Switch_N = false;
		}
	}

	if (changeMap_Boss)	//������ �׸���
	{
		if (check_B == 0)//����ġ ó�� �������� ������
		{
			DeleteOb();
		}

		check_B++;

		if ((Switch_B % 4) == 0)//������ �� 4���߿� 0��° �� �׸���
			drawGB_B(GBInfo_B[0]);
		else
			Rotate_BossMap();
	}
	else if (changeMap_Normal) 	//�Ϲݸ� �׸���
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
					printf("��");
				if (GBInfo_N[y][x] == 2)
					printf("��");
				if (GBInfo_N[y][x] == 3)
					printf("��");
				if (GBInfo_N[y][x] == 4)
					printf("��");
				else
					printf("��");

			}

		}
	}

}

void UpOB()	//���� ������ ���� ���ݸ��� ���� �÷��ִ� �Լ� 
{
	int line, x, y;
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		memcpy(&GBInfo_N[y][1], &GBInfo_N[y + 1][1], GBOARD_WIDTH * sizeof(int));//�����Լ�,������ �� ���̵� ���
	}
}

void MakeOb()	//���� ������ GBInfo_N�� �������ִ� �Լ� 
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

int Shoot_MT() //showMT�� ��Ȱ ���׿� �������ִ� ��
{
	if (changeMap_Boss == false)
	{
		COORD curPos = GetCurrentCursorPos();

		DeleteMT(MeteoInfo[0]);

		if (MT_pos.Y == 0 ) {

			return 0;
		} //y�� 1�϶� ���׿��� �ٽ� �Ʒ����� �׷��ֱ�

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

int Physical(int maxLife)	//ü���Լ�(ĳ������ �ִ� ü���� �޾Ƽ� ���� ü���� ����)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//ü���� 0�϶� game over
	{
		SetCurrentCursorPos(20, 30);
		printf("Game Over!\n");
		Sleep(50);
		getchar();
		exit(0);
	}
	else if (attacked && attacked_Boss)//���� �ʿ��� �������� ��������
		nowLife -= 2;

	else if (attacked && attacked_Boss == false)	//��������
		nowLife--;

	attacked = false;	//�ٽ� attacked�� false (������)�� �����ش�.
	return nowLife;	//���� ü���� �����Ѵ�.

}


int isCrash(int posX, int posY, char PCInfo[4][4], char GBInfo_B[31][31], char MeteoInfo[4][4])	//�浹 �Լ�
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
				if (GBInfo_N[arrY + y][arrX + x] == 1)	//���̶� �ε�������
					return 0;

				if (GBInfo_N[arrY + y][arrX + x] == 2)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}
				if (GBInfo_N[arrY + y][arrX + x] == 3)	//�Ϲݸ� ����ġ
				{
					Switch_N = true;
					deletePC(PCInfo);
					PC_pos.Y = 13;
					MT_pos.X = 28;
					MT_pos.Y = 3;
					return 0;
				}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
				{
					attacked = true;
					DeleteMT(MeteoInfo);
					MT_pos.Y = 1;
				} //�浹�� 1�� ���ư��� int main���� ���Ǽ��� -> y�� 1�϶� �����

			}

			///��ȯ��
			if (PCInfo[x][y] == 1 && changeMap_Normal == true && Switch_N == true)	//pc�� 1�̰�, �Ϲݸ� �°�, ����ġ�� ����
			{

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 1)	//���̶� �ε�������
					return 0;

				if (GBInfo_N[arrY + y + 1][arrX + x - 1] == 2)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}

				//if (GBInfo_N[arrY + y][arrX + x] == 3)	//��ȯ�� ����ġ
				//{
				//	Switch_N = true;
				//	deletePC(PCInfo);
				//	PC_pos.Y = 13;
				//	MT_pos.X = 28;
				//	MT_pos.Y = 3;
				//	return 0;
				//}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
				{
					attacked = true;
					return 0;
				}

			}

			//���� ��
			if (PCInfo[y][x] == 1 && changeMap_Normal == false && changeMap_Boss == true)//�Ϲݸ� �ƴϰ�, ���� ���� ����
			{
				if (GBInfo_B[arrY + y][arrX + x] == 1)	//���̶� �ε�������
				{
					return 0;
				}

				////if(GBInfo_B[arrY + y][arrX + x] == 'b' || GBInfo_B[arrY + y][arrX + x -1] == 'b' || GBInfo_B[arrY + y][arrX + x -2] == 'b' || GBInfo_B[arrY + y][arrX + x - 3] == 'b' || GBInfo_B[arrY + y][arrX + x - 4] == 'b')
				//if(GBInfo_B[arrY + y -1][arrX + x ] == 'b' || GBInfo_B[arrY + y -2][arrX + x ] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x-1] == 'b' || GBInfo_B[arrY + y - 2][arrX + x-1] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x - 2] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 2] == 'b' ||
				//	GBInfo_B[arrY + y - 1][arrX + x - 3] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 3] == 'b' || 
				//	GBInfo_B[arrY + y - 1][arrX + x - 4] == 'b' || GBInfo_B[arrY + y - 2][arrX + x - 4] == 'b'  )	//������ �ε�������	
				////if(BossInfo[arrY + y][arrX + x] == 1)
				//{
				//	//printf("����");
				//	attacked = true;
				//	attacked_Boss = true;
				//	return 0;
				//}

				/*if (GBInfo_B[arrY + y][arrX + x] == 'm')	//�ſ��̶� �ε�������
				{
				printf("�ſ�");
				//return 0;
				}*/

				if (GBInfo_B[arrY + y][arrX + x] == 's')	//����ġ�� �ε�������
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
				//	//printf("������");


				//}


				//if (GBInfo_N[arrY + y][arrX + x] == 3)	//�Ϲݸ� ����ġ
				//{
				//	Switch_N = true;
				//	deletePC(PCInfo);
				//	PC_pos.Y = 13;
				//	MT_pos.X = 28;
				//	MT_pos.Y = 3;
				//	return 0;
				//}

				if ((PC_pos.X == MT_pos.X) && ((PC_pos.Y + 3 == MT_pos.Y) || (PC_pos.Y + 2 == MT_pos.Y) || (PC_pos.Y + 1 == MT_pos.Y)))	//� �浹 (���� �հ� ������)
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
	if (isCrash(PC_pos.X + 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0)//�ε���
	{
		if (changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//��ȯ�ʿ��� �ε���(�и�)
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
	if (isCrash(PC_pos.X - 2, PC_pos.Y, PCInfo[0], GBInfo_B[Switch_B % 4],MeteoInfo[0]) == 0)//�ε���
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
	if (isCrash(PC_pos.X, PC_pos.Y - 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 || PC_pos.Y == 0)//�ε���
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
	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == false)	//�ε���	//�Ϲݸ� �ö���� ��	//����ġ X
	{
		PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)	//�ε���	//��ȯ�� �Ʒ� ��//����ġ O
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == true && changeMap_Boss == false && Switch_N == true)//�ε���	//��ȯ�� ������ �ٰ����� ��//����ġ O
	{
		PC_pos.X -= 2;
		PC_pos.Y += 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0], GBInfo_B[Switch_B % 4], MeteoInfo[0]) == 0 && changeMap_Normal == false && changeMap_Boss == true)//�ε���		//������	O	//GBInfo_B[Switch_B % 4]���� Switch_B % 4�� ������ �� ��ȣ��
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

void isN_clear()//Ŭ����(�Ϲ�)
{
	printf("�Ϲݸ� Ŭ����");	//�ϴ� ���
	Sleep(1000);

	changeMap_Normal = false;//�Ϲݸ� �ƴ�
	changeMap_Boss = true;//������ ����
	check_B = 0;//�ܼ�â �ѹ� �����ֱ� ���ؼ� 
	PC_pos.Y = 10;//PC��ġ �Ű���

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
				break;
			}
		}
		Sleep(speed);
	}
}


int main(void)
{
	srand((unsigned int)time(NULL));
	L = (rand() % 4) + 1;	//������ �� ����

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
		printf("PC ü��: %3d", Physical(PCLife));

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
	


		if (changeMap_Normal == true && changeMap_Boss == false)	//�Ϲݸ� O	�϶�//������ X
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

		if (changeMap_Normal == true && changeMap_Boss == false && ObTime == 50)	//5��° ���� �� Ŭ����(�Ϲ�) ->�� �� �ٲ����
			isN_clear();

		if (changeMap_Normal == false && changeMap_Boss == true && check_B>0)	//�������� ��Y	//check_B�� �������� �� �׸��� �������� �����
		{
			ShootLaser();//�������� ��� �Լ� (Draw & Delete)
		}

	}

	getchar();
	return 0;
}