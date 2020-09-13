#include <stdio.h>
#include <stdlib.h>	//malloc()�賣
#include <stdbool.h>
#include <conio.h>	//_kbhit()�賣�閬�
#include <windows.h>	//sleep()�賣�閬�
#define ROW 12 		//�啣擃�
#define COL 12		//�啣摰�

//�寧掩��霈啣��寧�璅芰熊��
typedef struct Mypoint{
	int row;
	int col;
}Point;

//����
typedef struct SnakeNode{
	Point pos;
	struct SnakeNode* next;
}snakeNode;

Point snakefood = { -1 };		//撠曉毀憌嚗����閬��撠曆�蝵殷�撠�撠曉�雿��抬�暺恕銝�-1嚗”蝷箸瓷�偏撌湧���
Point movepoint[3000] = { 0 };	//霈啣����粥餈���
snakeNode* list = NULL;		//�典蒂憭渡��寧��曇”�交�餈啗�
enum map{ space, wall, snake, food };//�啣銝�蝐餌��蜀 space蝛箇 wall憓� snake�� food憌
Point Food[1] = { 0 };	//憌��,憒���(0,0)���舫���
Point* pBuff = NULL;	//霈啣�space�澆�嚗��斗�憓�嚗�嚗��拙��蝏�
bool isLive = true;	//暺恕��瘣�
int* pMap = NULL;	//�啣

void initSnake();	//����
void initMap();		//�����
void initGame();	//���虜��
void showGame();	//�曄內皜豢�
void ctrolGame();	//�批皜豢�
void initFood();	//������
void updataMap();	//�湔�啣
bool vsnakeeat();	//�曉銝�∟����餃�憌
int* num();		//���0��3�摮�銵函內�宏�冽��
void snake_move(int row, int col);					//�宏��
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col);	//���宏��
enum map isWhat(int row, int col);					//�斗�葵�寞隞銋�撟嗉����
enum map visWhat(snakeNode** vlist, int row, int col);			//�斗����銝芰�臭�銋�撟嗉����
void insertNode(int row, int col);					//憭湔�瘜��亦��孵�銵其葉
void vinsertNode(snakeNode** vlist, snakeNode** rear, int row, int col);	//憭湔�瘜��亥��孵���銵其葉
bool vsnake_have_space(int row, int col);				//璉�交迨�孵�銝�憭湧�餈�西��pace
bool vsnakefand(snakeNode** vlist, snakeNode**rear);			//����啗�撠�



int main(){
	srand(time(0));		//����啁�摮�
	//霈曄蔭��憭渡���
	struct SnakeNode* head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode*));
	list = head;
	list->pos.row = NULL;
	list->pos.col = NULL;
	list->next = NULL;
	initGame();
	showGame();
	while (isLive){		//��瘣鳴�皜豢�蝏抒賒嚗��皜豢�蝏�
		ctrolGame();
	}
	printf("GameOver\n");
	while (1);
	return 0;
}


//���虜��
void initGame(){
	initSnake();
	pBuff = malloc(sizeof(Point)*(ROW*COL - 2 * ROW - 2 * (COL - 2) - 3));		//霈曄蔭buff蝛粹嚗扇敶�曆����space�澆�
	initFood();
	initMap();
}


//����嚗���3銝芰��孵�曇”銝哨���荔�2嚗�2嚗� 嚗� 2嚗�3嚗� 嚗� 2嚗�4嚗�
void initSnake(){
	for (int i = 0; i < 3; i++){
		insertNode(2, 2 + i);
	}
}


//������
void initFood(){
	//�瑕�敶�蝛箇�澆��圈�嚗�憪�buff
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
	if (index == 0){		//瘝⊥�蝛粹�暸��抬�皜豢�蝏�
		updataMap();
		printf("win!!!");
		while (1);
	}
	int suiji = rand() % index;	//�鈭抒�憌嚗ndex銝�0���瘜蝙��%
	Food[0].row = pBuff[suiji].row;
	Food[0].col = pBuff[suiji].col;
}


//�����
void initMap(){
	pMap = (int*)malloc(sizeof(int)*ROW*COL);  		//�啣�啁�
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			*(pMap + (i*COL) + j) = isWhat(i, j);	//鈭輕頧砌�蝏�
		}
	}
}


//�斗�葵�寞隞銋�撟嗉����
enum map isWhat(int row, int col){
	//璉�交�虫蛹憓�
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//璉�交�行��
	struct SnakeNode* temp = list->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col)  return snake;
		temp = temp->next;
	}

	//璉�交�虫蛹憌
	if (Food[0].row == row && Food[0].col == col) return food;

	//�賭��荔�餈�space
	return space;
}


//�����冽嚗�剖�曆���臭�銋�
enum map visWhat(snakeNode** vlist, int row, int col){
	if (row == 0 || row == (ROW - 1) || col == 0 || col == (COL - 1))
		return wall;

	//璉�交�虫蛹憌
	if (Food[0].row == row && Food[0].col == col) {
		if (snakefood.row == -1){		//�交迨�嗡�摮撠曉毀憌嚗銝撖餅撠曉毀雿蔭�塚�霂亦銝榻ood
			return food;
		}
	}

	//璉�交�行����
	struct SnakeNode* temp = (*vlist)->next;
	while (temp){
		if (temp->pos.row == row && temp->pos.col == col) return snake;
		temp = temp->next;
	}

	//�賭��荔�餈�space
	return space;
}


//�湔�啣
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
	printf("敶��嚗�%d\n", score);
}


//�曄內皜豢�
void showGame(){
	Point temlast;		//�偏
	snakeNode* tem = list->next;
	while (tem->next != NULL){
		tem = tem->next;
	}
	temlast.row = tem->pos.row;
	temlast.col = tem->pos.col;
	system("cls");
	for (int i = 0; i < ROW; i++){
		for (int j = 0; j < COL; j++){
			switch (*(pMap + (i*COL) + j)){		//鈭輕頧砌�蝏�
			case wall: printf("#"); break;		//憓���
			case space: printf(" "); break;		//蝛箇�澆�
			case snake:
				if (i == list->next->pos.row && j == list->next->pos.col){
					printf("$");		//�仍��
					break;
				}
				if (i == temlast.row&&j == temlast.col){
					printf("O");		//�偏��
					break;
				}
				else{
					printf("*");		//�澈��
					break;
				}
			case food: printf("@"); break;		//憌��
			}
		}
		printf("\n");
	}
	//Sleep(200);	//霈曄蔭�啣�瑟�漲
}


//憭湔�瘜��亦��孵���銵其葉
void insertNode(int row, int col){
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = list->next;
	list->next = pNew;
}


//撠暹�瘜��亦��孵���葉
void vinsertNode(snakeNode** vlist, snakeNode**rear, int row, int col){
	struct SnakeNode* vpNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vpNew->pos.row = row;
	vpNew->pos.col = col;
	vpNew->next = (*rear)->next;
	(*rear)->next = vpNew;
	*rear = vpNew;
}


//���宏��
void snake_move(int row, int col){
	//�餃偏
	struct SnakeNode* pTemp = list;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	insertNode(row, col);
}


//���宏�其���
void vsnake_move(snakeNode** vlist, snakeNode**rear, int row, int col){
	//�餃偏
	struct SnakeNode* pTemp = *vlist;
	while (pTemp->next->next != NULL){
		pTemp = pTemp->next;
	}
	free(pTemp->next);
	pTemp->next = NULL;
	*rear = pTemp;
	//憭湔�瘜��乩�銝芰���
	struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//�唾窈銝銝芣����
	pNew->pos.row = row;
	pNew->pos.col = col;
	pNew->next = (*vlist)->next;
	(*vlist)->next = pNew;
}


//���0��3銋銝�憭�4銝芣摮�
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
		if (tag == 0){		//�扇銝�0銵函內撌脩��砲�啣�嚗歲餈砲甈∪儐��
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


//�批皜豢�
void ctrolGame(){
	if (vsnakeeat()){		//����憌嚗���憌��曉撠曉毀
		//printf("����憌嚗���憌��曉撠曉毀");
		//Sleep(1000);
		int i = 0;
		while (movepoint[i].row != -1){		//����株���霈啣���蝵桃宏��
			switch (isWhat(movepoint[i].row, movepoint[i].col))
			{
			case food:
				insertNode(movepoint[i].row, movepoint[i].col);	//憭湔�瘜��乩�銝芰��孵����
				initFood();
				break;
			case snake:			//�砍偏撌�
			case space:
				snake_move(movepoint[i].row, movepoint[i].col);
				break;
			}
			updataMap();
			i++;
		}
	}
	else{	//����憌 �� �憌�銝撠曉毀�迨�嗉悟����蝳駁��� 銝� ��曄�頝臬��賣�啣偏撌�
		Point listlast;			//摰�����撠暸
		snakeNode* temp = list->next;
		while (temp->next != NULL){
			temp = temp->next;
		}
		listlast.row = temp->pos.row;
		listlast.col = temp->pos.col;
		Point dir[4] = { 0 };		//摮�仍��銝椰�喃�蝵桀����啁�,0銝�  1��  2銝�  3撌�
		int maxH = -1;		//霈曄蔭銝銝杳�潭�敹���銝箔�銝芾�撠�maxH嚗誑靘踵�啣�銝芣�葉蝳駁��拇�餈�(H�潭�憭�)���賣�啣偏撌湧��拍��孵���
		int max = -1;		//霈啣�隞交�啁���憭佚�潛��對�暺恕銝�-1嚗max�潭�孵�����曉����啗噢撠曉毀憌��
		int backupmaxH = -1;	//�亥砲�孵��寡����曆��啣偏撌湧��抬��蝙�冽迨霈啣���殷��氖憌�餈��唳蝘餃
		int backupmax = -1;	//霈啣�隞交�啁���憭佚�潛��對�暺恕銝�-1嚗backupmax�潭�孵�����曉����啗噢撠曉毀憌�� 銝� �孵��孵�湔瓷�pace�ood

		//瘛餃�銝銝椰�����單�
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: dir[i].row = list->next->pos.row - 1; dir[i].col = list->next->pos.col;		break;
			case 1: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col + 1;	break;
			case 2: dir[i].row = list->next->pos.row + 1; dir[i].col = list->next->pos.col;		break;
			case 3: dir[i].row = list->next->pos.row;		dir[i].col = list->next->pos.col - 1;	break;
			}
		}

		//�曉�賣�啣偏撌港�蝳駁��拇�餈���
		int *number = num();		//�鈭抒��孵�
		for (int n = 0; n < 4; n++){
			int i = *(number + n);

			//瘥活�孵��孵�撠梢��啁����∩����������
			snakeNode* vlist = NULL;	//�典蒂憭渡��寥銵冽�膩����
			struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
			vhead->pos.row = NULL;
			vhead->pos.col = NULL;
			vhead->next = NULL;
			vlist = vhead;
			snakeNode* rear = vlist;	//���偏��

			//��銝�摰�銝�瑞�����
			struct SnakeNode* tmp = list->next;
			while (tmp != NULL){
				vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
				tmp = tmp->next;
			}

			//頝唾��典銝韏啁���
			if ((dir[i].row != listlast.row || dir[i].col != listlast.col) && (isWhat(dir[i].row, dir[i].col) == snake || isWhat(dir[i].row, dir[i].col) == wall)){
				continue;	//�亥砲�孵��嫣�銝�摰�撠曄�� 銝� �航���頨急�憓� �歲餈�
			}
			vsnake_move(&vlist, &rear, dir[i].row, dir[i].col);//����霂交�蝘颱�甇�

			//�曇�曉撠暸�
			snakefood.row = rear->pos.row;			//霈曄蔭撠曉毀憌銝箏������偏撌湛�霈抵����餃�撠曉毀憌
			snakefood.col = rear->pos.col;
			if (vsnakefand(&vlist, &rear) == false){	//���銝撠曉毀憌
				if (vsnake_have_space(dir[i].row, dir[i].col)){		//憒�甇斗��典餈�space�ood
					int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//霈∠�H��,�曉蝳駁��拇�餈��孵���
					if (H > backupmaxH){
						backupmaxH = H;
						backupmax = i;
						//Sleep(1000);
						continue;		//撘憪�凋�銝銝芣�
					}
				}
				else{	//甇斗��典瘝⊥�space�ood嚗�典�剛砲�孵��寞�虫蛹憌
					if (isWhat(dir[i].row, dir[i].col) == food){	//憒�甇斗�蛹憌嚗�剛砲憌�臬銝箄鈭抒�����銝芷���
						//printf("�斗�箄砲�孵�銝粹��姆=%d\n",i);
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
						if (index == 0){			//space�澆�銝�0嚗秩��抵砲�孵����銝芷���
							insertNode(dir[i].row, dir[i].col);
							initFood();			//���銝芷��抵◤��嚗迨�嗅�憪�憌嚗��虜��
						}
					}
					else{
						continue;	//甇斗�銝蛹憌嚗�憪�凋�銝銝芣�
					}
				}

			}
			else{	//����曉撠曉毀憌嚗�蝳駁��妹ood�餈��孵���	
				int H = abs(dir[i].row - Food[0].row) + abs(dir[i].col - Food[0].col);		//霈∠�H��
				if (H > maxH){
					maxH = H;
					max = i;
				}
			}
		}//��摰��葵�孵���

		if (max == -1){		//瘝⊥��賣�啣偏撌湧��拍��孵���
			if (backupmax == -1){	//瘝⊥� �典�pace�ood ��
				int *number = num();	//�鈭抒��孵�
				for (int n = 0; n < 4; n++){
					int i = *(number + n);
					if (isWhat(dir[i].row, dir[i].col) == space){ //甇斗皜豢�撌脩�颲�嚗�銝箔��質粥摰迨�嗉韏啁��對��霈抵�敺�pace�ood��宏��
						snake_move(dir[i].row, dir[i].col);
						printf("�賜�仿�颲�嚗����航�韏啣��質粥�");
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

		if (isWhat(dir[max].row, dir[max].col) == space || isWhat(dir[max].row, dir[max].col) == snake){//�交space��摰�蝘餃嚗�盎nake��撠曉毀	
			if (isWhat(dir[max].row, dir[max].col) == snake){
				//printf("�撠曉毀鈭�");
				//Sleep(1000);
			}
			snake_move(dir[max].row, dir[max].col);
			updataMap();
		}
		else{
<<<<<<< HEAD
			printf("扂勛妘昜賸");
			Sleep(10000);
			insertNode(dir[max].row, dir[max].col);	//森源砃萸峈妘昜ㄛ芛脣楊脣珨跺賦萸善彴爵醱
=======
			insertNode(dir[max].row, dir[max].col);	//甇斗�銝粹��抬�憭湔�瘜��乩�銝芰��孵����
>>>>>>> f906844a6fb70729732ebfd97f099e45e06895e7
			initFood();
			updataMap();
		}
	}
}


//AI摰���
bool vsnakeeat(){				//�曉銝�∟����餃�憌
	int index = 0;				//���粥餈��寧��啁�銝�
	//������
	snakeNode* vlist = NULL;		//�典蒂憭渡��寥銵冽�膩����
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//����撠�

	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}


	//蝘餃�������
	Point vdir[4] = { 0 };			//摮���仍��銝椰�喃�蝵桀����啁�,0銝�  1��  2銝�  3撌�
	while (visWhat(&vlist, vlist->next->pos.row, vlist->next->pos.col) != food){
		int vfoodminH = ROW + COL;	//霈曄蔭銝銝杳�潭�敹�暺恕銝箸�憭抒�H嚗誑靘踵�啣�銝芣�葉蝳駁��拇�餈���
		int vmin = -1;			//霈啣�隞交�啁���撠�潛��對�暺恕銝�-1嚗vmin�潭�孵�����曉�航粥�
		int vsnakemaxH = -1;		//霈曄蔭銝銝芸��潘�隞乩噶�曉蝳餉�撠暹�憭抒��孵���

		//瘛餃����仍銝銝椰�����單�
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = vlist->next->pos.row - 1; vdir[i].col = vlist->next->pos.col;		break;
			case 1: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col + 1;	break;
			case 2: vdir[i].row = vlist->next->pos.row + 1; vdir[i].col = vlist->next->pos.col;		break;
			case 3: vdir[i].row = vlist->next->pos.row;		vdir[i].col = vlist->next->pos.col - 1;	break;
			}
		}

		//霈∠��航粥�嫣葉��撠�潛���
		int *number = num();			//�鈭抒��孵�
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != rear->pos.row || vdir[i].col != rear->pos.col) && (visWhat(&vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&vlist, vdir[i].row, vdir[i].col) == wall)){	//�亥砲�嫣��航���撠� 銝� �航���頨急�憓� �歲餈�
				//printf("頝唾�(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(5000);
				continue;		//頝唾�銝說頞單隞嗥��孵���
			}
			int vfoodH = abs(vdir[i].row - Food[0].row) + abs(vdir[i].col - Food[0].col);			//霈∠�霂交��圈��拍�H��

			if (vfoodH < vfoodminH){
				vfoodminH = vfoodH;	//�曉蝳駁��拇�餈��孵���
				vmin = i;
				vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);	//霈∠�霂交��啗�撠曄�H��
			}
			if (vfoodH == vfoodminH){	//�乩舅銝芣�蝳駁��拇����潘���嗡葉蝳餉�撠曇���
				if ((abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col)) > vsnakemaxH){
					vsnakemaxH = abs(vdir[i].row - rear->pos.row) + abs(vdir[i].col - rear->pos.col);
					vmin = i;
				}
			}
		}//��摰�銝芣�


		if (vmin == -1){			//瘝⊥�唳說頞單隞嗥��孵���	
			return false;			//餈�false嚗�曉�航粥�
		}

		if (visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == space || visWhat(&vlist, vdir[vmin].row, vdir[vmin].col) == snake){
			vsnake_move(&vlist, &rear, vdir[vmin].row, vdir[vmin].col);		//�乩蛹space嚗宏�刻���嚗銝漳nake嚗”蝷箄砲�嫣蛹�偏嚗宏�刻���
		}
		else{																//霂亦銝榻ood嚗仍���霂亦�啗���
			struct SnakeNode* pNew = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));		//�唾窈銝銝芣����
			pNew->pos.row = vdir[vmin].row;
			pNew->pos.col = vdir[vmin].col;
			//pNew->next = NULL;		//閫�銝剜瓷����
			pNew->next = vlist->next;
			vlist->next = pNew;
		}
		//system("cls");
		//printf("�頝臬�銝�");
		movepoint[index].row = vdir[vmin].row;		//撠粥餈��寡扇敶�啁�銝�
		movepoint[index].col = vdir[vmin].col;
		index++;
		if (index > 20000){
			//printf("�曆�敺�瘝⊥�堆��湔餈�");
			//Sleep(700);
			return false;
		}
	}

	//�斗甇斗����行�啗�撠�
	snakefood.row = rear->pos.row;
	snakefood.col = rear->pos.col;
	if (vsnakefand(&vlist, &rear)){
		movepoint[index].row = -1;		//�扇雿����韏� 摮�對�-1嚗�-1嚗����摮���
		movepoint[index].col = -1;
		//printf("�����圈��拙��曉撠曉毀鈭���������嚗悟摰����");
		//Sleep(5000);
		//printf("����摰��拇�啗楝敺n");
		//Sleep(5000);
		return true;
	}
	else{
		//printf("�����圈��拙��芣�啣偏撌湛���������霈拙���餃�憌嚗銝芸��函�雿蔭��");
		//Sleep(5000);
		//printf("����摰��拇�曉頝臬�\n");
		//Sleep(5000);
		return false;
	}
}


bool vsnakefand(snakeNode** vlist, snakeNode**rear){		//���撠曉毀憌
	Point vdir[4] = { 0 };		//摮�仍��銝椰�喃�蝵桀����啁�,0銝�  1��  2銝�  3撌�
	while ((*vlist)->next->pos.row != snakefood.row || (*vlist)->next->pos.col != snakefood.col){		//�����仍�臬�啗噢��扇敶��偏雿蔭
		int vminH = ROW + COL;				//霈曄蔭銝銝杳�潭�敹�暺恕銝箸�憭抒�H嚗誑靘踵�啣�銝芣�葉��撠�潛���
		int vmin = -1;						//霈啣�隞交�啁���撠�潛��對�暺恕銝�-1嚗vmin�潭�孵�����曉�航粥�

		//瘛餃�銝銝椰����
		for (int i = 0; i < 4; i++){
			switch (i){
			case 0: vdir[i].row = (*vlist)->next->pos.row - 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 1: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col + 1;	break;
			case 2: vdir[i].row = (*vlist)->next->pos.row + 1; vdir[i].col = (*vlist)->next->pos.col;		break;
			case 3: vdir[i].row = (*vlist)->next->pos.row;		vdir[i].col = (*vlist)->next->pos.col - 1;	break;
			}
		}

		//霈∠��航粥�嫣葉��撠�潛���
		int *number = num();		//�鈭抒��孵�
		for (int n = 0; n < 4; n++){
			int i = *(number + n);
			if ((vdir[i].row != snakefood.row || vdir[i].col != snakefood.col) && (visWhat(&*vlist, vdir[i].row, vdir[i].col) == snake || visWhat(&*vlist, vdir[i].row, vdir[i].col) == wall)){	//�亥砲�嫣��臬偏撌湧��孟nakefood 銝� �航���頨急�憓� �歲餈�
				//printf("頝唾�嚗�(%d,%d)", vdir[i].row, vdir[i].col);
				//Sleep(1000);
				continue;
			}
			int H = abs(vdir[i].row - snakefood.row) + abs(vdir[i].col - snakefood.col);		//霈∠�H��
			if (H < vminH){
				vminH = H;
				vmin = i;
			}
		}
		if (vmin == -1){
			//printf("�啣�刻����曉偏撌湧��拙�銝哨��芣�堆���餈�鈭�");
			//Sleep(5000);
			snakefood.row = -1;	//�蔭snakefood
			snakefood.col = -1;
			return false;		//�芣�啣韏啁���
		}
		vsnake_move(&*vlist, &*rear, vdir[vmin].row, vdir[vmin].col);		//�曉�航粥��銝芰嚗宏�刻���
	}
	snakefood.row = -1;	//�蔭snakefood
	snakefood.col = -1;
	return true;

}


//璉�交迨�孵�銝�憭湧�餈�西��pace�ood
bool vsnake_have_space(int row, int col){
	snakeNode* vlist = NULL;		//�典蒂憭渡��寥銵冽�膩����
	struct SnakeNode* vhead = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
	vhead->pos.row = NULL;
	vhead->pos.col = NULL;
	vhead->next = NULL;
	vlist = vhead;
	snakeNode* rear = vlist;		//撠暹���
	struct SnakeNode* tmp = list->next;
	while (tmp != NULL){
		vinsertNode(&vlist, &rear, tmp->pos.row, tmp->pos.col);
		tmp = tmp->next;
	}
	vsnake_move(&vlist, &rear, row, col);		//����甇斗�宏銝��
	Point exam[4] = { 0 };
	for (int i = 0; i < 4; i++){
		switch (i){
		case 0: exam[i].row = vlist->next->pos.row - 1; 	exam[i].col = vlist->next->pos.col;	break;			/////�寞
		case 1: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col + 1;	break;
		case 2: exam[i].row = vlist->next->pos.row + 1; 	exam[i].col = vlist->next->pos.col;	break;
		case 3: exam[i].row = vlist->next->pos.row;		exam[i].col = vlist->next->pos.col - 1;	break;
		}
	}
	int *number = num();		//�鈭抒��孵�
	for (int n = 0; n < 4; n++){
		int i = *(number + n);
		if (visWhat(&vlist, exam[i].row, exam[i].col) == space || visWhat(&vlist, exam[i].row, exam[i].col) == food){
			return true;
		}
	}
	
	return false;
}



