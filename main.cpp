#include <stdio.h>

int Physical(int maxLife)	//체력함수(캐릭터의 최대 체력을 받아서 현재 체력을 리턴)
{
	int nowLife = maxLife;

	if (nowLife == 0)
		gameover;

	else if (attacked)	//공격 받았거나 충돌했을때 
		nowLife--;

	else if (portion)	//포션 먹어서 다시 게이지 올라감
		nowLife++;

	else if (stageClear)	//스테이지 클리어하면 체력 회복(?)- 안정함
		nowLife = maxLife;

	printf("현재 체력: %d", nowLife);
	return nowLife;	//현재 체력을 리턴한다.

}

int isCrash(int map)
{
	bool attcked = false;

	if (map == N && (PCinfo[PC_pos_x][PC_pos_y] && Obstacleinfo[X][Y] == 1))	//일반 맵에서 PC가 장애물에 도달 했을때
	{
		attacked = true;
		//여기에서 Life함수 호출

	}

	if(map == C )

}

void isN_clear()	//일반 맵 클리어
{

}

void isB_clear()	//보스 맵 클리어
{

}





int main(void)
{

	

	
	return 0;
}