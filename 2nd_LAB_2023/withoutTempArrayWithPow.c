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
double conV[N][M],conH[N][M],con2[N][M];
int final[N*s][M*s];
//#pragma arm section

int GV[3][3],GH[3][3],G2[3][3];
int i,j;
int xc,yc,xf,yf,k1,k2,l1,l2;
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

 
GV[0][0]=GV[1][0]=GV[2][0]=1;
GV[0][1]=GV[1][1]=GV[2][1]=0;
GV[0][2]=GV[1][2]=GV[2][2]=-1;

GH[0][0]=GH[0][1]=GH[0][2]=-1;
GH[1][0]=GH[1][1]=GH[1][2]=0;
GH[2][0]=GH[2][1]=GH[2][2]=1;

G2[0][0]=G2[0][2]=G2[2][0]=G2[2][2]=0;
G2[0][1]=G2[1][0]=G2[1][2]=G2[2][1]=1;
G2[1][1]=-4;

 for(i=0;i<N;++i)
 {
   k1 = (i==0)?0:1;
   k2 = (i==(N-1))?0:1;
    for(j=0;j<M;++j)
    {
     l1 = (j==0)?0:1;
     l2 = (j==(M-1))?0:1;
     conV[i][j]=k1*l1*GV[0][0]*current[i-k1][j-l1]+k1*GV[0][1]*current[i-k1][j]+k1*l2*GV[0][2]*current[i-k1][j+l2]+l1*GV[1][0]*current[i][j-l1]+GV[1][1]*current[i][j]+l2*GV[1][2]*current[i][j+l2]+k2*l1*GV[2][0]*current[i+k2][j-l1]+k2*GV[2][1]*current[i+k2][j]+k2*l2*GV[2][2]*current[i+k2][j+l2];
     conH[i][j]=k1*l1*GH[0][0]*current[i-k1][j-l1]+k1*GH[0][1]*current[i-k1][j]+k1*l2*GH[0][2]*current[i-k1][j+l2]+l1*GH[1][0]*current[i][j-l1]+GH[1][1]*current[i][j]+l2*GH[1][2]*current[i][j+l2]+k2*l1*GH[2][0]*current[i+k2][j-l1]+k2*GH[2][1]*current[i+k2][j]+k2*l2*GH[2][2]*current[i+k2][j+l2];
     con2[i][j]=k1*G2[0][1]*current[i-k1][j]+l1*G2[1][0]*current[i][j-l1]+G2[1][1]*current[i][j]+l2*G2[1][2]*current[i][j+l2]+k2*G2[2][1]*current[i+k2][j];
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
           final[i][j]=(1-pow(a,2)*(3-2*a))*(1-pow(b,2)*(3-2*b))*current[xc][yc]+
                       pow(a,2)*(3-2*a)*(1-pow(b,2)*(3-2*b))*current[xc-1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(3-2*b)*current[xc][yc-1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(3-2*b)*current[xc-1][yc-1]+
                       a*pow((a-1),2)*(1-pow(b,2)*(3-2*b))*conV[xc][yc]+
                       pow(a,2)*(a-1)*(1-pow(b,2)*(3-2*b))*conV[xc-1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(3-2*b)*conV[xc][yc-1]+
                       pow(a,2)*(a-1)*pow(b,2)*(3-2*b)*conV[xc-1][yc-1]+
                       (1-pow(a,2)*(3-2*a))*b*pow((b-1),2)*conH[xc][yc]+
                       pow(a,2)*(3-2*a)*b*pow((b-1),2)*conH[xc-1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(b-1)*conH[xc][yc-1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(b-1)*conH[xc-1][yc-1]+
                       a*pow((a-1),2)*b*pow((b-1),2)*con2[xc][yc]+
                       pow(a,2)*(a-1)*b*pow((b-1),2)*con2[xc-1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(b-1)*con2[xc][yc-1]+
                       pow(a,2)*(a-1)*pow(b,2)*(b-1)*con2[xc-1][yc-1];

   else if((q>=0)&(q<=0.5)&(w>0.5)&(w<=1))
           final[i][j]=(1-pow(a,2)*(3-2*a))*(1-pow(b,2)*(3-2*b))*current[xc][yc]+
                       pow(a,2)*(3-2*a)*(1-pow(b,2)*(3-2*b))*current[xc-1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(3-2*b)*current[xc][yc+1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(3-2*b)*current[xc-1][yc+1]+
                       a*pow((a-1),2)*(1-pow(b,2)*(3-2*b))*conV[xc][yc]+
                       pow(a,2)*(a-1)*(1-pow(b,2)*(3-2*b))*conV[xc-1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(3-2*b)*conV[xc][yc+1]+
                       pow(a,2)*(a-1)*pow(b,2)*(3-2*b)*conV[xc-1][yc+1]+
                       (1-pow(a,2)*(3-2*a))*b*pow((b-1),2)*conH[xc][yc]+
                       pow(a,2)*(3-2*a)*b*pow((b-1),2)*conH[xc-1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(b-1)*conH[xc][yc+1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(b-1)*conH[xc-1][yc+1]+
                       a*pow((a-1),2)*b*pow((b-1),2)*con2[xc][yc]+
                       pow(a,2)*(a-1)*b*pow((b-1),2)*con2[xc-1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(b-1)*con2[xc][yc+1]+
                       pow(a,2)*(a-1)*pow(b,2)*(b-1)*con2[xc-1][yc+1];

   else if((q>0.5)&(q<=1)&(w>=0)&(w<=0.5))
           final[i][j]=(1-pow(a,2)*(3-2*a))*(1-pow(b,2)*(3-2*b))*current[xc][yc]+
                       pow(a,2)*(3-2*a)*(1-pow(b,2)*(3-2*b))*current[xc+1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(3-2*b)*current[xc][yc-1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(3-2*b)*current[xc+1][yc-1]+
                       a*pow((a-1),2)*(1-pow(b,2)*(3-2*b))*conV[xc][yc]+
                       pow(a,2)*(a-1)*(1-pow(b,2)*(3-2*b))*conV[xc+1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(3-2*b)*conV[xc][yc-1]+
                       pow(a,2)*(a-1)*pow(b,2)*(3-2*b)*conV[xc+1][yc-1]+
                       (1-pow(a,2)*(3-2*a))*b*pow((b-1),2)*conH[xc][yc]+
                       pow(a,2)*(3-2*a)*b*pow((b-1),2)*conH[xc+1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(b-1)*conH[xc][yc-1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(b-1)*conH[xc+1][yc-1]+
                       a*pow((a-1),2)*b*pow((b-1),2)*con2[xc][yc]+
                       pow(a,2)*(a-1)*b*pow((b-1),2)*con2[xc+1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(b-1)*con2[xc][yc-1]+
                       pow(a,2)*(a-1)*pow(b,2)*(b-1)*con2[xc+1][yc-1];

   else if((q>0.5)&(q<=1)&(w>0.5)&(w<=1))
           final[i][j]=(1-pow(a,2)*(3-2*a))*(1-pow(b,2)*(3-2*b))*current[xc][yc]+
                       pow(a,2)*(3-2*a)*(1-pow(b,2)*(3-2*b))*current[xc+1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(3-2*b)*current[xc][yc+1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(3-2*b)*current[xc+1][yc+1]+
                       a*pow((a-1),2)*(1-pow(b,2)*(3-2*b))*conV[xc][yc]+
                       pow(a,2)*(a-1)*(1-pow(b,2)*(3-2*b))*conV[xc+1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(3-2*b)*conV[xc][yc+1]+
                       pow(a,2)*(a-1)*pow(b,2)*(3-2*b)*conV[xc+1][yc+1]+
                       (1-pow(a,2)*(3-2*a))*b*pow((b-1),2)*conH[xc][yc]+
                       pow(a,2)*(3-2*a)*b*pow((b-1),2)*conH[xc+1][yc]+
                       (1-pow(a,2)*(3-2*a))*pow(b,2)*(b-1)*conH[xc][yc+1]+
                       pow(a,2)*(3-2*a)*pow(b,2)*(b-1)*conH[xc+1][yc+1]+
                       a*pow((a-1),2)*b*pow((b-1),2)*con2[xc][yc]+
                       pow(a,2)*(a-1)*b*pow((b-1),2)*con2[xc+1][yc]+
                       a*pow((a-1),2)*pow(b,2)*(b-1)*con2[xc][yc+1]+
                       pow(a,2)*(a-1)*pow(b,2)*(b-1)*con2[xc+1][yc+1];
   }
 }

}



int main()
{
  read();
  bicubic();
}
