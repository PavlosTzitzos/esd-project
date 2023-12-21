
// for visual studio only :
#pragma warning(disable : 4996)

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define M 498
#define N 372

// L1 Cache :
//#pragma arm section zidata="sram1"
int MM = 496; // local M
int NN = 372; // local N
int IsBinaryImage = 0;
int image_selected = 1;
int min = 0;
int max = 300;
double slope;
double a, b, c, d; // local temp variables
int f;
int i, j;
FILE* frame_c;
int ret_inp = 2;
//#pragma arm section


// L2 Cache :
//#pragma arm section zidata="sram2"
int buffer1[M + 2];                 /* Used for filters */
int buffer2[M + 2];                 /* Used for filters */
int buffer3[M + 2];                 /* Used for filters */
//#pragma arm section

// The slowest memory - Data Memory :
//#pragma arm section zidata="dram"
int frame_1_a[N][M];                /* Frame R of final rgb image */
int frame_1_b[N][M];                /* Frame G of final rgb image */
int frame_1_c[N][M];                /* Frame B of final rgb image */

int frame_padded[N + 2][M + 2];     /* Frame (I) used to load Y, scale it, apply gaussian kernel, store angle */

int frame_filtered_y[N + 2][M + 2]; /* Frame Y after appling the gaussian filter, store magnitude, normalized magnitude */

int frame_sobel_x[N + 2][M + 2];    /* Frame of image after appling the sobel filter x */
int frame_sobel_y[N + 2][M + 2];    /* Frame of image after appling the sobel filter y */
//#pragma arm section

int main()
{
    printf("\nStarted...\n\n");
    while(1)
    {
        printf("\nSelect YUV image: (default is 1) \n");
        printf("\n0. Exit Program");
        printf("\n1. cherry 496x372 420");
        printf("\n2. car 420x278 420");
        printf("\n3. cat 498x332 420");
        printf("\n4. sunflower 200x200 420");
        printf("\n5. cherry 496x372 444");
        printf("\n6. car 420x278 444");
        printf("\n7. cat 498x332 444");
        printf("\n8. sunflower 200x200 444");
        printf("\n9. cherry 496x372 binary");
        printf("\n10. car 420x278 binary");
        printf("\n11. cat 498x332 binary");
        printf("\n12. sunflower 200x200 binary\n");
        printf("\n Type number : ");
        ret_inp = scanf("%d", &image_selected);

        while (ret_inp != 1 || !(image_selected >= 0 && image_selected <= 12))
        {
            printf("\nPlease check your input...");
            printf("\n Type number again : ");
            ret_inp = scanf("%d", &image_selected);
        }

        switch (image_selected)
        {
        case 0:
        {
            printf("\nExiting...\n\n");
            return 0;
        }
        case 1:
        {
            frame_c = fopen("cherry_496x372_420.yuv", "rb");
            MM = 496; NN = 372; IsBinaryImage = 0;
        }
        case 2:
        {
            frame_c = fopen("car_420x278_420.yuv", "rb");
            MM = 420; NN = 278; IsBinaryImage = 0;
        }
        case 3:
        {
            frame_c = fopen("cat_498x332_420.yuv", "rb");
            MM = 498; NN = 332; IsBinaryImage = 0;
        }
        case 4:
        {
            frame_c = fopen("sunflower_200x200_420.yuv", "rb");
            MM = 200; NN = 200; IsBinaryImage = 0;
        }
        case 5:
        {
            frame_c = fopen("cherry_496x372_444.yuv", "rb");
            MM = 496; NN = 372; IsBinaryImage = 0;
        }
        case 6:
        {
            frame_c = fopen("car_420x278_444.yuv", "rb");
            MM = 420; NN = 278; IsBinaryImage = 0;
        }
        case 7:
        {
            frame_c = fopen("cat_498x332_444.yuv", "rb");
            MM = 498; NN = 332; IsBinaryImage = 0;
        }
        case 8:
        {
            frame_c = fopen("sunflower_200x200_444.yuv", "rb");
            MM = 200; NN = 200; IsBinaryImage = 0;
        }
        case 9:
        {
            frame_c = fopen("cherry_496x372_BIN.yuv", "rb");
            MM = 496; NN = 372; IsBinaryImage = 1;
        }
        case 10:
        {
            frame_c = fopen("car_420x278_BIN.yuv", "rb");
            MM = 420; NN = 278; IsBinaryImage = 1;
        }
        case 11:
        {
            frame_c = fopen("cat_498x332_BIN.yuv", "rb");
            MM = 498; NN = 332; IsBinaryImage = 1;
        }
        case 12:
        {
            frame_c = fopen("sunflower_200x200_BIN.yuv", "rb");
            MM = 200; NN = 200; IsBinaryImage = 1;
        }
        default:
        {
            frame_c = fopen("cherry_496x372_420.yuv", "rb");
            MM = 496; NN = 372; IsBinaryImage = 0;
        }
        }
        if (MM > M || NN > N)
        {
            printf("\nImage Size is bigger than the maximum. Correct the code then run again...\n");
            return 0;
        }

        if (frame_c == NULL)
        {
            printf("current frame doesn't exist\n");
            return 0;//exit(-1);
        }
        else {
            for (i = 1;i < NN + 1;i++)
            {
                for (j = 1;j < MM + 1;j += 4)
                {
                    // Loop Unrolling
                    frame_padded[i][j] = fgetc(frame_c);
                    frame_padded[i][j + 1] = fgetc(frame_c);
                    frame_padded[i][j + 2] = fgetc(frame_c);
                    frame_padded[i][j + 3] = fgetc(frame_c);
                }
            }
            fclose(frame_c);
        }
        /*
        for (i = 0;i < NN + 2;i += 2)
        {
            frame_padded[i][0] = 0;
            frame_padded[i][MM + 1] = 0;
            frame_padded[i + 1][0] = 0;
            frame_padded[i + 1][MM + 1] = 0;
        }
        for (j = 0;j < MM + 2;j += 2)
        {
            frame_padded[0][j] = 0;
            frame_padded[NN + 1][j] = 0;
            frame_padded[0][j + 1] = 0;
            frame_padded[NN + 1][j + 1] = 0;
        }*/
        printf("\nScale input image\n\n");
        if (IsBinaryImage == 0)
        {
            for (i = 1;i < NN + 1;i++)
            {
                for (j = 1;j < MM + 1;j++)
                {
                    f = frame_padded[i][j];
                    if (min > f)
                    {
                        min = f;
                    }
                    if (max < f)
                    {
                        max = f;
                    }
                }
            }
            slope = 255.0 / (max - min);
        }
        else
        {
            max = 1;
            min = 0;
            slope = 255;
        }

        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j += 4)
            {
                // Loop Unrolling :
                frame_padded[i][j] = (int)(slope * (frame_padded[i][j] - min) + 0.5);
                frame_padded[i][j + 1] = (int)(slope * (frame_padded[i][j + 1] - min) + 0.5);
                frame_padded[i][j + 2] = (int)(slope * (frame_padded[i][j + 2] - min) + 0.5);
                frame_padded[i][j + 3] = (int)(slope * (frame_padded[i][j + 3] - min) + 0.5);
            }
        }
        printf("\nStep 4: Apply the Gaussian filter on the Image using buffers\n\n");
        for (j = 0;j < MM + 2;j+=2)
        {
            // Loop Unrolling :
            buffer1[j] = frame_padded[0][j];
            buffer1[j + 1] = frame_padded[0][j + 1];
            buffer2[j] = frame_padded[1][j];
            buffer2[j + 1] = frame_padded[1][j + 1];
            buffer3[j] = frame_padded[2][j];
            buffer3[j + 1] = frame_padded[2][j + 1];
        }
        for (i = 1;i < NN + 1;i++)
        {

            for (j = 1;j < MM + 1;j++)
            {
                frame_filtered_y[i][j] = buffer1[j - 1] + buffer1[j] + buffer1[j] + buffer1[j + 1] +
                    buffer2[j - 1] + buffer2[j - 1] + 4 * buffer2[j] + buffer2[j + 1] + buffer2[j + 1] +
                    buffer3[j - 1] + buffer3[j] + buffer3[j] + buffer3[j + 1];
            }
            for (j = 0;j < MM + 2;j+=2)
            {
                // Loop Unrolling :
                buffer1[j] = buffer2[j];
                buffer1[j + 1] = buffer2[j + 1];
                buffer2[j] = buffer3[j];
                buffer2[j + 1] = buffer3[j + 1];
                buffer3[j] = frame_padded[i + 1][j];
                buffer3[j + 1] = frame_padded[i + 1][j + 1];
            }
        }
        printf("\nStep 5: Calculate the grad , the angle and the magnitude of the image using buffers \n\n");
        for (j = 0;j < MM + 2;j+=2)
        {
            // Loop Unrolling :
            buffer1[j] = frame_filtered_y[0][j];
            buffer1[j + 1] = frame_filtered_y[0][j + 1];
            buffer2[j] = frame_filtered_y[1][j];
            buffer2[j + 1] = frame_filtered_y[1][j + 1];
            buffer3[j] = frame_filtered_y[2][j];
            buffer3[j + 1] = frame_filtered_y[2][j + 1];
        }
        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j++)
            {
                b = (double)(buffer1[j + 1] - buffer1[j - 1] +
                    buffer2[j + 1] + buffer2[j + 1] - buffer2[j - 1] - buffer2[j - 1] +
                    buffer3[j + 1] - buffer3[j - 1]);

                c = (double)(-1 * buffer1[j - 1] - buffer1[j + 1] -
                    buffer1[j] - buffer1[j] +
                    buffer3[j] + buffer3[j] +
                    buffer3[j - 1] + buffer3[j + 1]);
                // We can imidiatly calculate the angle: 
                if (c == 0)
                    d = (atan(b / 0.01) * 180 / 3.14159265);
                else
                {
                    a = b / c;
                    d = (atan(a) * 180 / 3.14159265);
                }
                frame_sobel_x[i][j] = (int)(b + 0.5);
                frame_sobel_x[i][j] = (int)(c + 0.5);
                frame_padded[i][j]  = (int)(d + 0.5); // store angle
            }
            for (j = 0;j < MM + 2;j+=2)
            {
                // Loop Unrolling :
                buffer1[j] = buffer2[j];
                buffer1[j + 1] = buffer2[j + 1];
                buffer2[j] = buffer3[j];
                buffer2[j + 1] = buffer3[j + 1];
                buffer3[j] = frame_filtered_y[i + 1][j];
                buffer3[j + 1] = frame_filtered_y[i + 1][j + 1];
            }
        }
        min = 0; max = 500;
        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j++)
            {
                a = frame_sobel_x[i][j];
                b = frame_sobel_y[i][j];
                f = (int)sqrt(a * a + b * b); // magnitude
                if (min > f)
                {
                    min = f;
                }
                if (max < f)
                {
                    max = f;
                }
                frame_filtered_y[i][j] = f;
            }
        }
        printf("\nStep 6: Scale the magnitude image\n\n");
        slope = 255.0 / (max - min);
        for (i = 0;i < NN + 2;i++)
        {
            for (j = 0; j < MM + 2;j += 4)
            {
                // Loop Unrolling :
                frame_filtered_y[i][j] = (int)(slope * (frame_filtered_y[i][j] - min) + 0.5);
                frame_filtered_y[i][j + 1] = (int)(slope * (frame_filtered_y[i][j + 1] - min) + 0.5);
                frame_filtered_y[i][j + 2] = (int)(slope * (frame_filtered_y[i][j + 2] - min) + 0.5);
                frame_filtered_y[i][j + 3] = (int)(slope * (frame_filtered_y[i][j + 3] - min) + 0.5);
            }
        }
        printf("\nStep 7: Colour the image\n\n");
        for (i = 0; i < NN; i++)
        {
            for (j = 0; j < MM; j++)
            {
                if (abs(frame_padded[i + 1][j + 1]) >= 67.5 && abs(frame_padded[i + 1][j + 1]) <= 90) {
                    // Horizontal edge (Blue)
                    frame_1_a[i][j] = 0;                    // Red component
                    frame_1_b[i][j] = 0;                    // Green component
                    frame_1_c[i][j] = frame_filtered_y[i + 1][j + 1];// Blue component
                }
                else if (abs(frame_padded[i + 1][j + 1]) < 67.5 && abs(frame_padded[i + 1][j + 1]) > 22.5) {
                    // Diagonal edge (green)
                    frame_1_a[i][j] = 0;                    // Red component
                    frame_1_b[i][j] = frame_filtered_y[i + 1][j + 1];// Green component
                    frame_1_c[i][j] = 0;                    // Blue component
                }
                else if (abs(frame_padded[i + 1][j + 1]) < 22.5 && abs(frame_padded[i + 1][j + 1]) > 0) {
                    // Horizontal edge (red)
                    frame_1_a[i][j] = frame_filtered_y[i + 1][j + 1];// Red component
                    frame_1_b[i][j] = 0;                    // Green component
                    frame_1_c[i][j] = 0;                    // Blue component
                }
                else
                {
                    frame_1_a[i][j] = frame_filtered_y[i + 1][j + 1];// Red component
                    frame_1_b[i][j] = 0;                    // Green component
                    frame_1_c[i][j] = 0;                    // Blue component
                }
            }
        }
        frame_c = fopen("colored.yuv", "wb");
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling:
                fputc(frame_1_a[i][j], frame_c);
                fputc(frame_1_a[i][j + 1], frame_c);
                fputc(frame_1_a[i][j + 2], frame_c);
                fputc(frame_1_a[i][j + 3], frame_c);
            }
        }
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling :
                fputc(frame_1_b[i][j], frame_c);
                fputc(frame_1_b[i][j + 1], frame_c);
                fputc(frame_1_b[i][j + 2], frame_c);
                fputc(frame_1_b[i][j + 3], frame_c);
            }
        }
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling :
                fputc(frame_1_c[i][j], frame_c);
                fputc(frame_1_c[i][j + 1], frame_c);
                fputc(frame_1_c[i][j + 2], frame_c);
                fputc(frame_1_c[i][j + 3], frame_c);
            }
        }
        fclose(frame_c);

        printf("\nFinished...\n\n");
    }
    return 0;
}
