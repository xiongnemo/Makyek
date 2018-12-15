#include <stdio.h>
#include <time.h>
int main(void)
{
    srand((unsigned)time(NULL));
    int randf[3]={999,999,999};
    int ii;
    int jj;
    int temp;
    int rand_flag;
    for (ii=0;ii<3;)
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
    for (ii=0;ii<3;ii++)
        printf("%d ",randf[ii]);
}
