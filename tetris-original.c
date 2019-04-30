#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
extern int errno;
struct termios otty,ntty;
int kbhit(void);
int getch(void);
int tinit(void);

#define clearScreen() printf("\033[2J")
#define setPosition(x,y) printf("\033[%d;%dH", (y)+1, (x)*2+1)
#define cursolOn() printf("\033[?25h")
#define cursolOff() printf("\033[?25l")
#define WIDTH 10
#define HEIGHT 24

#define setCharColor(n) printf("\033[3%dm", (n))
#define setBackColor(n) printf("\033[4%dm", (n))
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define DEFAULT 9

#define setAttribute(n) printf("\033[%dm", (n))
#define NORMAL 0
#define BLIGHT 1
#define DIM 2
#define UNDERBAR 4
#define BLINK 5
#define REVERSE 7
#define HIDE 8
#define STRIKE 9
#define BLOCK_SIZE 4
#define BLOCK_NUM 7

typedef struct cell{
	char c;
	int charcolor;
	int backcolor;
	int attribute;
} Cell;

int wait(int msec);
void initialize(void);
void reset(void);
int checkRange(Cell a, int x, int y);
int printCell(Cell c, int x, int y);
int clearCell(Cell c, int x, int y);
int printBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y);
int clearBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y);
void copyBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE], Cell dst[BLOCK_SIZE][BLOCK_SIZE]);
void rotateBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE], Cell dst[BLOCK_SIZE][BLOCK_SIZE]);
int checkCell(Cell a, int x, int y);
int checkMap(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y);
void putMap(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y);
void printMap(void);
int checkLine(int y);
void deleteLine(int ys);
int deleteMap(void);
void printNext(int type);
void printScore(int score);

Cell block_type[BLOCK_NUM][BLOCK_SIZE][BLOCK_SIZE] = {
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	' ', RED, BLACK, REVERSE,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	'\0', RED, BLACK, NORMAL,
	
	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	' ', BLUE, BLACK, REVERSE,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,
	'\0', BLUE, BLACK, NORMAL,

	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	' ', GREEN, BLACK, REVERSE,
	' ', GREEN, BLACK, REVERSE,
	'\0', GREEN, BLACK, NORMAL,
	' ', GREEN, BLACK, REVERSE,
	' ', GREEN, BLACK, REVERSE,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	'\0', GREEN, BLACK, NORMAL,
	
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	' ', YELLOW, BLACK, REVERSE,
	' ', YELLOW, BLACK, REVERSE,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	' ', YELLOW, BLACK, REVERSE,
	' ', YELLOW, BLACK, REVERSE,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	'\0', YELLOW, BLACK, NORMAL,
	
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	' ', CYAN, BLACK, REVERSE,
	' ', CYAN, BLACK, REVERSE,
	' ', CYAN, BLACK, REVERSE,
	' ', CYAN, BLACK, REVERSE,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	'\0', CYAN, BLACK, NORMAL,
	
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	' ', MAGENTA, BLACK, REVERSE,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	' ', MAGENTA, BLACK, REVERSE,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	'\0', MAGENTA, BLACK, NORMAL,
	
	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	' ', WHITE, BLACK, REVERSE,
	' ', WHITE, BLACK, REVERSE,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
	'\0', WHITE, BLACK, NORMAL,
};
Cell map[HEIGHT][WIDTH];

int main(int argc, char *argv[])
{
	int x, y, c, prex, prey, t, next, score;
	Cell block[BLOCK_SIZE][BLOCK_SIZE], block_tmp[BLOCK_SIZE][BLOCK_SIZE];
	struct timeval start_time, now_time, pre_time;
	double duration, thold;

	score=0;
	t=rand()%BLOCK_NUM;
	next=rand()%BLOCK_NUM;
	copyBlock(block_type[t], block);
	initialize();
	printNext(next);
	printScore(score);
	x=5;
	y=BLOCK_SIZE;
	thold=0.5;
	printBlock(block, x, y);
	gettimeofday(&start_time, NULL);
	pre_time = start_time;
	for(; ;){
		prex = x;
		prey = y;

		if(kbhit()){
			c = getch();
			if(c==0x1b){
				c=getch();
				if(c==0x5b){
					c=getch();
					switch(c){
						case 0x41:
							rotateBlock(block, block_tmp);
							clearBlock(block, x, y);
							printBlock(block_tmp, x, y);
							copyBlock(block_tmp, block);
							break;
						case 0x42:
							while(checkMap(block, x, y+1)==0)
								y++;
							score+=y-prey;
							printScore(score);
							break;
						case 0x43:
							if(checkMap(block, x+1, y)==0)
								x++;
							break;
						case 0x44:
							if(checkMap(block, x-1, y)==0)
								x--;
							break;
					}
				}
			}
			else{
				reset();
				exit(1);
			}
		}
		gettimeofday(&now_time, NULL);
		duration = now_time.tv_sec - pre_time.tv_sec + (now_time.tv_sec - pre_time.tv_sec) / 1000000.0;
		if(duration > thold){
			pre_time = now_time;
			thold -= 0.001;
			if(checkMap(block, x, y+1)==0)
				y++;
			else{
				int line;
				if(y==BLOCK_SIZE){
					reset();
					exit(1);
				}
				putMap(block, x, y);
				line = deleteMap();
				switch(line){
					case 1: score+=40; break;
					case 2: score+=100; break;
					case 3: score+=300; break;
					case 4: score+=1200; break;
				}
				printScore(score);
				x=5;
				y=BLOCK_SIZE;
				prex=5;
				prey=BLOCK_SIZE;
				t=next;
				next=rand()%BLOCK_NUM;
				printNext(next);
				copyBlock(block_type[t], block);
				printBlock(block, x, y);
			}
		}
		if(prex!=x || prey!=y){
			clearBlock(block, prex, prey);
			printBlock(block, x, y);
		}
	}
	reset();
}

int wait(int msec){
	struct timespec r = {0, msec * 1000L * 1000L};
	return nanosleep(&r, NULL);
}

void initialize(void){
	int x, y;
	Cell a = {'\0', BLACK, BLACK, NORMAL};
	tinit();
	setAttribute(NORMAL);
	setBackColor(BLACK);
	setCharColor(WHITE);
	cursolOff();
	clearScreen();
	fflush(stdout);
	for(y=0;y<HEIGHT;y++)
		for(x=0;x<WIDTH;x++)
			map[y][x] = a;
}

void reset(void){
	setBackColor(BLACK);
	setCharColor(WHITE);
	setAttribute(NORMAL);
	clearScreen();
	cursolOn();
	tcsetattr(1, TCSADRAIN, &otty);
	write(1, "\n", 1);
}

int checkRange(Cell a, int x, int y){
	if(a.c=='\0' || x<0 || y<0 || x>=WIDTH || y>=HEIGHT)
		return -1;
	else
		return 0;
}

int printCell(Cell c, int x, int y){
	if(checkRange(c, x, y)==-1)
		return -1;
	setPosition(x, y);
	setAttribute(c.attribute);
	setBackColor(c.backcolor);
	setCharColor(c.charcolor);
	printf("%c%c", c.c, c.c);
	fflush(stdout);
	return 0;
}

int clearCell(Cell c, int x, int y){
	if(checkRange(c, x, y)==-1)
		return -1;
	setPosition(x, y);
	setAttribute(NORMAL);
	setBackColor(BLACK);
	setCharColor(BLACK);
	printf(" ");
	fflush(stdout);
	return 0;
}

void copyBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE], Cell dst[BLOCK_SIZE][BLOCK_SIZE]){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++)
			dst[j][i]=src[j][i]; 
}

int printBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++)
			printCell(block[j][i], i+x, j+y);
	return 0;
}

int clearBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++)
			clearCell(block[j][i], i+x, j+y);
	return 0;
}

int kbhit(void){
	int ret;
	fd_set rfd;
	struct timeval timeout = {0, 0};
	FD_ZERO(&rfd);
	FD_SET(0, &rfd);
	ret = select(1, &rfd, NULL, NULL, &timeout);
	if(ret == 1)
		return 1;
	else
		return 0;
}

int getch(void){
	unsigned char c;
	int n;
	while((n = read(0, &c, 1)) < 0 && errno == EINTR);
	if(n==0)
		return -1;
	else
		return (int)c;
}

static void onsignal(int sig){
	signal(sig, SIG_IGN);
	switch(sig){
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
		case SIGHUP:
		 exit(1);
		 break;
	}
}

int tinit(void){
	if(tcgetattr(1, &otty) < 0)
		return -1;
	ntty = otty;
	ntty.c_iflag &= ~(INLCR|ICRNL|IXON|IXOFF|ISTRIP);
	ntty.c_oflag &= ~OPOST;
	ntty.c_lflag &= ~(ICANON|ECHO);
	ntty.c_cc[VMIN] = 1;
	ntty.c_cc[VTIME] = 0;
	tcsetattr(1, TCSADRAIN, &ntty);
	signal(SIGINT, onsignal);
	signal(SIGQUIT, onsignal);
	signal(SIGTERM, onsignal);
	signal(SIGHUP, onsignal);
}

void rotateBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE], Cell dst[BLOCK_SIZE][BLOCK_SIZE]){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++)
			dst[i][BLOCK_SIZE-1 - j]=src[j][i];
}

int checkCell(Cell a, int x, int y){
	if(checkRange(a, x, y) || map[y][x].c != '\0')
		return -1;
	return 0;
}

int checkMap(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++){
			if(block[j][i].c != '\0'){
				if(checkCell(block[j][i], x+i, y+j))
					return -1;
			}
		}
	return 0;
}

void putMap(Cell block[BLOCK_SIZE][BLOCK_SIZE], int x, int y){
	int i, j;
	for(j=0; j<BLOCK_SIZE; j++)
		for(i=0; i<BLOCK_SIZE; i++){
			if(checkCell(block[j][i], x+i, y+j)==0)
				map[y+j][x+i] = block[j][i];
		}
}

void printMap(void){
	int x, y;
	for(y=0; y<HEIGHT; y++)
		for(x=0; x<WIDTH; x++)
			printCell(map[y][x], x, y);
}

int checkLine(int y){
	int x;
	for(x=0; x<WIDTH; x++)
		if(map[y][x].c=='\0')
			return -1;
	return 0;
}

void deleteLine(int ys){
	int x, y;
	for(y=ys; y>0; y--)
		for(x=0; x<WIDTH; x++)
			map[y][x]=map[y-1][x];
	setBackColor(BLACK);
	clearScreen();
	printMap();
}

int deleteMap(void){
	int y, count;
	count=0;
	for(y=0;y<HEIGHT;y++)
		if(checkLine(y)==0){
			deleteLine(y);
			count++;
		}
	return count;
}

void printNext(int type){
	int i, j;
	Cell a = {' ', BLACK, BLACK, NORMAL};
	setPosition(0, 0);
	setAttribute(NORMAL);
	setBackColor(WHITE);
	setCharColor(BLACK);
	printf("NEXT");
	for(j=0;j<BLOCK_SIZE;j++)
		for(i=0;i<BLOCK_SIZE;i++)
			printCell(a, 5+i, 0+j);
	printBlock(block_type[type], 5, 0);
}

void printScore(int score){
	setPosition(WIDTH, 10);
	setAttribute(NORMAL);
	setBackColor(WHITE);
	setCharColor(BLACK);
	printf("Score' %d", score);
}
