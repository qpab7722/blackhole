#include <stdio.h>

int Physical(int maxLife)	//ü���Լ�(ĳ������ �ִ� ü���� �޾Ƽ� ���� ü���� ����)
{
	int nowLife = maxLife;

	if (nowLife == 0)
		gameover;

	else if (attacked)	//���� �޾Ұų� �浹������ 
		nowLife--;

	else if (portion)	//���� �Ծ �ٽ� ������ �ö�
		nowLife++;

	else if (stageClear)	//�������� Ŭ�����ϸ� ü�� ȸ��(?)- ������
		nowLife = maxLife;

	printf("���� ü��: %d", nowLife);
	return nowLife;	//���� ü���� �����Ѵ�.

}

int isCrash(int map)
{
	bool attcked = false;

	if (map == N && (PCinfo[PC_pos_x][PC_pos_y] && Obstacleinfo[X][Y] == 1))	//�Ϲ� �ʿ��� PC�� ��ֹ��� ���� ������
	{
		attacked = true;
		//���⿡�� Life�Լ� ȣ��

	}

	if(map == C )

}

void isN_clear()	//�Ϲ� �� Ŭ����
{

}

void isB_clear()	//���� �� Ŭ����
{

}





int main(void)
{

	

	
	return 0;
}