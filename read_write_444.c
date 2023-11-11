#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 372 /* frame dimension for QCIF format */
#define M 496 /* frame dimension for QCIF format */
#define filename "cherry_496x372.yuv"
#define file_yuv "cherry_yuv_output.yuv"

/* code for armulator*/
#pragma arm section zidata="ram"
int current_y[N][M];
int current_u[N][M];
int current_v[N][M];
#pragma arm section

int i,j;



void read()
{
  FILE *frame_c;
  if((frame_c=fopen(filename,"rb"))==NULL)
  {
    printf("current frame doesn't exist\n");
    exit(-1);
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_y[i][j]=fgetc(frame_c);
    }
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_u[i][j]=fgetc(frame_c);
    }
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_v[i][j]=fgetc(frame_c);
    }
  }
  fclose(frame_c);
}

void write()
{
  FILE *frame_yuv;
  frame_yuv=fopen(file_yuv,"wb");

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_y[i][j],frame_yuv);
    }
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_u[i][j],frame_yuv);
    }
  }


  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_v[i][j],frame_yuv);
    }
  }
  fclose(frame_yuv);
}

int main()
{
  read();
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_y[i][j]);
    }
	printf("\n");
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_u[i][j]);
    }
	printf("\n");
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_v[i][j]);
    }
	printf("\n");
  }
  write();
}
