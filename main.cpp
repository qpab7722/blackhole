#include <stdio.h>
#include <Windows.h>

int curPosX;
int curPosY;

int PCLife = 30;	//PC�� ü��

bool attacked = false;	//���ݹ޾Ҵ��� �˷��ִ� �Լ�


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

int isCrash(int posX, int posY, char PCinfo[4][4], char ObInfo[25][25])	//�浹 �Լ�
{
	int x, y;
	int arrX = (posX) / 2;
	int arrY = posY;
	for (x = 0; x<4; x++)
	{
		for (y = 0; y<4; y++)
		{
			if (PCinfo[y][x] == 1)
			{
				if (ObInfo[arrY + y][arrX + x] == 1)	//���̶� �ε�������
					return 0;

				if (ObInfo[arrY + y][arrX + x] == 2)	//��ֹ��̶� �ε�������
				{
					attacked = true;
					return 0;
				}
			}
		}
	}
	return 1;
}




int main(void)
{
	RemoveCursor();

	while (1)
	{
		SetCurrentCursorPos(60, curPosY - 1);
		printf("                 ");	//���ٿ� ���°� �����ַ���
		SetCurrentCursorPos(60, curPosY);
		printf("PC ü��: %3d", Physical(PCLife));
	}


	
	return 0;
}