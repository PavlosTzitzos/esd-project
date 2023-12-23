
// for visual studio only :
#pragma warning(disable : 4996)

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define M 498
#define N 372

// L2 Cache :
//#pragma arm section zidata="sram2"
int MM = 496; // local M
int NN = 372; // local N
int IsBinaryImage = 0;
int image_selected = 1;
int min = 0;
int max = 300;
double slope = 0;
double a, b, c, d = 0; // local temp variables
int f = 0;
int i, j = 0;
int ret_inp = 2;
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

// L1 Cache (Pointers):
//#pragma arm section zidata="sram1"
int* p0 = &ret_inp;                 // points at ret_inp
int* pM = &MM;                      // points at MM
int* pN = &NN;                      // points at NN
int* p1 = &max;                     // points at max
int* p2 = &image_selected;          // points at image_selected
int* p3 = &IsBinaryImage;           // points at IsBinaryImage
int* p4 = &min;                     // points at min
int* pf = &f;                       // points at f
int* pi = &i;                       // points at i
int* pj = &j;                       // points at j
double* ps = &slope;                // points at slope
double* pa = &a;                    // points at a
double* pb = &b;                    // points at b
double* pc = &c;                    // points at c
double* pd = &d;                    // points at d
FILE* fp = NULL;                    // frame pointer
int* pb1 = buffer1;                // points at buffer1
int* pb2 = buffer2;                // points at buffer2
int* pb3 = buffer3;                // points at buffer3
int( *pfa)[M] = frame_1_a;          // points at frame_1_a
int( *pfb)[M] = frame_1_b;          // points at frame_1_b
int( *pfc)[M] = frame_1_c;          // points at frame_1_c
int( *pfp)[M + 2] = frame_padded;   // points at frame_padded
int( *pff)[M + 2] = frame_filtered_y;// points at frame_filtered_y
int( *psx)[M + 2] = frame_sobel_x;  // points at frame_sobel_x
int( *psy)[M + 2] = frame_sobel_y;  // points at frame_sobel_y
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
        /* *p0 = scanf("%d", p2);

        while (*p0 != 1 || !(*p2 >= 0 && *p2 <= 12))
        {
            printf("\nPlease check your input...");
            printf("\n Type number again : ");
            *p0 = scanf("%d", p2);
        }*/
        
        switch (*p2)
        {
        case 0:
        {
            printf("\nExiting...\n\n");
            return 0;
        }
        case 1:
        {
            fp = fopen("cherry_496x372_420.yuv", "rb");
            *pM = 496; *pN = 372; *p3 = 0; break;
        }
        case 2:
        {
            fp = fopen("car_420x278_420.yuv", "rb");
            *pM = 420; *pN = 278; *p3 = 0; break;
        }
        case 3:
        {
            fp = fopen("cat_498x332_420.yuv", "rb");
            *pM = 498; *pN = 332; *p3 = 0; break;
        }
        case 4:
        {
            fp = fopen("sunflower_200x200_420.yuv", "rb");
            *pM = 200; *pN = 200; *p3 = 0; break;
        }
        case 5:
        {
            fp = fopen("cherry_496x372_444.yuv", "rb");
            *pM = 496; *pN = 372; *p3 = 0; break;
        }
        case 6:
        {
            fp = fopen("car_420x278_444.yuv", "rb");
            *pM = 420; *pN = 278; *p3 = 0; break;
        }
        case 7:
        {
            fp = fopen("cat_498x332_444.yuv", "rb");
            *pM = 498; *pN = 332; *p3 = 0; break;
        }
        case 8:
        {
            fp = fopen("sunflower_200x200_444.yuv", "rb");
            *pM = 200; *pN = 200; *p3 = 0; break;
        }
        case 9:
        {
            fp = fopen("cherry_496x372_BIN.yuv", "rb");
            *pM = 496; *pN = 372; *p3 = 1; break;
        }
        case 10:
        {
            fp = fopen("car_420x278_BIN.yuv", "rb");
            *pM = 420; *pN = 278; *p3 = 1; break;
        }
        case 11:
        {
            fp = fopen("cat_498x332_BIN.yuv", "rb");
            *pM = 498; *pN = 332; *p3 = 1; break;
        }
        case 12:
        {
            fp = fopen("sunflower_200x200_BIN.yuv", "rb");
            *pM = 200; *pN = 200; *p3 = 1; break;
        }
        default:
        {
            fp = fopen("cherry_496x372_420.yuv", "rb");
            *pM = 496; *pN = 372; *p3 = 0; break;
        }
        }

        if (*pM > M || *pN > N)
        {
            printf("\nImage Size is bigger than the maximum. Correct the code then run again...\n");
            return 0;
        }

        if (fp == NULL)
        {
            printf("current frame doesn't exist\n");
            return 0;//exit(-1);
        }
        else
        {
            for (i = 1;i < NN + 1;i++)
            {
                for (j = 1;j < MM + 1;j+=4)
                {
                    *(*(pfp + i) + j) = fgetc(fp);
                    *(*(pfp + i) + j + 1) = fgetc(fp);
                    *(*(pfp + i) + j + 2) = fgetc(fp);
                    *(*(pfp + i) + j + 3) = fgetc(fp);
                }
            }
            fclose(fp);
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
                    //f = frame_padded[i][j];
                    f = *(*(pfp + i) + j);
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
                *(*(pfp + i) + j) = (int)(slope * (*(*(pfp + i) + j) - min) + 0.5);
                *(*(pfp + i) + j + 1) = (int)(slope * (*(*(pfp + i) + j + 1) - min) + 0.5);
                *(*(pfp + i) + j + 2) = (int)(slope * (*(*(pfp + i) + j + 2) - min) + 0.5);
                *(*(pfp + i) + j + 3) = (int)(slope * (*(*(pfp + i) + j + 3) - min) + 0.5);
            }
        }
        printf("\nStep 4: Apply the Gaussian filter on the Image using buffers\n\n");
        pb1 = pfp;
        pb2 = pfp + 1;
        pb3 = pfp + 2;
        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j++)
            {
                *(*(pff + i) + j) = *(pb1 + j - 1) + *(pb1 + j) + *(pb1 + j) + *(pb1 + j + 1) + 
                    *(pb2 + j - 1) + *(pb2 + j - 1) + *(pb2 + j) + *(pb2 + j) + *(pb2 + j) + *(pb2 + j) + *(pb2 + j + 1) + *(pb2 + j + 1) +
                    *(pb3 + j - 1) + *(pb3 + j) + *(pb3 + j) + *(pb3 + j + 1);
            }
            // Buffers change :
            pb1 = pb2;
            pb2 = pb3;
            pb3 = pb3++;
        }
        printf("\nStep 5: Calculate the grad , the angle and the magnitude of the image using buffers \n\n");
        pb1 = pff;
        pb2 = pff + 1;
        pb3 = pff + 2;
        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j++)
            {
                b = (double)(
                    *(pb1 + j + 1) - *(pb1 + j - 1) +
                    *(pb2 + j + 1) + *(pb2 + j + 1) - *(pb2 + j - 1) - *(pb2 + j - 1) +
                    *(pb3 + j + 1) - *(pb3 + j - 1));

                c = (double)(
                    *(pb3 + j + 1) + *(pb3 + j - 1) + *(pb3 + j) + *(pb3 + j) -
                    *(pb1 + j + 1) - *(pb1 + j - 1) - *(pb1 + j) - *(pb1 + j));

                // We can imidiatly calculate the angle: 
                if (c == 0)
                    d = (atan(b / 0.01) * 180 / 3.14159265);
                else
                {
                    a = b / c;
                    d = (atan(a) * 180 / 3.14159265);
                }
                *(*(psx + i) + j) = (int)(b + 0.5);
                *(*(psy + i) + j) = (int)(c + 0.5);
                *(*(pfp + i) + j) = (int)(d + 0.5); // store angle
            }
            // Buffers change :
            pb1 = pb2;
            pb2 = pb3;
            pb3 = pb3++;
        }
        min = 0; max = 500;
        for (i = 1;i < NN + 1;i++)
        {
            for (j = 1;j < MM + 1;j++)
            {
                a = *(*(psx + i) + j);
                b = *(*(psx + i) + j);
                f = (int)sqrt(a * a + b * b); // magnitude
                if (min > f)
                {
                    min = f;
                }
                if (max < f)
                {
                    max = f;
                }
                *(*(pff + i) + j) = f;
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
        fp = fopen("colored.yuv", "wb");
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling:
                fputc(frame_1_a[i][j], fp);
                fputc(frame_1_a[i][j + 1], fp);
                fputc(frame_1_a[i][j + 2], fp);
                fputc(frame_1_a[i][j + 3], fp);
            }
        }
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling :
                fputc(frame_1_b[i][j], fp);
                fputc(frame_1_b[i][j + 1], fp);
                fputc(frame_1_b[i][j + 2], fp);
                fputc(frame_1_b[i][j + 3], fp);
            }
        }
        for (i = 0;i < NN;i++)
        {
            for (j = 0;j < MM;j+=4)
            {
                // Loop Unrolling :
                fputc(frame_1_c[i][j], fp);
                fputc(frame_1_c[i][j + 1], fp);
                fputc(frame_1_c[i][j + 2], fp);
                fputc(frame_1_c[i][j + 3], fp);
            }
        }
        fclose(fp);

        printf("\nFinished...\n\n");
    }
    return 0;
}
