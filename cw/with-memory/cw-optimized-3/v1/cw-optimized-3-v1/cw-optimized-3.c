
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N 372
#define M 496


/* code for armulator*/
int frame_1_a[N][M];      /* Frame R of final rgb image */
int frame_1_b[N][M];      /* Frame G of final rgb image */
int frame_1_c[N][M];      /* Frame B of final rgb image */

int frame_padded[N + 2][M + 2]; /* Frame (I) used to load Y, scale it, apply gaussian kernel, store angle */

int frame_filtered_y[N + 2][M + 2]; /* Frame Y after appling the gaussian filter, store magnitude, normalized magnitude */

int frame_sobel_x[N + 2][M + 2]; /* Frame of image after appling the sobel filter x */
int frame_sobel_y[N + 2][M + 2]; /* Frame of image after appling the sobel filter y */
//#pragma arm section zidata="sram"
int buffer1[M + 2];         /*  */
int buffer2[M + 2];         /*  */
int buffer3[M + 2];         /*  */
//#pragma arm section


int round(double a)
{
    return (int)(a + 0.5);
}

int main()
{
    //printf("\nStarted...\n\n");
    //int kernel_gaussian[3][3] = { {1,2,1},    {2,4,2},  {1,2,1} };
    //int kernel_sobel_x[3][3] = { {-1,0,1},   {-2,0,2}, {-1,0,1} };
    int min = 0;
    int max = 300;
    double slope;
    int pixel;
    double a,b;
    //printf("\nStep 1: Load Image as YUV type from memory\n\n");
    int i, j;
    FILE* frame_c;
    FILE* frame_coloured_file;
    if ((frame_c = fopen("cherry_496x372.yuv", "rb")) == NULL)
    {
        printf("current frame doesn't exist\n");
        return 0;//exit(-1);
    }
    else {
        for (i = 1;i < N+1;i++)
        {
            for (j = 1;j < M+1;j+=4)
            {
                //frame_1_a[i][j] = fgetc(frame_c);
                frame_padded[i][j]     = fgetc(frame_c);
                frame_padded[i][j + 1] = fgetc(frame_c);
                frame_padded[i][j + 2] = fgetc(frame_c);
                frame_padded[i][j + 3] = fgetc(frame_c);
            }
        }
        fclose(frame_c);
    }
    for (i = 0;i < N + 2;i += 2)
    {
        frame_padded[i][0] = 0;
        frame_padded[i][M + 1] = 0;
        frame_padded[i + 1][0] = 0;
        frame_padded[i + 1][M + 1] = 0;
    }
    for (j = 0;j < M + 2;j += 2)
    {
        frame_padded[0][j] = 0;
        frame_padded[N + 1][j] = 0;
        frame_padded[0][j + 1] = 0;
        frame_padded[N + 1][j + 1] = 0;
    }
    for (i = 1;i < N+1;i++)
    {
        for (j = 1;j < M+1;j++)
        {
            pixel = frame_padded[i][j];
            if (min > pixel)
            {
                min = pixel;
            }
            if (max < pixel)
            {
                max = pixel;
            }
        }
    }
    slope = 255.0 / (max - min);

    for (i = 1;i < N+1;i++)
    {
        for (j = 1;j < M+1;j += 4)
        {
            frame_padded[i][j]     = round(slope * (frame_padded[i][j]     - min));
            frame_padded[i][j + 1] = round(slope * (frame_padded[i][j + 1] - min));
            frame_padded[i][j + 2] = round(slope * (frame_padded[i][j + 2] - min));
            frame_padded[i][j + 3] = round(slope * (frame_padded[i][j + 3] - min));
        }
    }
    //printf("\nStep 4: Apply the Gaussian filter on the Image\n\n");
    for (j = 0;j < M + 2;j++) {
        buffer1[j] = frame_padded[0][j];
        buffer2[j] = frame_padded[1][j];
        buffer3[j] = frame_padded[2][j];
    }
    for (i = 1;i < N + 1;i++)
    {

        for (j = 1;j < M + 1;j++)
        {
            frame_filtered_y[i][j] = buffer1[j - 1]                  + buffer1[j] + buffer1[j] + buffer1[j + 1] +
                                     buffer2[j - 1] + buffer2[j - 1] + 4 * buffer2[j]          + buffer2[j + 1] + buffer2[j + 1] + 
                                     buffer3[j - 1]                  + buffer3[j] + buffer3[j] + buffer3[j + 1];
        }
        for (j = 0;j < M + 2;j++) {
            buffer1[j] = buffer2[j];
            buffer2[j] = buffer3[j];
            buffer3[j] = frame_padded[i + 1][j];
        }
    }
    //printf("\nStep 5: Calculate the grad , the angle and the magnitude of the image\n\n");
    for (j = 0;j < M + 2;j++) {
        buffer1[j] = frame_filtered_y[0][j];
        buffer2[j] = frame_filtered_y[1][j];
        buffer3[j] = frame_filtered_y[2][j];
    }
    for (i = 1;i < N + 1;i++)
    {
        for (j = 1;j < M + 1;j++)
        {
            frame_sobel_x[i][j] = buffer1[j + 1] - buffer1[j - 1] +
                                  buffer2[j + 1] + buffer2[j + 1] - buffer2[j - 1] - buffer2[j - 1] +
                                  buffer3[j + 1] - buffer3[j - 1];

            frame_sobel_y[i][j] = -1 * buffer1[j - 1] - buffer1[j + 1] -
                                    buffer1[j]     - buffer1[j] + 
                                    buffer3[j]     + buffer3[j] +
                                    buffer3[j - 1] + buffer3[j + 1];

        }
        for (j = 0;j < M + 2;j++) {
            buffer1[j] = buffer2[j];
            buffer2[j] = buffer3[j];
            buffer3[j] = frame_filtered_y[i + 1][j];
        }
    }
    for (i = 1;i < N+1;i++)
    {
        for (j = 1;j < M+1;j++)
        {
            // handling division by 0 
            if (frame_sobel_y[i][j] == 0)
                //frame_angle[i][j] = (int)(atan(frame_sobel_x[i][j] / 0.01) * 180 / 3.14159265);
                frame_padded[i][j] = (int)(atan(frame_sobel_x[i][j] / 0.01) * 180 / 3.14159265);
            else
            {
                a = frame_sobel_x[i][j] / frame_sobel_y[i][j];
                //frame_angle[i][j] = (int)(atan(a) * 180 / 3.14159265);
                frame_padded[i][j] = (int)(atan(a) * 180 / 3.14159265);
            }
        }
    }
    for (i = 1;i < N+1;i++)
    {
        for (j = 1;j < M+1;j++)
        {
            a = frame_sobel_x[i][j];
            b = frame_sobel_y[i][j];
            frame_filtered_y[i][j] = (int)sqrt(a * a + b * b);
        }
    }
    //printf("\nStep 6: Scale the magnitude image\n\n");
    min = 0;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            if (min > frame_filtered_y[i][j])
            {
                min = frame_filtered_y[i][j];
            }
        }
    }
    max = 500;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            if (max < frame_filtered_y[i][j])
            {
                max = frame_filtered_y[i][j];
            }
        }
    }
    slope = 255.0 / (max - min);
    for (i = 0;i < N+2;i++)
    {
        for (j = 0; j < M+2;j += 4)
        {
            frame_filtered_y[i][j]     = round(slope * (frame_filtered_y[i][j]     - min));
            frame_filtered_y[i][j + 1] = round(slope * (frame_filtered_y[i][j + 1] - min));
            frame_filtered_y[i][j + 2] = round(slope * (frame_filtered_y[i][j + 2] - min));
            frame_filtered_y[i][j + 3] = round(slope * (frame_filtered_y[i][j + 3] - min));
        }
    }

    //printf("\nStep 7: Colour the image\n\n");

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            if (abs(frame_padded[i+1][j+1]) >= 67.5 && abs(frame_padded[i+1][j+1]) <= 90) {
                // Horizontal edge (Blue)
                frame_1_a[i][j] = 0;                    // Red component
                frame_1_b[i][j] = 0;                    // Green component
                frame_1_c[i][j] = frame_filtered_y[i+1][j+1];// Blue component
            }
            else if (abs(frame_padded[i+1][j+1]) < 67.5 && abs(frame_padded[i+1][j+1]) > 22.5) {
                // Diagonal edge (green)
                frame_1_a[i][j] = 0;                    // Red component
                frame_1_b[i][j] = frame_filtered_y[i+1][j+1];// Green component
                frame_1_c[i][j] = 0;                    // Blue component
            }
            else if (abs(frame_padded[i+1][j+1]) < 22.5 && abs(frame_padded[i+1][j+1]) > 0) {
                // Horizontal edge (red)
                frame_1_a[i][j] = frame_filtered_y[i+1][j+1];// Red component
                frame_1_b[i][j] = 0;                    // Green component
                frame_1_c[i][j] = 0;                    // Blue component
            }
            else
            {
                frame_1_a[i][j] = frame_filtered_y[i+1][j+1];// Red component
                frame_1_b[i][j] = 0;                    // Green component
                frame_1_c[i][j] = 0;                    // Blue component
            }
        }
    }
    frame_coloured_file = fopen("cherry_colored.yuv", "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_1_a[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_1_b[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_1_c[i][j], frame_coloured_file);
        }
    }
    fclose(frame_coloured_file);

    //printf("\nFinished...\n\n");
    return 0;
}
