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
COORD Mirr_pos[4] = { 0 };//�ݻ�� ��ġ
COORD Switch_pos[4] = { 0 };//����ġ ��ġ
COORD Boss_pos = { 0,0 };

int GBInfo_N[GBOARD_HEIGHT][GBOARD_WIDTH];


int speed = 30;
int PCLife = 30;	//PC�� ü��
int ObTime;	//����� ���� ��������� X��ǥ
int Check_Ob = 0;	//�ö󰡴� ����	(��ֹ��� ��ֹ����� ����)
int Ran;	//����� ���� ����
int check_N;//��ȯ�ð� �˻�
int starttime = 0;//��ȯ ���� �ð�

bool attacked = false;	//���ݹ޾Ҵ��� �˷��ִ� �Լ�
bool Switch_N = false; //����ġ �浹 �˷��ִ� �Լ�
bool B_OK=false;//������ ��ȯ ��ȣ(�ӽ�)


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

int DetectCollision(int posX, int posY, char MeteoInfo[4][4])	//�� �������� ������ �� ���� �ҵ�
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

//�ݻ���� �׸��� �Լ�
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
				printf("��");			
		}	
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//����ġ�� �׸��� �Լ�
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
				printf("��");
		}
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//Boss�� �׸��� �Լ�
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
				if (y == 0) printf("��");
				if (y == 1)	printf("��");
				else  printf("��");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

//�������� �׸��� �Լ�
void drawGB_B(char GBInfo_B[30][30])
{
	int x, y, mn=0,sn=0;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0;y<B_GBOARD_HEIGHT;y++)
		for (x = 0;x < B_GBOARD_WIDTH;x++)
		{
			SetCurrentCursorPos((x * 2), y);

			//���׸���
			if (GBInfo_B[y][x] == 1)
			{
				if (y == 0) //����
				{
					if (x == 0) { printf("��"); continue; }
					if (x == B_GBOARD_WIDTH - 1) { printf("��"); continue; }
					printf("��");
				}
				if (y == B_GBOARD_HEIGHT - 1)//�Ʒ���
				{
					if (x == 0) { printf("��"); continue; }
					if (x == B_GBOARD_WIDTH - 1) { printf("��"); continue; }
					printf("��");
				}
				if (x == 0 )//���ʺ�
					printf("��");
				
				if (x == B_GBOARD_WIDTH - 1)//������
					printf("��");			
			}

			//�ݻ��׸���
			if (GBInfo_B[y][x] == 'm')
			{	
				if(x == 1 || x == B_GBOARD_WIDTH - 2)//����
					drawMirr(MirrInfo[1]);
				
				else
					drawMirr(MirrInfo[0]);
				
				Mirr_pos[mn].X = curPos.X;
				Mirr_pos[mn].Y = curPos.Y;
				mn++;
			}

			//����ġ�׸���
			if (GBInfo_B[y][x] == 's')
			{
				if (x == 1 || x == B_GBOARD_WIDTH - 2)//����
					drawSwitch(SwitchInfo[1]);

				else
					drawSwitch(SwitchInfo[0]);

				Switch_pos[sn].X = curPos.X;
				Switch_pos[sn].Y = curPos.Y;
				sn++;
			}

			//Boss�׸���
			if (GBInfo_B[y][x] == 'b')
			{
				drawBoss(BossInfo[0]);
				Boss_pos.X = curPos.X;
				Boss_pos.Y = curPos.Y;
			}

		}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}


void DrawOb()	//���� ������ �׸��� �Լ�
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

	if (B_OK)	//������ �׸���
	{
		drawGB_B(GBInfo_B[0]);
	}
	else 	//�Ϲݸ� �׸���
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
					printf("��");
				if (GBInfo_N[y][x] == 2)
					printf("��");
				if (GBInfo_N[y][x] == 3)
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

	else if (attacked)	//��������
		nowLife--;

	attacked = false;	//�ٽ� attacked�� false (������)�� �����ش�.
	return nowLife;	//���� ü���� �����Ѵ�.

}


int isCrash(int posX, int posY, char PCInfo[4][4])	//�浹 �Լ�
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
			///�Ϲݸ�
			if (PCInfo[y][x] == 1 && Switch_N == false)
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

				}

			}

			///��ȯ��
			if (PCInfo[x][y] == 1 && Switch_N == true)
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
	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == false)	//�Ϲݸ� �ö���� ��
	{
		PC_pos.Y -= 1;
		return 0;
	}

	if (isCrash(PC_pos.X, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == true)	//��ȯ�� �Ʒ� ��
		return 0;

	else if (isCrash(PC_pos.X + 2, PC_pos.Y + 1, PCInfo[0]) == 0 && Switch_N == true)	//��ȯ�� ������ �ٰ����� ��
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
			case '1':	//�߰�
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
		printf("PC ü��: %3d", Physical(PCLife));

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
