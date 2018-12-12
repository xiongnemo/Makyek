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
char visualboard[BOARD_SIZE][BOARD_SIZE] = {0};
int blackvalueboard[BOARD_SIZE][BOARD_SIZE] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,2,2,2,2,2,2,0,0,0},
    {0,0,3,3,3,3,3,3,3,2,0,0},
    {0,2,2,3,4,4,30,25,20,25,30,0},
    {1,1,2,3,4,5,25,300,200,300,25,0},
    {1,1,2,3,4,5,20,200,400,200,20,0},
    {0,2,2,3,4,4,25,300,200,300,25,0},
    {0,0,3,3,3,3,30,25,20,25,30,0},
    {0,0,0,2,2,2,2,2,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};
int whitevalueboard[BOARD_SIZE][BOARD_SIZE] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,2,2,2,2,2,2,0,0,0},
    {0,30,25,20,25,30,3,3,3,2,0,0},
    {0,25,300,200,300,25,4,4,3,2,1,0},
    {1,20,200,400,200,20,5,4,3,2,1,0},
    {1,25,300,200,300,25,4,3,2,1,0},
    {0,30,25,20,25,30,4,4,3,2,0,0},
    {0,0,3,3,3,3,3,3,3,0,0,0},
    {0,0,0,2,2,2,2,2,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};
char valueboard[BOARD_SIZE][BOARD_SIZE] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,2,2,2,2,2,2,0,0,0},
    {0,0,3,3,3,3,3,3,3,2,0,0},
    {0,2,2,3,4,4,4,4,3,2,1,0},
    {1,1,2,3,4,5,5,4,3,2,1,0},
    {1,1,2,3,4,5,5,4,3,2,1,0},
    {0,2,2,3,4,4,4,4,3,2,0,0},
    {0,0,3,3,3,3,3,3,3,0,0,0},
    {0,0,0,2,2,2,2,2,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};
int me_flag;
int other_flag;

int search_depth=4;//搜索层数

int moves_in_match=0;
int when_bigger_than_others_by_one=0;


int kk[8]={4,5,6,7,0,1,2,3};

struct Command command = {0, 0, 0};

const int DIR[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };//上下 左右 斜右上下 斜左上下
int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} }; //

void debug(const char *str);
void printBoard(void);
BOOL isInBound(int x, int y);
BOOL isMine(int x, int y);
float AlphaBeta(int nPlay,int nAlpha,int nBeta,char thisvisualboard[BOARD_SIZE][BOARD_SIZE],int this_flag);//AlphaBeta剪枝
float search_value(char thisviusalboard[BOARD_SIZE][BOARD_SIZE]);//搜索估值
BOOL isWhose_search(int x, int y,char thisvisualboard[BOARD_SIZE][BOARD_SIZE],int whose_flag);//虚拟棋盘里的棋子到底是谁的？
void search_place(int new_x,int new_y,int this_flag,char thisvisualboard[BOARD_SIZE][BOARD_SIZE]);//在虚拟棋盘落子并结算
void initAI(int me);
struct Command findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag);
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me);
BOOL place(int x, int y, OPTION option, int cur_flag);
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
            sscanf(buffer, "%s %d", tmp, &me_flag);
            other_flag = 3 - me_flag;
            /*if (me_flag==2)
            can_i_repeat=1;
            if (me_flag==1)
            can_i_repeat=999;
            */
            start(me_flag);
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
            place(x, y, option, other_flag);
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
  struct Command command = aiTurn((const char (*)[BOARD_SIZE])board, me_flag);
  place(command.x, command.y, command.option, me_flag);
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
    char visual_board[BOARD_SIZE][BOARD_SIZE] = {0};
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                visual_board[i][j] = '.';
            }
            else if (board[i][j] == BLACK)
            {
                visual_board[i][j] = 'O';
            }
            else if (board[i][j] == WHITE)
            {
                visual_board[i][j] = 'X';
            }
        }
        printf("%s\n", visual_board[i]);
    }
}

BOOL isInBound(int x, int y)
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
BOOL isMine(int x, int y)
{
    if (board[x][y]==me_flag)
        return 1;
    if (board[x][y]!=me_flag)
        return 0;
}
BOOL isWhose_search(int x, int y,char thisvisualboard[BOARD_SIZE][BOARD_SIZE],int whose_flag)
{
    if (thisvisualboard[x][y]==whose_flag)
        return 1;
    if (thisvisualboard[x][y]!=whose_flag)
        return 0;
}
BOOL place(int x, int y, OPTION option, int cur_flag)
{
    // 移动之前的位置没有我方棋子
    if (board[x][y] != cur_flag)
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
    board[new_x][new_y] = cur_flag;
    int cur_other_flag = 3 - cur_flag;//在函数中提供当前对面的棋子颜色
    // 挑
    int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (int i = 0; i < 4; i++)//对于四个方向进行挑的判断
    {
        int x1 = new_x + intervention_dir[i][0];
        int y1 = new_y + intervention_dir[i][1];
        int x2 = new_x - intervention_dir[i][0];
        int y2 = new_y - intervention_dir[i][1];
        if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == cur_other_flag && board[x2][y2] == cur_other_flag)
        {
            board[x1][y1] = cur_flag;
            board[x2][y2] = cur_flag;
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
        if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == cur_flag && board[x1][y1] == cur_other_flag)
        {
            board[x1][y1] = cur_flag;
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
void search_place(int new_x,int new_y,int this_flag,char thisvisualboard[BOARD_SIZE][BOARD_SIZE])
{
    int another_flag=3-this_flag;
    // 挑
    int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (int i = 0; i < 4; i++)
    {
        int x1 = new_x + intervention_dir[i][0];
        int y1 = new_y + intervention_dir[i][1];
        int x2 = new_x - intervention_dir[i][0];
        int y2 = new_y - intervention_dir[i][1];
        if (isInBound(x1, y1) && isInBound(x2, y2) && thisvisualboard[x1][y1] == another_flag && thisvisualboard[x2][y2] == another_flag)
        {
            thisvisualboard[x1][y1] = this_flag;
            thisvisualboard[x2][y2] = this_flag;
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
        if (isInBound(x1, y1) && isInBound(x2, y2) && thisvisualboard[x2][y2] == this_flag && thisvisualboard[x1][y1] == another_flag)
        {
            thisvisualboard[x1][y1] =this_flag;
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
    int option_rand=2;//rand()%3;
    int s=0;
    int sother=0;
    for (int i=0;i<BOARD_SIZE;i++)
    {
        for (int j=0;j<BOARD_SIZE;j++)
        {
            if (board[i][j]==me_flag)
                s++;
            if (board[i][j]==other_flag)
                sother++;
        }
    }
    if (s>sother)
        when_bigger_than_others_by_one=1;
    if (s<=sother)
        when_bigger_than_others_by_one=0;
	char currentboard[BOARD_SIZE][BOARD_SIZE];
	if (moves_in_match==0&&me_flag==1)
    {
        if (option_rand==2)
        {
            command.x=9;
            command.y=9;
            command.option=4;
        }
        if (option_rand==1)
        {
            command.x=5;
            command.y=3;
            command.option=5;
        }
        if (option_rand==0)
        {
            command.x=9;
            command.y=7;
            command.option=4;
        }
    }
    if (moves_in_match==0&&me_flag==2)
    {
        command.x=6;
        command.y=8;
        command.option=6;
    }

    if (moves_in_match!=0)
    {
        for (int i=0;i<BOARD_SIZE;i++)
        {
            for (int j=0;j<BOARD_SIZE;j++)
            {
                currentboard[i][j]=board[i][j];
            }
        }
        AlphaBeta(search_depth,-999999,9999999,currentboard,me_flag);
    }
    moves_in_match++;
    return command;
}
float search_value(char thisviusalboard[BOARD_SIZE][BOARD_SIZE])
{
    srand((unsigned)time(NULL));
    int i;
    int j;
    int k;//挑夹位判断变量1
    int dire;//走子方向
    int s=0;
    int valueb=0;
    int smak=0;
    int syek=0;
    //int sother=0;
    float form=0;//阵型相关
    int sumx=0;
    int sumy=0;
    int mydangerdisks=0;//我的危险棋子
    int otherdangerdisks=0;//对方的危险棋子
    //int sumotherx=0;
    //int sumothery=0;
    int x[20];
    int y[20];
    //int xother[20];
    //int yother[20];
    float averx=0;
    float avery=0;
    //float averotherx=0;
    //float averothery=0;
    float ex=0;
    float ey=0;
    //float eotherx=0;
    //float eothery=0;
    float e;
    //float eother;
    float efinal;
    for (i=0;i<BOARD_SIZE;i++)
    {
        for (j=0;j<BOARD_SIZE;j++)
        {
            if (isWhose_search(i,j,thisviusalboard,me_flag)==1)
            {
                s++;
                if (when_bigger_than_others_by_one==0)
                    valueb=valueb+valueboard[i][j];
                if (me_flag==1&&when_bigger_than_others_by_one==1)
                    valueb=valueb+blackvalueboard[i][j];
                if (me_flag==2&&when_bigger_than_others_by_one==1)
                    valueb=valueb+whitevalueboard[i][j];
                if(isWhose_search(i+1,j+3,thisviusalboard,me_flag)==1)//3*2对角线
                    form=form+0.6;
                if(isWhose_search(i-1,j+3,thisviusalboard,me_flag)==1)
                    form=form+0.6;
                if(isWhose_search(i+3,j-1,thisviusalboard,me_flag)==1)
                    form=form+0.6;
                if(isWhose_search(i-3,j-1,thisviusalboard,me_flag)==1)
                    form=form+0.6;
                // 挑
                for (dire=0;dire<8;dire++)
                {
                    if (isWhose_search(i+DIR[dire][0],j+DIR[dire][1],thisviusalboard,0)==1)
                    {
                        if (isWhose_search(i-DIR[dire][0],j-DIR[dire][1],thisviusalboard,other_flag)==1)
                        {
                            mydangerdisks++;
                            break;
                        }
                        if (isWhose_search(i+2*DIR[dire][0],j+2*DIR[dire][1],thisviusalboard,me_flag)==1)
                        {
                            mydangerdisks++;
                            break;
                        }

                        /*
                        for (k = 0; k < 4; k++)//对于四个方向进行挑的判断
                        {
                            int x1 = i+DIR[dire][0] + intervention_dir[k][0];
                            int y1 = j+DIR[dire][1] + intervention_dir[k][1];
                            int x2 = i+DIR[dire][0] - intervention_dir[k][0];
                            int y2 = j+DIR[dire][1] - intervention_dir[k][1];
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == other_flag && board[x2][y2] == other_flag)
                            {
                                smak=smak+2.2;
                            }
                        }
                        */
                        /*
                        for (k = 0; k < 8; k++)//对于八个方向进行夹的判断
                        {
                            int x1 = i+DIR[dire][0] + custodian_dir[k][0];
                            int y1 = j+DIR[dire][1] + custodian_dir[k][1];
                            int x2 = i+DIR[dire][0] + custodian_dir[k][0] * 2;
                            int y2 = j+DIR[dire][1] + custodian_dir[k][1] * 2;
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == me_flag && board[x1][y1] == other_flag)
                            {
                                syek=syek+1.5;
                            }
                        }*/

                    }
                }
                x[s-1]=i;//收集方差数据，坐标x
                y[s-1]=j;//收集方差数据，坐标y
                sumx=sumx+i;
                sumy=sumy+j;
            }
            if (isWhose_search(i,j,thisviusalboard,other_flag)==1)
            {
                for (dire=0;dire<8;dire++)
                {
                    if (isWhose_search(i+DIR[dire][0],j+DIR[dire][1],thisviusalboard,0)==1)
                    {
                        if (isWhose_search(i-DIR[dire][0],j-DIR[dire][1],thisviusalboard,me_flag)==1)
                        {
                            otherdangerdisks++;
                            break;
                        }
                        if (isWhose_search(i+2*DIR[dire][0],j+2*DIR[dire][1],thisviusalboard,other_flag)==1)
                        {
                            otherdangerdisks++;
                            break;
                        }
                        /*
                        for (k = 0; k < 4; k++)//对于四个方向进行挑的判断
                        {
                            int x1 = i+DIR[dire][0] + intervention_dir[k][0];
                            int y1 = j+DIR[dire][1] + intervention_dir[k][1];
                            int x2 = i+DIR[dire][0] - intervention_dir[k][0];
                            int y2 = j+DIR[dire][1] - intervention_dir[k][1];
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == me_flag && board[x2][y2] == me_flag)
                            {
                                smak=smak-2.6;
                            }
                        }

                        for (k = 0; k < 8; k++)//对于八个方向进行夹的判断
                        {
                            int x1 = i+DIR[dire][0] + custodian_dir[k][0];
                            int y1 = j+DIR[dire][1] + custodian_dir[k][1];
                            int x2 = i+DIR[dire][0] + custodian_dir[k][0] * 2;
                            int y2 = j+DIR[dire][1] + custodian_dir[k][1] * 2;
                            if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == other_flag && board[x1][y1] == me_flag)
                            {
                                syek=syek-1.8;
                            }
                        }
                        */

                    }
                }
            //    sother++;
            //    xother[sother-1]=i;//收集方差数据，坐标x
            //    yother[sother-1]=j;//收集方差数据，坐标y
            //    sumotherx=sumotherx+i;
            //   sumothery=sumothery+j;
            }
        }
    }
    if (s==0)
    {
        return -999999;
    }
    if (1)//当自己不很占据优势就聚集（全局）
    {
        /*
        if (me_flag==1)
        {
            averx=7;
            avery=11;
        }
        if (me_flag==2)
        {
            averx=4;
            avery=0;
        }
        */

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
    //printf("%d,%d,%d,%f,%f\n",100*s,20*otherdangerdisks,-17*mydangerdisks,15*form,-6*efinal);
    return 100*s+28*smak+28*syek+19*otherdangerdisks-17*mydangerdisks+15*form-6*efinal+1*valueb;
}
float AlphaBeta(int nPlay,int nAlpha,int nBeta,char thisvisualboard[BOARD_SIZE][BOARD_SIZE],int this_flag)
{
	char currentboard[BOARD_SIZE][BOARD_SIZE];
	int score;
	int i;
	int j;
	int k;
	int x;
	int y;
	int ii;
	int jj;
	if(nPlay==0)
		return search_value(thisvisualboard);  //叶子节点返回估值
	for (i=0;i<BOARD_SIZE;i++)
    {
        for (j=0;j<BOARD_SIZE;j++)
        {
            currentboard[i][j]=thisvisualboard[i][j];
        }
    }
	for (k=0;k<=7;k++)
    {
		for (i=0;i<BOARD_SIZE;i++)
        {
            for (j=0;j<BOARD_SIZE;j++)
            {
                if (isWhose_search(i,j,currentboard,this_flag)==1)
                {
                        x=i+DIR[kk[k]][0];
                        y=j+DIR[kk[k]][1];
                        if (isInBound(x,y)&&currentboard[x][y]==0)
                        {
							currentboard[x][y]=this_flag;
							currentboard[i][j]=EMPTY;
                            search_place(x,y,this_flag,currentboard);//生成新节点
                            score=-AlphaBeta(nPlay-1,-nBeta,-nAlpha,currentboard,3-this_flag);//递归搜索子节点
							for (ii=0;ii<BOARD_SIZE;ii++)//恢复原节点
                            {
                                for (jj=0;jj<BOARD_SIZE;jj++)
                                {
                                    currentboard[ii][jj]=thisvisualboard[ii][jj];
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
