#include <stdio.h>
#include <stdlib.h>	//malloc()����
#include <stdbool.h>
#include <conio.h>	//_kbhit()������Ҫ
#include <windows.h>	//sleep()������Ҫ
#define ROW 22 		//��ͼ��
#define COL 22		//��ͼ��

//�����ͣ���¼��ĺ�������
typedef struct Mypoint{
	int row;
	int col;
}Point;

//�߽��
typedef struct SnakeNode{
	Point pos;
	struct SnakeNode* next;
}snakeNode;

Point snakefood = { -1 };		//β��ʳ�����������Ҫ�ҵ���βλ�ã�����β����ʳ�Ĭ��Ϊ-1����ʾû��β��ʳ��
Point movepoint[3000] = { 0 };	//��¼�������߹��ĵ�
snakeNode* list = NULL;		//�ô�ͷ����������������
enum map{ space, wall, snake, food };//��ͼ�������ö�� space�հ� wallǽ snake�� foodʳ��
Point Food[1] = { 0 };	//ʳ���,�����(0,0)����ʳ��
Point* pBuff = NULL;	//��¼space���ӣ��ų���ǽ�ڣ��ߣ�ʳ��������
bool isLive = true;	//Ĭ���ߴ��
int* pMap = NULL;	//��ͼ

void initSnake();	//��ʼ����
void initMap();		//��ʼ����ͼ
void initGame();	//��ʼ����Ϸ
void showGame();	//��ʾ��Ϸ
void ctrolGame();	//������Ϸ
void initFood();	//��ʼ��ʳ��
void updataMap();	//���µ�ͼ
bool vsnakeeat();	//�ų�һ��������ȥ��ʳ��
int* num();		//�������0��3�����֣���ʾ���ƶ�����
void snake_move(int row, int col);					//���ƶ�
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col);	//�������ƶ�
enum map isWhat(int row, int col);					//�ж�ĳ������ʲô����������ֵ
enum map visWhat(snakeNode** vlist, int row, int col);			//�ж�������ĳ������ʲô����������ֵ
void insertNode(int row, int col);					//ͷ�巨�����㵽��������
void vinsertNode(snakeNode** vlist, snakeNode** rear, int row, int col);	//ͷ�巨����ڵ㵽������������
bool vsnake_have_space(int row, int col);				//���˷�������ͷ�����Ƿ���space
bool vsnakefand(snakeNode** vlist, snakeNode**rear);			//�������ҵ���β



int main(){
	srand(time(0));		//�������������
	//�����ߵ�ͷ���
	struct SnakeNode* head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode*));
	list = head;
	list->pos.row = NULL;
	list->pos.col = NULL;
	list->next = NULL;
	initGame();
	showGame();
	while (isLive){		//�ߴ���Ϸ������������Ϸ����
		ctrolGame();
	}
	printf("GameOver\n");
	while (1);
	return 0;
}


//��ʼ����Ϸ
void initGame(){
	initSnake();
	pBuff = malloc(sizeof(Point)*(ROW*COL - 2 * ROW - 2 * (COL - 2) - 3));		//����buff�ռ䣬��¼��ͼ��������space����
	initFood();
	initMap();
}


//��ʼ���ߣ�����3����㵽�����У��ֱ��ǣ�2��2�� �� 2��3�� �� 2��4��
void initSnake(){
	for (int i = 0; i < 3; i++){
		insertNode(2, 2 + i);
	}
}


//��ʼ��ʳ��
void initFood(){
	//��ȡ��ǰ�հ׸�����������ʼ��buff
	int index = 0;
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			if (isWhat(i, j) == space){
				pBuff[index].row = i;
				pBuff[index].col = j;
				index++;
			}
		}
	}
	if (index == 0){		//û�пռ��ʳ���Ϸ����
		updataMap();
		printf("win!!!");
		while (1);
	}
	int suiji = rand() % index;	//�������ʳ�indexΪ0��ʱ���޷�ʹ��%
	Food[0].row = pBuff[suiji].row;
	Food[0].col = pBuff[suiji].col;
}


//��ʼ����ͼ
void initMap(){
	pMap = (int*)malloc(sizeof(int)*ROW*COL);  		//��ͼ����
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			*(pMap + (i*COL) + j) = isWhat(i, j);	//��άתһά
		}
	}
}


//�ж�ĳ������ʲô����������ֵ
enum map isWhat(int row, int col){
	//����Ƿ�Ϊǽ��
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//����Ƿ�����
	struct SnakeNode* temp = list->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col)  return snake;
		temp = temp->next;
	}

	//����Ƿ�Ϊʳ��
	if (Food[0].row == row && Food[0].col == col) return food;

	//�����ǣ�����space
	return space;
}


//�����ߴ���ʱ���жϵ�ͼ�ϵĵ���ʲô
enum map visWhat(snakeNode** vlist, int row, int col){
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//����Ƿ�Ϊʳ��
	if (Food[0].row == row && Food[0].col == col) {
		if (snakefood.row == -1){		//����ʱ������β��ʳ�������Ѱ��β��λ��ʱ���õ�Ϊfood
			return food;
		}
	}

	//����Ƿ���������
	struct SnakeNode* temp = (*vlist)->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col) return snake;
		temp = temp->next;
	}

	//�����ǣ�����space
	return space;
}


//���µ�ͼ
void updataMap(){
	int score = 0;
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			*(pMap + (i*COL) + j) = isWhat(i, j);
			if (*(pMap + (i*COL) + j) == snake){
				score++;
			}
		}
	}
	showGame();
	printf("��ǰ�߳���%d\n", score);
}


//��ʾ��Ϸ
void showGame(){
	Point temlast;		//��β
	snakeNode* tem = list->next;
	while (tem->next != NULL){
		tem = tem->next;
	}
	temlast.row = tem->pos.row;
	temlast.col = tem->pos.col;
	system("cls");
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			switch (*(pMap + (i*COL) + j)){		//��άתһά
			case wall: printf("#"); break;		//ǽ�ڱ�ʶ
			case space: printf(" "); break;		//�հ׸���
			case snake:
				if (i == list->next->pos.row && j == list->next->pos.col){
					printf("$");		//��ͷ��ʶ
					break;
				}
				if (i == temlast.row&&j == temlast.col){
					printf("O");		//��β��ʶ
					break;
				}
				else{
					printf("*");		//�����ʶ
					break;
				}
			case food: printf("@"); break;		//ʳ���ʶ
			}
		}
		printf("\n");
	}
	//Sleep(200);	//���õ�ͼˢ���ٶ�
}


//ͷ�巨�����㵽��ʵ��������
void insertNode(int row, int col){
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = list->next;
	list->next = pNew;
}


//β�巨�����㵽��������
void vinsertNode(snakeNode** vlist, snakeNode**rear, int row, int col){
	struct SnakeNode* vpNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vpNew->pos.row = row;
	vpNew->pos.col = col;
	vpNew->next = (*rear)->next;
	(*rear)->next = vpNew;
	*rear = vpNew;
}


//��ʵ���ƶ�
void snake_move(int row, int col){
	//ȥβ
	struct SnakeNode* pTemp = list;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	insertNode(row, col);
}


//�������ƶ�һ��
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col){
	//ȥβ
	struct SnakeNode* pTemp = *vlist;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	*rear = pTemp;
	//ͷ�巨����һ�����
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//����һ���µĽ��
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = (*vlist)->next;
	(*vlist)->next = pNew;
}


//�������0��3֮�䲻�ظ���4������
int* num(){
	int *num = NULL;
	int id = 0;
	num = (int*)malloc(sizeof(int)* 4);
	for (int i = 0; i < 100; i++){
		int tag = 1;
		int a = rand() % 4;
		for (int i = 0; i < 4; i++){
			if (*(num + i) == a){
				tag = 0;
				break;
			}
		}
		if (tag == 0){		//���Ϊ0��ʾ�����ɸ����֣������ô�ѭ��
			continue;
		}
		else{
			*(num + id) = a;
			id++;
		}
		if (id == 4){
			break;
		}
	}
	return num;
}


//������Ϸ
void ctrolGame(){
	if (vsnakeeat()){		//�������ܳԵ�ʳ��ҳ���ʳ������ҵ�β��
		//printf("�������ܳԵ�ʳ��ҳ���ʳ������ҵ�β��");
		//Sleep(1000);
		int i = 0;
		while (movepoint[i].row != -1){		//��ʵ�߸��������߼�¼��λ���ƶ�
			switch (isWhat(movepoint[i].row, movepoint[i].col))
			{
			case food:
				insertNode(movepoint[i].row, movepoint[i].col);	//ͷ�巨����һ����㵽������
				initFood();
				break;
			case snake:			//ҧβ��
			case space:
				snake_move(movepoint[i].row, movepoint[i].col);
				break;
			}
			updataMap();
			i++;
		}
	}
	else{	//������δ�Ե�ʳ�� �� �Ե�ʳ����Ҳ���β�͡���ʱ����ʵ��Զ��ʳ�� �� ���ҵ�·�����ҵ�β��
		Point listlast;			//������ʵ�ߵ�β��
		snakeNode* temp = list->next;
		while (temp->next != NULL){
			temp = temp->next;
		}
		listlast.row = temp->pos.row;
		listlast.col = temp->pos.col;
		Point dir[4] = { 0 };		//�洢��ͷ����������λ�����������,0��  1��  2��  3��
		int maxH = -1;		//����һ��Hֵ��־����ʼΪһ����С��maxH���Ա��ҵ��ĸ���������ʳ����Զ(Hֵ���)�������ҵ�β��ʳ��ķ����
		int max = -1;		//��¼���ҵ��������Hֵ�ĵ㣬Ĭ��Ϊ-1����maxֵδ�ı���֤��δ�ҵ������߿ɵ���β��ʳ��ķ����
		int backupmaxH = -1;	//���÷�����������Ҳ���β��ʳ���ʹ�ô˼�¼�����ݣ�����ʳ����Զ�ĵط��ƶ�
		int backupmax = -1;	//��¼���ҵ��������Hֵ�ĵ㣬Ĭ��Ϊ-1����backupmaxֵδ�ı���֤��δ�ҵ������߿ɵ���β��ʳ��ķ���� �� �������Χû��space��food

		//���������������꣬�������
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: dir[i].row = list->next->pos.row - 1; dir[i].col = list->next->pos.col;		break;
			case 1: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col + 1;	break;
			case 2: dir[i].row = list->next->pos.row + 1; dir[i].col = list->next->pos.col;		break;
			case 3: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col - 1;	break;
			}
		}

		//�ҳ����ҵ�β������ʳ����Զ�ĵ�
		int *number = num();		//�����������
		for (int n = 0; n < 4; n++){
			int i = *(number + n);

			//ÿ�θı䷽�����������һ������ʵ����ͬ��������
			snakeNode* vlist = NULL;	//�ô�ͷ�������������������
			struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
			vhead->pos.row = NULL;
			vhead->pos.col = NULL;
			vhead->next = NULL;
			vlist = vhead;
			snakeNode* rear = vlist;	//������β��

			//��������ʵ��һ����������
			struct SnakeNode* tmp = list->next;
			while (tmp != NULL){
				vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
				tmp = tmp->next;
			}

			//������Χ�����ߵĵ�
			if ((dir[i].row != listlast.row || dir[i].col != listlast.col) && (isWhat(dir[i].row, dir[i].col) == snake || isWhat(dir[i].row, dir[i].col) == wall)){
				continue;	//���÷���㲻����ʵ��β��ͬ �� �����������ǽ ������
			}
			vsnake_move(&vlist, &rear, dir[i].row, dir[i].col);//��������÷������һ��

			//�����ҵ�β���ĵ�
			snakefood.row = rear->pos.row;			//����β��ʳ��Ϊ��ǰ�����ߵ�β�ͣ���������ȥ��β��ʳ��
			snakefood.col = rear->pos.col;
			if (vsnakefand(&vlist, &rear) == false){	//�������Ҳ���β��ʳ��
				if (vsnake_have_space(dir[i].row, dir[i].col)){		//����˷������Χ����space��food
					int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//����Hֵ,�ҳ���ʳ����Զ�ķ����
					if (H > backupmaxH){
						backupmaxH = H;
						backupmax = i;
						//Sleep(1000);
						continue;		//��ʼ�ж���һ�������
					}
				}
				else{	//�˷������Χû��space��food�������жϸ÷�����Ƿ�Ϊʳ��
					if (isWhat(dir[i].row, dir[i].col) == food){	//����˷���Ϊʳ��жϸ�ʳ���Ƿ�Ϊ�ܲ��������һ��ʳ��
						//printf("�жϳ��÷���Ϊʳ��i=%d\n",i);
						//Sleep(10000);
						int index = 0;
						for (int i = 0; i < ROW; i++){
							for (int j = 0; j < COL; j++){
								if (isWhat(i, j) == space){
									pBuff[index].row = i;
									pBuff[index].col = j;
									index++;
								}
							}
						}
						if (index == 0){			//space����Ϊ0��˵��ֻʣ�÷������һ��ʳ��
							insertNode(dir[i].row, dir[i].col);
							initFood();			//���һ��ʳ�ﱻ�Ե�����ʱ��ʼ��ʳ�������Ϸ
						}
					}
					else{
						continue;	//�˷���㲻Ϊʳ���ʼ�ж���һ�������
					}
				}

			}
			else{	//���������ҵ�β��ʳ�ȡ��ʳ��food��Զ�ķ����	
				int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//����Hֵ
				if (H > maxH){
					maxH = H;
					max = i;
				}
			}
		}//���������ĸ������

		if (max == -1){		//û�����ҵ�β��ʳ��ķ����
			if (backupmax == -1){	//û�� ��Χ��space��food �ķ����
				int *number = num();	//�����������
				for (int n = 0; n < 4; n++){
					int i = *(number + n);
					if (isWhat(dir[i].row, dir[i].col) == space){ //��ʱ��Ϸ�Ѿ����ˣ���Ϊ���������ʱ���ߵĵ㣬ѡ����������space��food�ķ����ƶ�
						snake_move(dir[i].row, dir[i].col);
						printf("��Ȼ֪�����ˣ����һ���Ҫ�������ߵĵ�");
						Sleep(5000);
						updataMap();
					}
					if (isWhat(dir[i].row, dir[i].col) == food){
						insertNode(dir[i].row, dir[i].col);
						initFood();
						updataMap();
					}
				}
				isLive = false;
				return;
			}
			else{
				max = backupmax;
			}
		}

		if (isWhat(dir[max].row, dir[max].col) == space || isWhat(dir[max].row, dir[max].col) == snake){//����space����ʵ���ƶ�������snake���β��	
			if (isWhat(dir[max].row, dir[max].col) == snake){
				//printf("��ҧβ����");
				//Sleep(1000);
			}
			snake_move(dir[max].row, dir[max].col);
			updataMap();
		}
		else{
			insertNode(dir[max].row, dir[max].col);	//�˷����Ϊʳ�ͷ�巨����һ����㵽������
			initFood();
			updataMap();
		}
	}
}


//AIʵ�ֵĲ���
bool vsnakeeat(){				//�ų�һ��������ȥ��ʳ��
	int index = 0;				//�������߹��ĵ�������±�
	//����������
	snakeNode* vlist = NULL;		//�ô�ͷ�������������������
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//��������β

	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}


	//�ƶ�������ȥ��ʳ��
	Point vdir[4] = { 0 };			//�洢������ͷ����������λ�����������,0��  1��  2��  3��
	while (visWhat(&vlist, vlist->next->pos.row, vlist->next->pos.col) != food){
		int vfoodminH = ROW + COL;	//����һ��Hֵ��־��Ĭ��Ϊ����H���Ա��ҵ��ĸ���������ʳ������ĵ�
		int vmin = -1;			//��¼���ҵ�������СHֵ�ĵ㣬Ĭ��Ϊ-1����vminֵδ�ı���֤��δ�ҵ����ߵĵ�
		int vsnakemaxH = -1;		//����һ��С��ֵ���Ա��ҵ�����β���ķ����

		//���������ͷ������������꣬�������
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = vlist->next->pos.row - 1; vdir[i].col = vlist->next->pos.col;		break;
			case 1: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col + 1;	break;
			case 2: vdir[i].row = vlist->next->pos.row + 1; vdir[i].col = vlist->next->pos.col;		break;
			case 3: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col - 1;	break;
			}
		}

		//������ߵ�������СHֵ�ĵ�
		int *number = num();			//�����������
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != rear->pos.row || vdir[i].col != rear->pos.col) && (visWhat(&vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&vlist, vdir[i].row, vdir[i].col) == wall)){	//���õ㲻��������β �� �����������ǽ ������
				//printf("����(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(5000);
				continue;		//���������������ķ����
			}
			int vfoodH = abs(vdir[i].row - Food[0].row) + abs(vdir[i].col - Food[0].col);			//����÷���㵽ʳ���Hֵ

			if (vfoodH < vfoodminH){
				vfoodminH = vfoodH;	//�ҳ���ʳ������ķ����
				vmin = i;
				vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);	//����÷���㵽��β��Hֵ
			}
			if (vfoodH == vfoodminH){	//�������������ʳ����ͬ����Hֵ����ѡ����������βԶ�ķ����
				if ((abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col)) > vsnakemaxH){
					vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);
					vmin = i;
				}
			}
		}					//�������ĸ������


		if (vmin == -1){			//û�ҵ����������ķ����	
			return false;			//����false��δ�ҵ����ߵĵ�
		}

		if (visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == space || visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == snake){
			vsnake_move(&vlist, &rear, vdir[vmin].row, vdir[vmin].col);		//��Ϊspace���ƶ������ߣ���Ϊsnake����ʾ�õ�Ϊ��β���ƶ�������
		}
		else{																//�õ�Ϊfood��ͷ�巨����õ㵽������
			struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//����һ���µĽ��
			pNew->pos.row = vdir[vmin].row;
			pNew->pos.col = vdir[vmin].col;
			//pNew->next = NULL;		//��Ƶ��ûд���
			pNew->next = vlist->next;
			vlist->next = pNew;
		}
		//system("cls");
		//printf("д��·����");
		movepoint[index].row = vdir[vmin].row;		//���߹��ĵ��¼��������
		movepoint[index].col = vdir[vmin].col;
		index++;
		if (index > 20000){
			//printf("���˺ܾ�û�ҵ���ֱ�ӷ���");
			//Sleep(700);
			return false;
		}
	}

	//�жϴ�ʱ�������ܷ��ҵ���β
	snakefood.row = rear->pos.row;
	snakefood.col = rear->pos.col;
	if (vsnakefand(&vlist, &rear)){
		movepoint[index].row = -1;		//���λ����ʵ��ֻ�� �洢�㣨-1��-1��֮ǰ������ĵ�
		movepoint[index].col = -1;
		//printf("�����߳Ե�ʳ����ҵ�β���ˣ���Ҫ������ʵ���ˣ�����ȥ��ʳ��");
		//Sleep(5000);
		//printf("�����߳���ʳ���ҵ�·��\n");
		//Sleep(5000);
		return true;
	}
	else{
		//printf("�����߳Ե�ʳ���δ�ҵ�β�ͣ���Ҫ������ʵ�ߣ������ȱ�ȥ��ʳ�������ȫ��λ���ٳ�");
		//Sleep(5000);
		//printf("�����߳���ʳ��δ�ҵ�·��\n");
		//Sleep(5000);
		return false;
	}
}


bool vsnakefand(snakeNode** vlist, snakeNode**rear){		//��������β��ʳ��
	Point vdir[4] = { 0 };		//�洢��ͷ����������λ�����������,0��  1��  2��  3��
	while ((*vlist)->next->pos.row != snakefood.row || (*vlist)->next->pos.col != snakefood.col){		//�����ߵ���ͷ�Ƿ񵽴������¼����βλ��
		int vminH = ROW + COL;				//����һ��Hֵ��־��Ĭ��Ϊ����H���Ա��ҵ��ĸ�����������СHֵ�ĵ�
		int vmin = -1;						//��¼���ҵ�������СHֵ�ĵ㣬Ĭ��Ϊ-1����vminֵδ�ı���֤��δ�ҵ����ߵĵ�

		//����������������
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = (*vlist)->next->pos.row - 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 1: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col + 1;	break;
			case 2: vdir[i].row = (*vlist)->next->pos.row + 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 3: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col - 1;	break;
			}
		}

		//������ߵ�������СHֵ�ĵ�
		int *number = num();		//�����������
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != snakefood.row || vdir[i].col != snakefood.col) && (visWhat(&*vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&*vlist, vdir[i].row, vdir[i].col) == wall)){	//���õ㲻��β��ʳ��snakefood �� �����������ǽ ������
				//printf("��������(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(1000);
				continue;
			}
			int H = abs(vdir[i].row - snakefood.row) + abs(vdir[i].col - snakefood.col);		//����Hֵ
			if (H < vminH){
				vminH = H;
				vmin = i;
			}
		}
		if (vmin == -1){
			//printf("��������������β��ʳ�ﵱ�У�δ�ҵ�����Ҫ������");
			//Sleep(5000);
			snakefood.row = -1;	//����snakefood
			snakefood.col = -1;
			return false;		//δ�ҵ����ߵĵ�
		}
		vsnake_move(&*vlist, &*rear, vdir[vmin].row, vdir[vmin].col);		//�ҵ����ߵ�һ���㣬�ƶ�������
	}
	snakefood.row = -1;	//����snakefood
	snakefood.col = -1;
	return true;

}


//���˷�������ͷ�����Ƿ���space��food
bool vsnake_have_space(int row, int col){
	snakeNode* vlist = NULL;		//�ô�ͷ�������������������
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//βָ��
	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}
	vsnake_move(&vlist, &rear, row, col);		//���������˷�����һ��
	Point exam[4] = { 0 };
	for (int i = 0; i < 4; i++){
		switch (i){
		case 0: exam[i].row = vlist->next->pos.row - 1; 	exam[i].col = vlist->next->pos.col;	break;			/////�ĸ�
		case 1: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col + 1;	break;
		case 2: exam[i].row = vlist->next->pos.row + 1; 	exam[i].col = vlist->next->pos.col;	break;
		case 3: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col - 1;	break;
		}
	}
	int *number = num();		//�����������
	for (int n = 0; n < 4; n++){
		int i = *(number + n);
		if (visWhat(&vlist, exam[i].row, exam[i].col) == space || visWhat(&vlist, exam[i].row, exam[i].col) == food){
			return true;
		}
	}
	return false;
}



