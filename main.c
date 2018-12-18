#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// board information
#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// option
typedef int OPTION;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_LEFT 6
#define DOWN_RIGHT 7

#define MAX_BYTE 10000

#define START "START"
#define PLACE "PLACE"
#define TURN  "TURN"
#define END   "END"

struct Command
{
  int x;
  int y;
  OPTION option;
};

char buffer[MAX_BYTE] = {0};
char board[BOARD_SIZE][BOARD_SIZE] = {0};
char valueboard[BOARD_SIZE][BOARD_SIZE] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,5,4,5,4,5,1,1,0,0,0,0},
    {0,4,6,6,6,4,2,2,2,0,0,0},
    {0,5,6,6,6,5,3,3,3,2,0,0},
    {0,4,6,6,6,4,2,2,3,2,1,0},
    {1,5,4,5,4,5,2,2,3,2,1,0},
    {1,1,2,3,2,2,5,4,5,4,5,0},
    {0,2,2,3,2,2,4,6,6,6,4,0},
    {0,0,3,3,3,3,5,6,6,6,5,0},
    {0,0,0,2,2,2,4,6,6,6,4,0},
    {0,0,0,0,1,1,5,4,5,4,5,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};

int meFlag;
int otherFlag;

int searchDepth=4;//搜索层数

int movesInMatch=0;

int kk[8]={4,5,6,7,0,1,2,3};

struct Command command = {0, 0, 0};

const int DIR[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };//上下 左右 斜右上下 斜左上下
int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} }; //

void debug(const char *str);
void printBoard(void);
BOOL isInBound(int x, int y);
BOOL isMine(int x, int y);
int AlphaBeta(int nPlay,int nAlpha,int nBeta,char thisBoard[BOARD_SIZE][BOARD_SIZE],int this_flag);//AlphaBeta剪枝
int searchValue(char thisBoard[BOARD_SIZE][BOARD_SIZE]);//搜索估值
BOOL isWhoseInThisBoard(int x, int y,char thisBoard[BOARD_SIZE][BOARD_SIZE],int whose_flag);//虚拟棋盘里的棋子到底是谁的？
void searchPlace(int new_x,int new_y,int this_flag,char thisBoard[BOARD_SIZE][BOARD_SIZE]);//在虚拟棋盘落子并结算
void initAI(int me);
struct Command findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag);
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me);
BOOL place(int x, int y, OPTION option, int currentFlag);
void start(int flag);
void turn(void);
void end(int x);
void loop(void);
//声明结束
int main(int argc, char *argv[])
{
  loop();
  return 0;
}
void loop(void)
{
//  freopen("../input", "r", stdin);
    while (TRUE)
    {
        memset(buffer, 0, sizeof(buffer));
        gets(buffer);
        if (strstr(buffer, START))
        {
            char tmp[MAX_BYTE] = {0};
            sscanf(buffer, "%s %d", tmp, &meFlag);
            otherFlag = 3 - meFlag;
            /*if (meFlag==2)
            can_i_repeat=1;
            if (meFlag==1)
            can_i_repeat=999;
            */
            start(meFlag);
            printf("OK\n");
            fflush(stdout);
        }
        else if (strstr(buffer, PLACE))
        {
            char tmp[MAX_BYTE] = {0};
            int x, y;
            OPTION option;
            sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &option);
            //other_x=x;
            //other_y=y;
            //other_option=option;
            place(x, y, option, otherFlag);
        }
        else if (strstr(buffer, TURN))
        {
            turn();
        }
        else if (strstr(buffer, END))
        {
            char tmp[MAX_BYTE] = {0};
            int x;
            sscanf(buffer, "%s %d", tmp, &x);
            end(x);
        }
        //printBoard();
    }
}
void end(int x)
{
}
void turn(void)
{
  // AI
  struct Command command = aiTurn((const char (*)[BOARD_SIZE])board, meFlag);
  place(command.x, command.y, command.option, meFlag);
  printf("%d %d %d\n", command.x, command.y, command.option);
  fflush(stdout);
}
void start(int flag)
{
    memset(board, 0, sizeof(board));

    for (int i = 0; i < 3; i++)
    {
        board[2][2 + i] = WHITE;
        board[6][6 + i] = WHITE;
        board[5][3 + i] = BLACK;
        board[9][7 + i] = BLACK;
    }

    for (int i = 0; i < 2; i++)
    {
        board[8 + i][2] = WHITE;
        board[2 + i][9] = BLACK;
    }
    initAI(flag);
}
void debug(const char *str) {
  printf("DEBUG %s\n", str);
  fflush(stdout);
}

void printBoard(void)//打印board[12][12]
{
    char visualBoard[BOARD_SIZE][BOARD_SIZE] = {0};
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                visualBoard[i][j] = '.';
            }
            else if (board[i][j] == BLACK)
            {
                visualBoard[i][j] = 'O';
            }
            else if (board[i][j] == WHITE)
            {
                visualBoard[i][j] = 'X';
            }
        }
        printf("%s\n", visualBoard[i]);
    }
}

BOOL isInBound(int x, int y)
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
BOOL isMine(int x, int y)
{
    if (board[x][y]==meFlag)
        return 1;
    if (board[x][y]!=meFlag)
        return 0;
}
BOOL isWhoseInThisBoard(int x, int y,char thisBoard[BOARD_SIZE][BOARD_SIZE],int whose_flag)
{
    if (thisBoard[x][y]==whose_flag)
        return 1;
    if (thisBoard[x][y]!=whose_flag)
        return 0;
}
BOOL place(int x, int y, OPTION option, int currentFlag)
{
    // 移动之前的位置没有我方棋子
    if (board[x][y] != currentFlag)
    {
        return FALSE;
    }
    int new_x = x + DIR[option][0];
    int new_y = y + DIR[option][1];
    // 移动之后的位置超出边界, 或者不是空地
    if (!isInBound(new_x, new_y) || board[new_x][new_y] != EMPTY)
    {
        return FALSE;
    }
    board[x][y] = EMPTY;
    board[new_x][new_y] = currentFlag;
    int cur_otherFlag = 3 - currentFlag;//在函数中提供当前对面的棋子颜色
    // 挑
    int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (int i = 0; i < 4; i++)//对于四个方向进行挑的判断
    {
        int x1 = new_x + intervention_dir[i][0];
        int y1 = new_y + intervention_dir[i][1];
        int x2 = new_x - intervention_dir[i][0];
        int y2 = new_y - intervention_dir[i][1];
        if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == cur_otherFlag && board[x2][y2] == cur_otherFlag)
        {
            board[x1][y1] = currentFlag;
            board[x2][y2] = currentFlag;
        }
    }
    // 夹
    int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    for (int i = 0; i < 8; i++)//对于八个方向进行夹的判断
    {
        int x1 = new_x + custodian_dir[i][0];
        int y1 = new_y + custodian_dir[i][1];
        int x2 = new_x + custodian_dir[i][0] * 2;
        int y2 = new_y + custodian_dir[i][1] * 2;
        if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == currentFlag && board[x1][y1] == cur_otherFlag)
        {
            board[x1][y1] = currentFlag;
        }
    }

    return TRUE;
}
/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */

/**
 * You can define your own struct and variable here
 * 你可以在这里定义你自己的结构体和变量
 */
void searchPlace(int new_x,int new_y,int this_flag,char thisBoard[BOARD_SIZE][BOARD_SIZE])
{
    int anotherFlag=3-this_flag;
    // 挑
    int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (int i = 0; i < 4; i++)
    {
        int x1 = new_x + intervention_dir[i][0];
        int y1 = new_y + intervention_dir[i][1];
        int x2 = new_x - intervention_dir[i][0];
        int y2 = new_y - intervention_dir[i][1];
        if (isInBound(x1, y1) && isInBound(x2, y2) && thisBoard[x1][y1] == anotherFlag && thisBoard[x2][y2] == anotherFlag)
        {
            thisBoard[x1][y1] = this_flag;
            thisBoard[x2][y2] = this_flag;
        }
    }

    // 夹
    int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    for (int i = 0; i < 8; i++)
    {
        int x1 = new_x + custodian_dir[i][0];
        int y1 = new_y + custodian_dir[i][1];
        int x2 = new_x + custodian_dir[i][0] * 2;
        int y2 = new_y + custodian_dir[i][1] * 2;
        if (isInBound(x1, y1) && isInBound(x2, y2) && thisBoard[x2][y2] == this_flag && thisBoard[x1][y1] == anotherFlag)
        {
            thisBoard[x1][y1] =this_flag;
        }
    }

}
/**
 * 你可以在这里初始化你的AI
 */
void initAI(int me)
{

}

/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * me表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，在x属性和y属性填上你想要移动的棋子的位置，option填上你想要移动的方向。
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
  /*
   * TODO：在这里写下你的AI。
   * 这里有一个示例AI，它只会寻找第一个可下的位置进行落子。
   */

  struct Command preferedPos = findValidPos(board, me);

  return preferedPos;
}
/**
 * 你的代码结束
 */
struct Command findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag)
{
    //srand((unsigned)time(NULL));
    //printf("%d\n",movesInMatch);
    int option_X=0;//rand()%3;
    int option_Y=1;//rand()%3;
	char currentBoard[BOARD_SIZE][BOARD_SIZE];
	if (movesInMatch==0&&meFlag==1)
    {
        if (option_X==3)
        {
            command.x=9;
            command.y=8;
            command.option=0;
        }
        if (option_X==2)
        {
            command.x=9;
            command.y=9;
            command.option=4;
        }
        if (option_X==1)
        {
            command.x=5;
            command.y=3;
            command.option=5;
        }
        if (option_X==0)
        {
            command.x=9;
            command.y=7;
            command.option=4;
        }
    }
    if (movesInMatch==0&&meFlag==2)
    {
        if (option_Y==1)
        {
            command.x=6;
            command.y=7;
            command.option=1;
        }
        if (option_Y==0)
        {
            command.x=6;
            command.y=8;
            command.option=6;
        }
    }
    if (movesInMatch==1&&meFlag==1)
    {
        if (option_X==0)
        {
            command.x=3;
            command.y=9;
            command.option=6;
        }
    }
    /*
    if (movesInMatch==1&&meFlag==2)
    {
        if (option_Y==1)
        {
            command.x=6;
            command.y=8;
            command.option=5;
        }
        if (option_Y==0)
        {
            command.x=2;
            command.y=3;
            command.option=7;
        }
    }
    */
    if (movesInMatch!=0)
    {
        for (int i=0;i<BOARD_SIZE;i++)
        {
            for (int j=0;j<BOARD_SIZE;j++)
            {
                currentBoard[i][j]=board[i][j];
            }
        }
        AlphaBeta(searchDepth,-999999,9999999,currentBoard,meFlag);
    }
    movesInMatch++;
    //printf("%d\n",movesInMatch);
    return command;
}
int searchValue(char thisBoard[BOARD_SIZE][BOARD_SIZE])
{
    srand((unsigned)time(NULL));
    int i;//x坐标
    int j;//y坐标
    int dire;//走子方向
    int s=0;
    int smak=0;
    int syek=0;
    float form=0;//阵型相关
    int myDangerousDisks=0;//我的危险棋子
    int othersDangerousDisks=0;//对方的危险棋子
    int gather=0;
    int finalScore;
    //int ii;
    //int jj;
    //int k;//挑夹位判断变量1
    //int rand_flag=0;//随机挑夹位判定
    //int temp;
    //int randf[3]={999,999,999};
    //int valueb=0;
    //int sother=0;
    //int sumx=0;
    //int sumy=0;
    //int sumotherx=0;
    //int sumothery=0;
    //int x[20];
    //int y[20];
    //int xother[20];
    //int yother[20];
    //float averx=0;
    //float avery=0;
    //float averotherx=0;
    //float averothery=0;
    //float ex=0;
    //float ey=0;
    //float eotherx=0;
    //float eothery=0;
    //float e;
    //float eother;
    //float efinal;
    for (i=0;i<BOARD_SIZE;i++)
    {
        for (j=0;j<BOARD_SIZE;j++)
        {
            if (isWhoseInThisBoard(i,j,thisBoard,meFlag)==1)
            {
                s++;
                if (meFlag==1)
                {
                    gather=gather+(i-6)*(i-6)+(j-7)*(j-7);
                }
                if (meFlag==2)
                {
                    gather=gather+(i-5)*(i-5)+(j-4)*(j-4);
                }
                //valueb=valueb+valueboard[i][j];
                if(isWhoseInThisBoard(i+1,j+3,thisBoard,meFlag)==1)//3*2对角线
                    form=form+6;
                if(isWhoseInThisBoard(i-1,j+3,thisBoard,meFlag)==1)
                    form=form+6;
                if(isWhoseInThisBoard(i+3,j-1,thisBoard,meFlag)==1)
                    form=form+6;
                if(isWhoseInThisBoard(i-3,j-1,thisBoard,meFlag)==1)
                    form=form+6;
                /*
                randf[0]=999;
                randf[1]=999;
                randf[2]=999;
                for (ii=0;ii<2;)
                {
                    rand_flag=0;
                    temp=rand()%8;
                    for (jj=0;jj<ii;jj++)
                    {
                        if (randf[jj]==temp)
                            rand_flag=1;
                    }
                    if (rand_flag==0)
                    {
                        randf[ii]=temp;
                        ii++;
                    }
                }
                for (ii=0;ii<2;ii++)
                {
                    dire=randf[ii];
                    if (isWhoseInThisBoard(i+DIR[dire][0],j+DIR[dire][1],thisBoard,0)==1)
                    {
                        for (k = 0; k < 4; k++)//对于四个方向进行挑的判断
                        {
                            int x1 = i+DIR[dire][0] + intervention_dir[k][0];
                            int y1 = j+DIR[dire][1] + intervention_dir[k][1];
                            int x2 = i+DIR[dire][0] - intervention_dir[k][0];
                            int y2 = j+DIR[dire][1] - intervention_dir[k][1];
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == otherFlag && board[x2][y2] == otherFlag)
                            {
                                smak=smak+2.4;
                            }
                        }
                        for (k = 0; k < 8; k++)//对于八个方向进行夹的判断
                        {
                            int x1 = i+DIR[dire][0] + custodian_dir[k][0];
                            int y1 = j+DIR[dire][1] + custodian_dir[k][1];
                            int x2 = i+DIR[dire][0] + custodian_dir[k][0] * 2;
                            int y2 = j+DIR[dire][1] + custodian_dir[k][1] * 2;
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == meFlag && board[x1][y1] == otherFlag)
                            {
                                syek=syek+1.8;
                            }
                        }
                    }
                }
                //printf("%d\n",dire);
                */
/*
                for (dire=0;dire<8;dire++)
                {

                        if (isWhoseInThisBoard(i-DIR[dire][0],j-DIR[dire][1],thisBoard,otherFlag)==1)
                        {
                            myDangerousDisks++;
                            break;
                        }
                        if (isWhoseInThisBoard(i+2*DIR[dire][0],j+2*DIR[dire][1],thisBoard,meFlag)==1)
                        {
                            myDangerousDisks++;
                            break;
                        }

                }
*/
                /*
                x[s-1]=i;//收集方差数据，坐标x
                y[s-1]=j;//收集方差数据，坐标y
                sumx=sumx+i;
                sumy=sumy+j;
                */
            }
            if (isWhoseInThisBoard(i,j,thisBoard,otherFlag)==1)
            {
                /*
                randf[0]=999;
                randf[1]=999;
                randf[2]=999;
                for (ii=0;ii<2;)
                {
                    rand_flag=0;
                    temp=rand()%8;
                    for (jj=0;jj<ii;jj++)
                    {
                        if (randf[jj]==temp)
                            rand_flag=1;
                    }
                    if (rand_flag==0)
                    {
                        randf[ii]=temp;
                        ii++;
                    }
                }
                for (ii=0;ii<2;ii++)
                {
                    dire=randf[ii];
                    if (isWhoseInThisBoard(i+DIR[dire][0],j+DIR[dire][1],thisBoard,0)==1)
                    {
                        for (k = 0; k < 4; k++)//对于四个方向进行挑的判断
                        {
                            int x1 = i+DIR[dire][0] + intervention_dir[k][0];
                            int y1 = j+DIR[dire][1] + intervention_dir[k][1];
                            int x2 = i+DIR[dire][0] - intervention_dir[k][0];
                            int y2 = j+DIR[dire][1] - intervention_dir[k][1];
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == meFlag && board[x2][y2] == meFlag)
                            {
                                smak=smak-1.5;
                            }
                        }
                        for (k = 0; k < 8; k++)//对于八个方向进行夹的判断
                        {
                            int x1 = i+DIR[dire][0] + custodian_dir[k][0];
                            int y1 = j+DIR[dire][1] + custodian_dir[k][1];
                            int x2 = i+DIR[dire][0] + custodian_dir[k][0] * 2;
                            int y2 = j+DIR[dire][1] + custodian_dir[k][1] * 2;
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == otherFlag && board[x1][y1] == meFlag)
                            {
                                syek=syek-1.3;
                            }
                        }
                    }
                }
                */

                for (dire=0;dire<8;dire++)
                {

                        if (isWhoseInThisBoard(i-DIR[dire][0],j-DIR[dire][1],thisBoard,meFlag)==1)
                        {
                            othersDangerousDisks++;
                            break;
                        }
                        if (isWhoseInThisBoard(i+2*DIR[dire][0],j+2*DIR[dire][1],thisBoard,otherFlag)==1)
                        {
                            othersDangerousDisks++;
                            break;
                        }

                }

            //    sother++;
            //    xother[sother-1]=i;//收集方差数据，坐标x
            //    yother[sother-1]=j;//收集方差数据，坐标y
            //    sumotherx=sumotherx+i;
            //    sumothery=sumothery+j;
            }
        }
    }
    if (s==0)
    {
        return -999999;
    }
    /*
    if (1)//当自己不很占据优势就聚集（全局）
    {

        if (meFlag==1)
        {
            averx=7;
            avery=8;
        }
        if (meFlag==2)
        {
            averx=4;
            avery=3;
        }


        averx=sumx/s;//开始处理自己的方差x，平均x
        avery=sumy/s;//开始处理自己的方差y，平均y

        for (i=0;i<s;i++)
        {
            ex=ex+(x[i]-averx)*(x[i]-averx);
            ey=ey+(y[i]-avery)*(y[i]-avery);
        }
        e=ex/(s-1)+ey/(s-1);//自身方差处理完成
        efinal=e-12;
    }
    */
    /*
    if (s<=sother+2)//当自己的占据优势就聚集到对方（被弃用）
    {
        averx=sumx/s;//开始处理自己的方差x，平均x
        avery=sumy/s;//开始处理自己的方差y，平均y
        for (i=0;i<s;i++)
        {
            ex=ex+(x[i]-averx)*(x[i]-averx);
            ey=ey+(y[i]-avery)*(y[i]-avery);
        }
        e=ex/(s-1)+ey/(s-1);//自身方差处理完成
        averotherx=sumotherx/sother;//开始处理对方的方差x，平均x
        averothery=sumothery/sother;//开始处理对方的方差y，平均y
        for (i=0;i<s;i++)
        {
            eotherx=eotherx+(x[i]-averotherx)*(x[i]-averotherx);
            eothery=eothery+(y[i]-averothery)*(y[i]-averothery);
        }
        eother=eotherx/(s-1)+eothery/(s-1);//对方相对于自己的方差处理完成
        efinal=3/2*e+1/2*eother;//efinal是假设对方的平均位置得出的方差
    }
    */
    finalScore=1500*s+280*smak+280*syek+190*othersDangerousDisks-40*myDangerousDisks+13*form-7*gather;
    //printf("%d\n",finalScore);
    return finalScore;
}
int AlphaBeta(int nPlay,int nAlpha,int nBeta,char thisBoard[BOARD_SIZE][BOARD_SIZE],int this_flag)
{
	//printf("depth %d\n",nPlay);
	char currentBoard[BOARD_SIZE][BOARD_SIZE];
	int score;
	int i;
	int j;
	int k;
	int x;
	int y;
	int ii;
	int jj;
	if(nPlay==0)
		return searchValue(thisBoard);  //叶子节点返回估值
	for (i=0;i<BOARD_SIZE;i++)
    {
        for (j=0;j<BOARD_SIZE;j++)
        {
            currentBoard[i][j]=thisBoard[i][j];
        }
    }
	for (k=0;k<=7;k++)
    {
		for (i=0;i<BOARD_SIZE;i++)
        {
            for (j=0;j<BOARD_SIZE;j++)
            {
                if (isWhoseInThisBoard(i,j,currentBoard,this_flag)==1)
                {
                        x=i+DIR[kk[k]][0];
                        y=j+DIR[kk[k]][1];
                        if (isInBound(x,y)&&currentBoard[x][y]==0)
                        {
							currentBoard[x][y]=this_flag;
							currentBoard[i][j]=EMPTY;
                            searchPlace(x,y,this_flag,currentBoard);//生成新节点
                            score=-AlphaBeta(nPlay-1,-nBeta,-nAlpha,currentBoard,3-this_flag);//递归搜索子节点
							for (ii=0;ii<BOARD_SIZE;ii++)//恢复原节点
                            {
                                for (jj=0;jj<BOARD_SIZE;jj++)
                                {
                                    currentBoard[ii][jj]=thisBoard[ii][jj];
                                }
                            }
                            if (score > nAlpha)
                            {
                                nAlpha=score;
                                if(nPlay==4)
                                {
                                    command.x=i;
                                    command.y=j;
                                    command.option=kk[k];
                                }
                            }
                            if (score >= nBeta)
                            {
                                return nBeta;
                            }
						}
					}
                }
            }
    }
    return nAlpha;//返回最小值
}
