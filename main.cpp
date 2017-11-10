#include <stdio.h>
#include <Windows.h>

int curPosX;
int curPosY;

int PCLife = 30;	//PC의 체력

bool attacked = false;	//공격받았는지 알려주는 함수


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






int Physical(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	static int nowLife = maxLife;

	if (nowLife == 0)	//체력이 0일때 game over
	{
		SetCurrentCursorPos(20, curPosY);
		printf("Game Over!\n");
		Sleep(100);
		exit(0);
	}

	else if (attacked)	//아팠을때
		nowLife--;

	attacked = false;	//다시 attacked을 false (원상태)로 돌려준다.
	return nowLife;	//현재 체력을 리턴한다.

}

int isCrash(int posX, int posY, char PCinfo[4][4], char ObInfo[25][25])	//충돌 함수
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
				if (ObInfo[arrY + y][arrX + x] == 1)	//벽이랑 부딪혔을때
					return 0;

				if (ObInfo[arrY + y][arrX + x] == 2)	//장애물이랑 부딪혔을때
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
		printf("                 ");	//윗줄에 나온거 지워주려고
		SetCurrentCursorPos(60, curPosY);
		printf("PC 체력: %3d", Physical(PCLife));
	}


	
	return 0;
}