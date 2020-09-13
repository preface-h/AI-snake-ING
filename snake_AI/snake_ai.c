#include <stdio.h>
#include <stdlib.h>	//malloc()函数
#include <stdbool.h>
#include <conio.h>	//_kbhit()函数需要
#include <windows.h>	//sleep()函数需要
#define ROW 12 		//地图高
#define COL 12		//地图宽

//点类型，记录点的横纵坐标
typedef struct Mypoint{
	int row;
	int col;
}Point;

//蛇结点
typedef struct SnakeNode{
	Point pos;
	struct SnakeNode* next;
}snakeNode;

Point snakefood = { -1 };		//尾巴食物，即虚拟蛇所要找的蛇尾位置，将蛇尾当作食物，默认为-1，表示没有尾巴食物
Point movepoint[3000] = { 0 };	//记录虚拟蛇走过的点
snakeNode* list = NULL;		//用带头结点的链表来描述蛇
enum map{ space, wall, snake, food };//地图上种类的枚举 space空白 wall墙 snake蛇 food食物
Point Food[1] = { 0 };	//食物点,如果是(0,0)则不是食物
Point* pBuff = NULL;	//记录space格子，排除掉墙壁，蛇，食物后的数组
bool isLive = true;	//默认蛇存活
int* pMap = NULL;	//地图

void initSnake();	//初始化蛇
void initMap();		//初始化地图
void initGame();	//初始化游戏
void showGame();	//显示游戏
void ctrolGame();	//控制游戏
void initFood();	//初始化食物
void updataMap();	//更新地图
bool vsnakeeat();	//放出一条虚拟蛇去吃食物
int* num();		//随机生成0—3的数字，表示蛇移动方向
void snake_move(int row, int col);					//蛇移动
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col);	//虚拟蛇移动
enum map isWhat(int row, int col);					//判断某个点是什么，并返回其值
enum map visWhat(snakeNode** vlist, int row, int col);			//判断虚拟蛇某个点是什么，并返回其值
void insertNode(int row, int col);					//头插法插入结点到蛇链表中
void vinsertNode(snakeNode** vlist, snakeNode** rear, int row, int col);	//头插法插入节点到虚拟蛇链表中
bool vsnake_have_space(int row, int col);				//检查此方向上蛇头附近是否还有space
bool vsnakefand(snakeNode** vlist, snakeNode**rear);			//虚拟蛇找到蛇尾



int main(){
	srand(time(0));		//生成随机数种子
	//设置蛇的头结点
	struct SnakeNode* head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode*));
	list = head;
	list->pos.row = NULL;
	list->pos.col = NULL;
	list->next = NULL;
	initGame();
	showGame();
	while (isLive){		//蛇存活，游戏继续，否则，游戏结束
		ctrolGame();
	}
	printf("GameOver\n");
	while (1);
	return 0;
}


//初始化游戏
void initGame(){
	initSnake();
	pBuff = malloc(sizeof(Point)*(ROW*COL - 2 * ROW - 2 * (COL - 2) - 3));		//设置buff空间，记录地图上所有有space格子
	initFood();
	initMap();
}


//初始化蛇，插入3个结点到链表中：分别是（2，2） （ 2，3） （ 2，4）
void initSnake(){
	for (int i = 0; i < 3; i++){
		insertNode(2, 2 + i);
	}
}


//初始化食物
void initFood(){
	//获取当前空白格子数量，初始化buff
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
	if (index == 0){		//没有空间放食物，游戏结束
		updataMap();
		printf("win!!!");
		while (1);
	}
	int suiji = rand() % index;	//随机产生食物，index为0的时候无法使用%
	Food[0].row = pBuff[suiji].row;
	Food[0].col = pBuff[suiji].col;
}


//初始化地图
void initMap(){
	pMap = (int*)malloc(sizeof(int)*ROW*COL);  		//地图数组
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			*(pMap + (i*COL) + j) = isWhat(i, j);	//二维转一维
		}
	}
}


//判断某个点是什么，并返回其值
enum map isWhat(int row, int col){
	//检查是否为墙壁
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//检查是否是蛇
	struct SnakeNode* temp = list->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col)  return snake;
		temp = temp->next;
	}

	//检查是否为食物
	if (Food[0].row == row && Food[0].col == col) return food;

	//都不是，返回space
	return space;
}


//虚拟蛇存在时，判断地图上的点是什么
enum map visWhat(snakeNode** vlist, int row, int col){
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//检查是否为食物
	if (Food[0].row == row && Food[0].col == col) {
		if (snakefood.row == -1){		//若此时不存在尾巴食物，即不在寻找尾巴位置时，该点为food
			return food;
		}
	}

	//检查是否是虚拟蛇
	struct SnakeNode* temp = (*vlist)->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col) return snake;
		temp = temp->next;
	}

	//都不是，返回space
	return space;
}


//更新地图
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
	printf("当前蛇长：%d\n", score);
}


//显示游戏
void showGame(){
	Point temlast;		//蛇尾
	snakeNode* tem = list->next;
	while (tem->next != NULL){
		tem = tem->next;
	}
	temlast.row = tem->pos.row;
	temlast.col = tem->pos.col;
	system("cls");
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			switch (*(pMap + (i*COL) + j)){		//二维转一维
			case wall: printf("#"); break;		//墙壁标识
			case space: printf(" "); break;		//空白格子
			case snake:
				if (i == list->next->pos.row && j == list->next->pos.col){
					printf("$");		//蛇头标识
					break;
				}
				if (i == temlast.row&&j == temlast.col){
					printf("O");		//蛇尾标识
					break;
				}
				else{
					printf("*");		//蛇身标识
					break;
				}
			case food: printf("@"); break;		//食物标识
			}
		}
		printf("\n");
	}
	//Sleep(200);	//设置地图刷新速度
}


//头插法插入结点到真实蛇链表中
void insertNode(int row, int col){
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = list->next;
	list->next = pNew;
}


//尾插法插入结点到虚拟蛇中
void vinsertNode(snakeNode** vlist, snakeNode**rear, int row, int col){
	struct SnakeNode* vpNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vpNew->pos.row = row;
	vpNew->pos.col = col;
	vpNew->next = (*rear)->next;
	(*rear)->next = vpNew;
	*rear = vpNew;
}


//真实蛇移动
void snake_move(int row, int col){
	//去尾
	struct SnakeNode* pTemp = list;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	insertNode(row, col);
}


//虚拟蛇移动一格
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col){
	//去尾
	struct SnakeNode* pTemp = *vlist;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	*rear = pTemp;
	//头插法插入一个结点
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//申请一个新的结点
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = (*vlist)->next;
	(*vlist)->next = pNew;
}


//随机生成0—3之间不重复的4个数字
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
		if (tag == 0){		//标记为0表示已生成该数字，跳过该次循环
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


//控制游戏
void ctrolGame(){
	if (vsnakeeat()){		//虚拟蛇能吃到食物，且吃完食物后能找到尾巴
		//printf("虚拟蛇能吃到食物，且吃完食物后能找到尾巴");
		//Sleep(1000);
		int i = 0;
		while (movepoint[i].row != -1){		//真实蛇根据虚拟蛇记录的位置移动
			switch (isWhat(movepoint[i].row, movepoint[i].col))
			{
			case food:
				insertNode(movepoint[i].row, movepoint[i].col);	//头插法插入一个结点到蛇里面
				initFood();
				break;
			case snake:			//咬尾巴
			case space:
				snake_move(movepoint[i].row, movepoint[i].col);
				break;
			}
			updataMap();
			i++;
		}
	}
	else{	//虚拟蛇未吃到食物 或 吃到食物后找不到尾巴。此时让真实蛇远离食物 且 所找的路径能找到尾巴
		Point listlast;			//定义真实蛇的尾部
		snakeNode* temp = list->next;
		while (temp->next != NULL){
			temp = temp->next;
		}
		listlast.row = temp->pos.row;
		listlast.col = temp->pos.col;
		Point dir[4] = { 0 };		//存储蛇头的上下左右位置坐标的数组,0上  1右  2下  3左
		int maxH = -1;		//设置一个H值标志，初始为一个较小的maxH，以便找到四个方向中离食物最远(H值最大)的且能找到尾巴食物的方向点
		int max = -1;		//记录以找到的有最大H值的点，默认为-1，若max值未改变则证明未找到虚拟蛇可到达尾巴食物的方向点
		int backupmaxH = -1;	//若该方向点虚拟蛇找不到尾巴食物，则使用此记录的数据，向离食物最远的地方移动
		int backupmax = -1;	//记录以找到的有最大H值的点，默认为-1，若backupmax值未改变则证明未找到虚拟蛇可到达尾巴食物的方向点 且 方向点周围没有space和food

		//添加上右下左的坐标，即方向点
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: dir[i].row = list->next->pos.row - 1; dir[i].col = list->next->pos.col;		break;
			case 1: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col + 1;	break;
			case 2: dir[i].row = list->next->pos.row + 1; dir[i].col = list->next->pos.col;		break;
			case 3: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col - 1;	break;
			}
		}

		//找出能找到尾巴且离食物最远的点
		int *number = num();		//随机产生方向
		for (int n = 0; n < 4; n++){
			int i = *(number + n);

			//每次改变方向就重新生成一条与真实蛇相同的虚拟蛇
			snakeNode* vlist = NULL;	//用带头结点链表来描述虚拟蛇
			struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
			vhead->pos.row = NULL;
			vhead->pos.col = NULL;
			vhead->next = NULL;
			vlist = vhead;
			snakeNode* rear = vlist;	//虚拟蛇尾部

			//生成与真实蛇一样的虚拟蛇
			struct SnakeNode* tmp = list->next;
			while (tmp != NULL){
				vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
				tmp = tmp->next;
			}

			//跳过周围不能走的点
			if ((dir[i].row != listlast.row || dir[i].col != listlast.col) && (isWhat(dir[i].row, dir[i].col) == snake || isWhat(dir[i].row, dir[i].col) == wall)){
				continue;	//若该方向点不与真实蛇尾相同 且 是虚拟蛇身或墙 则跳过
			}
			vsnake_move(&vlist, &rear, dir[i].row, dir[i].col);//虚拟蛇向该方向点移一步

			//找能找到尾部的点
			snakefood.row = rear->pos.row;			//设置尾巴食物为当前虚拟蛇的尾巴，让虚拟蛇去吃尾巴食物
			snakefood.col = rear->pos.col;
			if (vsnakefand(&vlist, &rear) == false){	//虚拟蛇找不到尾巴食物
				if (vsnake_have_space(dir[i].row, dir[i].col)){		//如果此方向点周围还有space、food
					int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//计算H值,找出离食物最远的方向点
					if (H > backupmaxH){
						backupmaxH = H;
						backupmax = i;
						//Sleep(1000);
						continue;		//开始判断下一个方向点
					}
				}
				else{	//此方向点周围没有space、food，现在判断该方向点是否为食物
					if (isWhat(dir[i].row, dir[i].col) == food){	//如果此方向为食物，判断该食物是否为能产生的最后一个食物
						//printf("判断出该方向为食物i=%d\n",i);
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
						if (index == 0){			//space格子为0，说明只剩该方向最后一个食物
							insertNode(dir[i].row, dir[i].col);
							initFood();			//最后一个食物被吃掉，此时初始化食物，结束游戏
						}
					}
					else{
						continue;	//此方向点不为食物，开始判断下一个方向点
					}
				}

			}
			else{	//虚拟蛇能找到尾巴食物，取离食物food最远的方向点	
				int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//计算H值
				if (H > maxH){
					maxH = H;
					max = i;
				}
			}
		}//遍历完了四个方向点

		if (max == -1){		//没有能找到尾巴食物的方向点
			if (backupmax == -1){	//没有 周围有space、food 的方向点
				int *number = num();	//随机产生方向
				for (int n = 0; n < 4; n++){
					int i = *(number + n);
					if (isWhat(dir[i].row, dir[i].col) == space){ //此时游戏已经输了，但为了能走完此时能走的点，选择让蛇往有space或food的方向移动
						snake_move(dir[i].row, dir[i].col);
						printf("虽然知道输了，但我还是要走完能走的点");
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

		if (isWhat(dir[max].row, dir[max].col) == space || isWhat(dir[max].row, dir[max].col) == snake){//若是space则真实蛇移动，若是snake则吃尾巴	
			if (isWhat(dir[max].row, dir[max].col) == snake){
				//printf("我咬尾巴了");
				//Sleep(1000);
			}
			snake_move(dir[max].row, dir[max].col);
			updataMap();
		}
		else{
<<<<<<< HEAD
			printf("�ҳ�ʳ����");
			Sleep(10000);
			insertNode(dir[max].row, dir[max].col);	//�˷����Ϊʳ�ͷ�巨����һ����㵽������
=======
			insertNode(dir[max].row, dir[max].col);	//此方向点为食物，头插法插入一个结点到蛇里面
>>>>>>> f906844a6fb70729732ebfd97f099e45e06895e7
			initFood();
			updataMap();
		}
	}
}


//AI实现的部分
bool vsnakeeat(){				//放出一条虚拟蛇去吃食物
	int index = 0;				//虚拟蛇走过的点的数组下标
	//生成虚拟蛇
	snakeNode* vlist = NULL;		//用带头结点链表来描述虚拟蛇
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//虚拟蛇蛇尾

	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}


	//移动虚拟蛇去吃食物
	Point vdir[4] = { 0 };			//存储虚拟蛇头的上下左右位置坐标的数组,0上  1右  2下  3左
	while (visWhat(&vlist, vlist->next->pos.row, vlist->next->pos.col) != food){
		int vfoodminH = ROW + COL;	//设置一个H值标志，默认为最大的H，以便找到四个方向中离食物最近的点
		int vmin = -1;			//记录以找到的有最小H值的点，默认为-1，若vmin值未改变则证明未找到可走的点
		int vsnakemaxH = -1;		//设置一个小的值，以便找到离蛇尾最大的方向点

		//添加虚拟蛇头上右下左的坐标，即方向点
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = vlist->next->pos.row - 1; vdir[i].col = vlist->next->pos.col;		break;
			case 1: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col + 1;	break;
			case 2: vdir[i].row = vlist->next->pos.row + 1; vdir[i].col = vlist->next->pos.col;		break;
			case 3: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col - 1;	break;
			}
		}

		//计算可走点中有最小H值的点
		int *number = num();			//随机产生方向
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != rear->pos.row || vdir[i].col != rear->pos.col) && (visWhat(&vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&vlist, vdir[i].row, vdir[i].col) == wall)){	//若该点不是虚拟蛇尾 且 是虚拟蛇身或墙 则跳过
				//printf("跳过(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(5000);
				continue;		//跳过不满足条件的方向点
			}
			int vfoodH = abs(vdir[i].row - Food[0].row) + abs(vdir[i].col - Food[0].col);			//计算该方向点到食物的H值

			if (vfoodH < vfoodminH){
				vfoodminH = vfoodH;	//找出离食物最近的方向点
				vmin = i;
				vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);	//计算该方向点到蛇尾的H值
			}
			if (vfoodH == vfoodminH){	//若两个方向点离食物有同样的H值，则选择其中离蛇尾远的方向点
				if ((abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col)) > vsnakemaxH){
					vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);
					vmin = i;
				}
			}
		}//遍历完四个方向点


		if (vmin == -1){			//没找到满足条件的方向点	
			return false;			//返回false，未找到可走的点
		}

		if (visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == space || visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == snake){
			vsnake_move(&vlist, &rear, vdir[vmin].row, vdir[vmin].col);		//若为space，移动虚拟蛇；若为snake，表示该点为蛇尾，移动虚拟蛇
		}
		else{																//该点为food，头插法插入该点到虚拟蛇
			struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//申请一个新的结点
			pNew->pos.row = vdir[vmin].row;
			pNew->pos.col = vdir[vmin].col;
			//pNew->next = NULL;		//视频中没写这句
			pNew->next = vlist->next;
			vlist->next = pNew;
		}
		//system("cls");
		//printf("写入路径中");
		movepoint[index].row = vdir[vmin].row;		//将走过的点记录在数组中
		movepoint[index].col = vdir[vmin].col;
		index++;
		if (index > 20000){
			//printf("找了很久没找到，直接返回");
			//Sleep(700);
			return false;
		}
	}

	//判断此时虚拟蛇能否找到蛇尾
	snakefood.row = rear->pos.row;
	snakefood.col = rear->pos.col;
	if (vsnakefand(&vlist, &rear)){
		movepoint[index].row = -1;		//标记位，真实蛇只走 存储点（-1，-1）之前的所存的点
		movepoint[index].col = -1;
		//printf("虚拟蛇吃到食物后找到尾巴了，我要告诉真实蛇了，让它去吃食物");
		//Sleep(5000);
		//printf("虚拟蛇吃完食物找到路径\n");
		//Sleep(5000);
		return true;
	}
	else{
		//printf("虚拟蛇吃到食物后未找到尾巴，我要告诉真实蛇，让它先别去吃食物，换个安全的位置再吃");
		//Sleep(5000);
		//printf("虚拟蛇吃完食物未找到路径\n");
		//Sleep(5000);
		return false;
	}
}


bool vsnakefand(snakeNode** vlist, snakeNode**rear){		//虚拟蛇找尾巴食物
	Point vdir[4] = { 0 };		//存储蛇头的上下左右位置坐标的数组,0上  1右  2下  3左
	while ((*vlist)->next->pos.row != snakefood.row || (*vlist)->next->pos.col != snakefood.col){		//虚拟蛇的蛇头是否到达最初记录的蛇尾位置
		int vminH = ROW + COL;				//设置一个H值标志，默认为最大的H，以便找到四个方向中有最小H值的点
		int vmin = -1;						//记录以找到的有最小H值的点，默认为-1，若vmin值未改变则证明未找到可走的点

		//添加上右下左的坐标
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = (*vlist)->next->pos.row - 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 1: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col + 1;	break;
			case 2: vdir[i].row = (*vlist)->next->pos.row + 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 3: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col - 1;	break;
			}
		}

		//计算可走点中有最小H值的点
		int *number = num();		//随机产生方向
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != snakefood.row || vdir[i].col != snakefood.col) && (visWhat(&*vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&*vlist, vdir[i].row, vdir[i].col) == wall)){	//若该点不是尾巴食物snakefood 且 是虚拟蛇身或墙 则跳过
				//printf("跳过！！(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(1000);
				continue;
			}
			int H = abs(vdir[i].row - snakefood.row) + abs(vdir[i].col - snakefood.col);		//计算H值
			if (H < vminH){
				vminH = H;
				vmin = i;
			}
		}
		if (vmin == -1){
			//printf("现在在虚拟蛇找尾巴食物当中，未找到，我要返回了");
			//Sleep(5000);
			snakefood.row = -1;	//重置snakefood
			snakefood.col = -1;
			return false;		//未找到可走的点
		}
		vsnake_move(&*vlist, &*rear, vdir[vmin].row, vdir[vmin].col);		//找到可走的一个点，移动虚拟蛇
	}
	snakefood.row = -1;	//重置snakefood
	snakefood.col = -1;
	return true;

}


//检查此方向上蛇头附近是否还有space、food
bool vsnake_have_space(int row, int col){
	snakeNode* vlist = NULL;		//用带头结点链表来描述虚拟蛇
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//尾指针
	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}
	vsnake_move(&vlist, &rear, row, col);		//虚拟蛇往此方向移一格
	Point exam[4] = { 0 };
	for (int i = 0; i < 4; i++){
		switch (i){
		case 0: exam[i].row = vlist->next->pos.row - 1; 	exam[i].col = vlist->next->pos.col;	break;			/////改改
		case 1: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col + 1;	break;
		case 2: exam[i].row = vlist->next->pos.row + 1; 	exam[i].col = vlist->next->pos.col;	break;
		case 3: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col - 1;	break;
		}
	}
	int *number = num();		//随机产生方向
	for (int n = 0; n < 4; n++){
		int i = *(number + n);
		if (visWhat(&vlist, exam[i].row, exam[i].col) == space || visWhat(&vlist, exam[i].row, exam[i].col) == food){
			return true;
		}
	}
	return false;
}



