#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 144 /* frame dimension for QCIF format */
#define M 176 /* frame dimension for QCIF format */
#define s 2   /*scaling factor*/


/* code for armulator*/
//#pragma arm section zidata="ram"
int current[N][M];
int temp[N+2][M+2];
double conV[N][M],conH[N][M],con2[N][M];
int final[N*s][M*s];
//#pragma arm section

int GV[3][3],GH[3][3],G2[3][3];
int i,j;
int xc,yc,xf,yf;
double x,y,q,w,a,b;


void read()
{
  FILE *frame_c;
  if((frame_c=fopen("akiyo1.y","rb"))==NULL)
  {
    printf("current frame doesn't exist\n");
    exit(-1);
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current[i][j]=fgetc(frame_c);
    }
  }
}


void bicubic()
{

 for(i=0;i<(N+2);++i)
 {
   for(j=0;j<(M+2);++j)
   {
     temp[i][j]=0;
   }
  }

  for(i=1;i<(N+1);++i)
  {
    for(j=1;j<(M+1);++j)
    {
      temp[i][j]=current[i-1][j-1];
    }
  }

GV[0][0]=GV[1][0]=GV[2][0]=1;
GV[0][1]=GV[1][1]=GV[2][1]=0;
GV[0][2]=GV[1][2]=GV[2][2]=-1;

GH[0][0]=GH[0][1]=GH[0][2]=-1;
GH[1][0]=GH[1][1]=GH[1][2]=0;
GH[2][0]=GH[2][1]=GH[2][2]=1;

G2[0][0]=G2[0][2]=G2[2][0]=G2[2][2]=0;
G2[0][1]=G2[1][0]=G2[1][2]=G2[2][1]=1;
G2[1][1]=-4;


 for(i=1;i<(N+1);++i)
 {
   for(j=1;j<(M+1);++j)
   {
     conV[i-1][j-1]=temp[i-1][j-1]*GV[0][0]+temp[i-1][j]*GV[0][1]+temp[i-1][j+1]*GV[0][2]+temp[i][j-1]*GV[1][0]+temp[i][j]*GV[1][1]+temp[i][j+1]*GV[1][2]+temp[i+1][j-1]*GV[2][0]+temp[i+1][j]*GV[2][1]+temp[i+1][j+1]*GV[2][2];
     conH[i-1][j-1]=temp[i-1][j-1]*GH[0][0]+temp[i-1][j]*GH[0][1]+temp[i-1][j+1]*GH[0][2]+temp[i][j-1]*GH[1][0]+temp[i][j]*GH[1][1]+temp[i][j+1]*GH[1][2]+temp[i+1][j-1]*GH[2][0]+temp[i+1][j]*GH[2][1]+temp[i+1][j+1]*GH[2][2];
     con2[i-1][j-1]=temp[i-1][j]*G2[0][1]+temp[i][j-1]*G2[1][0]+temp[i][j]*G2[1][1]+temp[i][j+1]*G2[1][2]+temp[i+1][j]*G2[2][1];
      }
 }

 
 for(i=0;i<(N*s);++i)
 {
   for(j=0;j<(M*s);++j)
   {
     x=(i+0.5)/s;
     y=(j+0.5)/s;
     xc=ceil(x); 
     yc=ceil(y);
     xf=floor(x);
     yf=floor(y);
     q=x-xf;
     w=y-yf;
     a=abs(x-xc);
     b=abs(y-yc);

   if((q>=0)&(q<=0.5)&(w>=0)&(w<=0.5))
           final[i][j]=(1-a*a*(3-2*a))*(1-b*b*(3-2*b))*current[xc][yc]+
                       a*a*(3-2*a)*(1-b*b*(3-2*b))*current[xc-1][yc]+
                       (1-a*a*(3-2*a))*b*b*(3-2*b)*current[xc][yc-1]+
                       a*a*(3-2*a)*b*b*(3-2*b)*current[xc-1][yc-1]+
                       a*(a-1)*(a-1)*(1-b*b*(3-2*b))*conV[xc][yc]+
                       a*a*(a-1)*(1-b*b*(3-2*b))*conV[xc-1][yc]+
                       a*(a-1)*(a-1)*b*b*(3-2*b)*conV[xc][yc-1]+
                       a*a*(a-1)*b*b*(3-2*b)*conV[xc-1][yc-1]+
                       (1-a*a*(3-2*a))*b*(b-1)*(b-1)*conH[xc][yc]+
                       a*a*(3-2*a)*b*(b-1)*(b-1)*conH[xc-1][yc]+
                       (1-a*a*(3-2*a))*b*b*(b-1)*conH[xc][yc-1]+
                       a*a*(3-2*a)*b*b*(b-1)*conH[xc-1][yc-1]+
                       a*(a-1)*(a-1)*b*(b-1)*(b-1)*con2[xc][yc]+
                       a*a*(a-1)*b*(b-1)*(b-1)*con2[xc-1][yc]+
                       a*(a-1)*(a-1)*b*b*(b-1)*con2[xc][yc-1]+
                       a*a*(a-1)*b*b*(b-1)*con2[xc-1][yc-1];

   else if((q>=0)&(q<=0.5)&(w>0.5)&(w<=1))
           final[i][j]=(1-a*a*(3-2*a))*(1-b*b*(3-2*b))*current[xc][yc]+
                       a*a*(3-2*a)*(1-b*b*(3-2*b))*current[xc-1][yc]+
                       (1-a*a*(3-2*a))*b*b*(3-2*b)*current[xc][yc+1]+
                       a*a*(3-2*a)*b*b*(3-2*b)*current[xc-1][yc+1]+
                       a*(a-1)*(a-1)*(1-b*b*(3-2*b))*conV[xc][yc]+
                       a*a*(a-1)*(1-b*b*(3-2*b))*conV[xc-1][yc]+
                       a*(a-1)*(a-1)*b*b*(3-2*b)*conV[xc][yc+1]+
                       a*a*(a-1)*b*b*(3-2*b)*conV[xc-1][yc+1]+
                       (1-a*a*(3-2*a))*b*(b-1)*(b-1)*conH[xc][yc]+
                       a*a*(3-2*a)*b*(b-1)*(b-1)*conH[xc-1][yc]+
                       (1-a*a*(3-2*a))*b*b*(b-1)*conH[xc][yc+1]+
                       a*a*(3-2*a)*b*b*(b-1)*conH[xc-1][yc+1]+
                       a*(a-1)*(a-1)*b*(b-1)*(b-1)*con2[xc][yc]+
                       a*a*(a-1)*b*(b-1)*(b-1)*con2[xc-1][yc]+
                       a*(a-1)*(a-1)*b*b*(b-1)*con2[xc][yc+1]+
                       a*a*(a-1)*b*b*(b-1)*con2[xc-1][yc+1];

   else if((q>0.5)&(q<=1)&(w>=0)&(w<=0.5))
           final[i][j]=(1-a*a*(3-2*a))*(1-b*b*(3-2*b))*current[xc][yc]+
                       a*a*(3-2*a)*(1-b*b*(3-2*b))*current[xc+1][yc]+
                       (1-a*a*(3-2*a))*b*b*(3-2*b)*current[xc][yc-1]+
                       a*a*(3-2*a)*b*b*(3-2*b)*current[xc+1][yc-1]+
                       a*(a-1)*(a-1)*(1-b*b*(3-2*b))*conV[xc][yc]+
                       a*a*(a-1)*(1-b*b*(3-2*b))*conV[xc+1][yc]+
                       a*(a-1)*(a-1)*b*b*(3-2*b)*conV[xc][yc-1]+
                       a*a*(a-1)*b*b*(3-2*b)*conV[xc+1][yc-1]+
                       (1-a*a*(3-2*a))*b*(b-1)*(b-1)*conH[xc][yc]+
                       a*a*(3-2*a)*b*(b-1)*(b-1)*conH[xc+1][yc]+
                       (1-a*a*(3-2*a))*b*b*(b-1)*conH[xc][yc-1]+
                       a*a*(3-2*a)*b*b*(b-1)*conH[xc+1][yc-1]+
                       a*(a-1)*(a-1)*b*(b-1)*(b-1)*con2[xc][yc]+
                       a*a*(a-1)*b*(b-1)*(b-1)*con2[xc+1][yc]+
                       a*(a-1)*(a-1)*b*b*(b-1)*con2[xc][yc-1]+
                       a*a*(a-1)*b*b*(b-1)*con2[xc+1][yc-1];

   else if((q>0.5)&(q<=1)&(w>0.5)&(w<=1))
           final[i][j]=(1-a*a*(3-2*a))*(1-b*b*(3-2*b))*current[xc][yc]+
                       a*a*(3-2*a)*(1-b*b*(3-2*b))*current[xc+1][yc]+
                       (1-a*a*(3-2*a))*b*b*(3-2*b)*current[xc][yc+1]+
                       a*a*(3-2*a)*b*b*(3-2*b)*current[xc+1][yc+1]+
                       a*(a-1)*(a-1)*(1-b*b*(3-2*b))*conV[xc][yc]+
                       a*a*(a-1)*(1-b*b*(3-2*b))*conV[xc+1][yc]+
                       a*(a-1)*(a-1)*b*b*(3-2*b)*conV[xc][yc+1]+
                       a*a*(a-1)*b*b*(3-2*b)*conV[xc+1][yc+1]+
                       (1-a*a*(3-2*a))*b*(b-1)*(b-1)*conH[xc][yc]+
                       a*a*(3-2*a)*b*(b-1)*(b-1)*conH[xc+1][yc]+
                       (1-a*a*(3-2*a))*b*b*(b-1)*conH[xc][yc+1]+
                       a*a*(3-2*a)*b*b*(b-1)*conH[xc+1][yc+1]+
                       a*(a-1)*(a-1)*b*(b-1)*(b-1)*con2[xc][yc]+
                       a*a*(a-1)*b*(b-1)*(b-1)*con2[xc+1][yc]+
                       a*(a-1)*(a-1)*b*b*(b-1)*con2[xc][yc+1]+
                       a*a*(a-1)*b*b*(b-1)*con2[xc+1][yc+1];
   }
 }

}



int main()
{

  read();
  bicubic();
}
