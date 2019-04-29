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
#define setPosition(x,y) printf("\033[%d;%dH", (y), (x))
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