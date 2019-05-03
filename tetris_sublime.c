#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

int stage[21][12] = { 0 };
int block[4][4] = { 0 };
int nextblock[4][4] = { 0 };
int field[21][12] = { 0 };
int holdblock[4][4] = { 0 };
int tempblock[4][4] = { 0 };
int block_type;



int block_list[7][4][4] =
{ { { 0,1,0,0 },{ 0,1,0,0 },{ 0,1,0,0 },{ 0,1,0,0 } },
{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,0,0 },{ 0,1,0,0 } },
{ { 0,0,1,0 },{ 0,1,1,0 },{ 0,1,0,0 },{ 0,0,0,0 } },
{ { 0,1,0,0 },{ 0,1,1,0 },{ 0,0,1,0 },{ 0,0,0,0 } },
{ { 0,0,0,0 },{ 0,1,0,0 },{ 1,1,1,0 },{ 0,0,0,0 } },
{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,1,1,0 },{ 0,0,0,0 } },
{ { 0,0,0,0 },{ 0,1,1,0 },{ 0,0,1,0 },{ 0,0,1,0 } } };

int y = 0;//テトリミノの画面上でのy座標
int x = 4;//テトリミノの画面上でのx座標
int level = 0;
int oneline, twoline, threeline, fourline;//消したラインの数
int gameover = 0;//ゲームオーバー判定

void Initialize();
int CreateBlock();
void ShowGameField();
void ControlBlock();
int  CheckOverlap(int, int);//テトリミノが壁や固定済みテトリミノに接触していないか判別
void MoveBlock(int, int);//落下中テトリミノを一旦消して、任意の座標に移動させる
int TurnBlock();
void DropBlock();//テトリミノを落下させる。移動できない場合テトリミノを固定
void LockBlock();//着地したテトリミノを固定済みテトリミノに加える関数
void CheckLines();//テトリミノが横一列にそろえばそこを消去、上のテトリミノをそこに下ろす
void InitializeNextBlock();
void HoldBlock();
int ShowHoldBlock();

int main()
{
	int time = 0;//時間をカウント
	char insertcoin = 'y';


	while (insertcoin == 'y') {
		Initialize();//初期化

					 //ゲームオーバーになるまでゲーム続行
		while (!gameover) {

			//キー入力があればそれに応じて操作
			if (_kbhit()) {
				ControlBlock();
			}
			//時間がきたらテトリミノを１マス落下させる。時間が来てなければ時間を１だけインクリメント。
			if (level == 0) {
				if (time < 20000) {
					time++;
				}
				else {
					DropBlock();
					time = 0;
				}
			}

			if (level == 1) {
				if (time < 10000) {
					time++;
				}
				else {
					DropBlock();
					time = 0;
				}
			}

			if (level == 2) {
				if (time < 5000) {
					time++;
				}
				else {
					DropBlock();
					time = 0;
				}
			}

			if (level == 3) {
				if (time < 3000) {
					time++;
				}
				else {
					DropBlock();
					time = 0;
				}
			}

		}
		//ゲームオーバーの場合はGAME OVERを表示
		system("cls");//画面をクリアに
		printf("\n\n\n GAME OVER\n\n YOUR SCORE :　　%d\n\n\n\n\n\n\n",
			oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500);


		printf("コンティニューしますか？（コンティニューする場合はyしない場合はn）\n");
		insertcoin = _getch();

		while (insertcoin != 'y' && insertcoin != 'n') {
			printf("yかnを押してください\n");
			insertcoin = _getch();
		}
		if (insertcoin == 'y') {
			gameover = 0;
		}

	}
	return 0;
}

void Initialize()
{
	int i, j;
	oneline = 0;
	twoline = 0;
	threeline = 0;
	fourline = 0;


	//画面と壁を初期設定
	for (i = 0; i <= 20; i++) {
		for (j = 0; j <= 11; j++) {
			if ((j == 0) || (j == 11) || (i == 20)) {
				field[i][j] = stage[i][j] = 9;
			}
			else {
				field[i][j] = stage[i][j] = 0;
			}
		}
	}
	InitializeNextBlock();//次のテトリミノ初期化
	CreateBlock();//最初のテトリミノ
	ShowGameField();//ゲーム直後の画面を描画
}
void InitializeNextBlock()
{
	int i, j;

	srand((unsigned)time(NULL));//about random number
	block_type = (234 * rand() % 7 + (99 * rand() % 7)*rand()) % 7;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			nextblock[i][j] = block_list[block_type][i][j];
		}
	}
}

int CreateBlock()
{
	int i, j;


	//テトリミノの座標を初期位置にリセット
	y = 0;
	x = 4;

	//乱数を発生させ、その乱数を7で割った余り（０～６まで）でテトリミノの種類を決定
	srand((unsigned)time(NULL));


	block_type = rand() % 7;


	//ネクストテトリミノ出現
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			block[i][j] = 0;
			block[i][j] = nextblock[i][j];
		}
	}

	//ネクストテトリミノ生成
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			nextblock[i][j] = block_list[block_type][i][j];
		}
	}

	//壁＋テトリミノをフィールドへ
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			field[i][j + 4] = stage[i][j + 4] + block[i][j];

			//初期位置に置いたテトリミノが既に固定テトリミノに重なっていればゲームオーバー
			if (field[i][j + 4] > 1) {
				gameover = 1;
				return 1;
			}

		}
	}
	return 0;
}

int ShowHoldBlock()//すでにホールドの所にブロックがある場合に使用
{
	int i, j;
	int block_type;//テトリミノの種類用

				   //まずテトリミノの座標を初期位置にリセット
	y = 0;
	x = 4;


	//ネクストテトリミノ
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			block[i][j] = 0;
			block[i][j] = holdblock[i][j];
			holdblock[i][j] = tempblock[i][j];
		}
	}

	//壁＋テトリミノをフィールドへ
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			field[i][j + 4] = stage[i][j + 4] + block[i][j];

			//初期位置に置いたテトリミノが既に固定テトリミノに重なっていればゲームオーバー
			if (field[i][j + 4] > 1) {
				gameover = 1;
				return 1;
			}

		}
	}
	return 0;
}

//画面表示
void ShowGameField()
{
	int i, j, k;

	//画面を一旦クリア
	system("cls");

	//データに応じてテトリミノや空白を画面表示
	for (i = 0; i<21; i++) {
		for (j = 0; j < 12; j++) {

			switch (field[i][j]) {
			case 0:
				printf("　");
				break;
			case 9:
				printf("□");
				break;
			default:
				printf("■");
				break;
			}
		}
		if (i == 1) {
			printf("　　　□□NEXT□□");//jの指定なくても勝手に上で作った枠の横に配置される
		}

		int k;
		for(s=2;s<6;s++){
			if (i == s) {
				printf("　　　□");
				for (k = 0; k<4; k++) {
					switch (nextblock[s-2][k]) {
					case 0:
						printf("　");
						break;
					default:
						printf("■");
						break;
					}
				}
				printf("□");
			}
		}
		
		if (i == 6) {
			printf("　　　□□□□□□");
		}



		if (i == 8) {
			printf("　　　□□HOLD□□");
		}
		if (i == 9) {
			printf("　　　□");
			for (k = 0; k<4; k++) {
				switch (holdblock[0][k]) {
				case 0:
					printf("　");
					break;
				default:
					printf("■");
					break;
				}
			}
			printf("□");
		}
		if (i == 10) {
			printf("　　　□");
			for (k = 0; k<4; k++) {
				switch (holdblock[1][k]) {
				case 0:
					printf("　");
					break;
				default:
					printf("■");
					break;
				}
			}
			printf("□");
		}
		if (i == 11) {
			printf("　　　□");
			for (k = 0; k<4; k++) {
				switch (holdblock[2][k]) {
				case 0:
					printf("　");
					break;
				default:
					printf("■");
					break;
				}
			}
			printf("□");
		}
		if (i == 12) {
			printf("　　　□");
			for (k = 0; k<4; k++) {
				switch (holdblock[3][k]) {
				case 0:
					printf("　");
					break;
				default:
					printf("■");
					break;
				}
			}
			printf("□");
		}
		if (i == 13) {
			printf("　　　□□□□□□");
		}



		if (i == 15) {
			printf("　　　□□□□□□");
		}
		if (i == 16) {
			printf("　　　　 SCORE");
		}
		if (i == 17) {
			printf("　　　　　%d", oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500);
		}
		if (i == 18) {
			printf("　　　□□□□□□");
		}
		printf("\n");
	}

	//得点表示
	printf("\n１行消し：%d回　２行消し：%d回　３行消し：%d回　４行消し：%d回\n", oneline, twoline, threeline, fourline);

	if (oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500>100 && oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500<1000)
		level = 1;

	if (oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500>1000 && oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500<2000)
		level = 2;

	if (oneline * 100 + twoline * 300 + threeline * 700 + fourline * 1500>2000)
		level = 3;

}

//キー入力に応じてテトリミノを処理
void ControlBlock()
{
	char key;//受け付けたキーを保存する変数

	key = _getch();//キーから一文字入力

				  //キーに応じて各方向へブロックを移動したり、回転させたりする
	switch (key) {

	case 'f':
		if (!CheckOverlap(x + 1, y)) {
			MoveBlock(x + 1, y);
		}
		break;
	case 's':
		if (!CheckOverlap(x - 1, y)) {
			MoveBlock(x - 1, y);
		}
		break;
	case 'd':
		if (!CheckOverlap(x, y + 1)) {
			MoveBlock(x, y + 1);
		}
		break;
	case ' ':
		TurnBlock();
		break;
	case 'e':
		HoldBlock();

	}
}

//重なり検査
int CheckOverlap(int x2, int y2)
{
	int i, j;

	//テトリミノが向かう位置に、固定テトリミノもしくは壁があるかどうかを検査
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			if (block[i][j]) {
				if (stage[y2 + i][x2 + j] != 0) {
					return 1;
				}
			}
		}
	}
	return 0;
}

//移動
void MoveBlock(int x2, int y2)
{
	int i, j;

	//消して
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			field[y + i][x + j] -= block[i][j];
		}
	}
	//テトリミノの座標を更新
	x = x2;
	y = y2;

	//表示
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			field[y + i][x + j] += block[i][j];
		}
	}

	ShowGameField();
}

void HoldBlock() {
	int i, j, cnt = 0;


	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {

			if (holdblock[i][j] == 0)
				cnt++;

			tempblock[i][j] = 0;
			tempblock[i][j] = block[i][j];

			if (block[i][j] == 1)
				field[y + i][x + j] = 0;
		}
	}




	if (cnt == 16) {

		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				holdblock[i][j] = tempblock[i][j];
			}
		}

		CreateBlock();

	}
	else {
		ShowHoldBlock();

	}
	ShowGameField();
}

//テトリミノ回転
int TurnBlock()
{
	int i, j;
	int temp[4][4] = { 0 };//テトリミノを一時保存


	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			temp[i][j] = block[i][j];
		}
	}

	//テトリミノ回転
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			block[i][j] = temp[3 - j][i];
		}
	}
	//重なるテトリミノが出てしまったら回転前に戻して中止
	if (CheckOverlap(x, y)) {
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				block[i][j] = temp[i][j];
			}
		}
		return 1;
	}

	//一旦フィールドからテトリミノ消して回転後のブロックを再表示
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			field[y + i][x + j] -= temp[i][j];
			field[y + i][x + j] += block[i][j];
		}
	}

	ShowGameField();

	return 0;
}

//テトリミノ落とす
void DropBlock()
{
	//重なりがなければ移動
	if (!CheckOverlap(x, y + 1)) {
		MoveBlock(x, y + 1);
	}
	//重なりがあれば壁にする
	else {
		LockBlock();
		CreateBlock();
		ShowGameField();
	}
}

//テトリミノを固定し、横一列がそろってるかの判定を呼び出す
void LockBlock()
{
	int i, j;

	//テトリミノを壁に加える
	for (i = 0; i<21; i++) {
		for (j = 0; j<12; j++) {
			stage[i][j] = field[i][j];
		}
	}

	CheckLines();//横一列がそろってるか判定

				 //列完成判定後の壁をフィールドへ
	for (i = 0; i<21; i++) {
		for (j = 0; j<12; j++) {
			field[i][j] = stage[i][j];
		}
	}
}

//横一列が完成しているか検査。
void CheckLines()
{
	int i, j, k;
	int comp;//横一列がそろっていれば１、一つでも隙間があると０になる
	int lines = 0;//消したラインの数

	while (1) {
		for (i = 0; i<20; i++) {
			comp = 1;

			for (j = 1; j<11; j++) {
				if (stage[i][j] == 0) {
					comp = 0;
				}
			}
			if (comp == 1) break;
		}
		if (comp == 0) break;

		lines++;
		// 列を消去
		for (j = 1; j<11; j++) {
			stage[i][j] = 0;
		}
		//消えた列より上にあった固定テトリミノを列の消えたところへ下ろす
		for (k = i; k>0; k--) {
			for (j = 1; j<11; j++) {
				stage[k][j] = stage[k - 1][j];
			}
		}
	}

	switch (lines) {
	case 1:
		oneline++;
		break;
	case 2:
		twoline++;
		break;
	case 3:
		threeline++;
		break;
	case 4:
		fourline++;
		break;
	default:
		break;
	}

}