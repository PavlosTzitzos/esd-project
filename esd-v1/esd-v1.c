// esd-v1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// This disables some warnings in Visual Studio
#pragma warning(disable : 4996)

#include <stdio.h>
#include <math.h>

// Give all initial values here :
#define N 372                                           /* Frame dimension for QCIF format */
#define M 496                                           /* Frame dimension for QCIF format */

#define filename "cherry_496x372.yuv"                   /* The starting image */
#define file_yuv "cherry_yuv_readed.yuv"                /* Testing loaded image */
#define file_rgb "cherry_rgb_converted.yuv"             /* RGB image */
#define file_grayscale "cherry_grayscale_converted.yuv" /* Grayscale image */
#define file_gaussian_f "cherry_gaussian.yuv"           /* Image after appling the gaussian filter */

#define MIN_2(a,b) ((a) < (b) ? (a) : (b))              /* Finds the minimum between 2 numbers */
#define MIN_3(a,b,c) MIN_2(MIN_2(a,b),c))               /* Finds the minimum between 3 numbers */
#define MAX_2(a,b) ((a) < (b) ? (b) : (a))              /* Finds the maximum between 2 numbers */
#define MAX_3(a,b,c) MAX_2(MAX_2(a,b),c))               /* Finds the maximum between 3 numbers */

#define PI 3.14159265                                   /* Pi number */

int test_array[3][3] = { {0,1,2},{1,2,3},{2,3,0} }; // this is for tests only

int current_y[N][M];
int current_u[N][M];
int current_v[N][M];

int current_r[N][M];
int current_g[N][M];
int current_b[N][M];

int grayscale_image_y[N][M];
int grayscale_image_u[N][M];
int grayscale_image_v[N][M];

int padded_image[N+2][M+2]; /* Image (I) used to apply the filters */

int gaussian_kernel[3][3] = { {1,2,1},    {2,4,2},  {1,2,1} };  /* Gaussian 3x3 mask */
int sobel_x_kernel[3][3]  = { {-1,0,1},   {-2,0,2}, {-1,0,1} }; /* Sobel 3x3 mask for Ix=DxI */
int sobel_y_kernel[3][3]  = { {-1,-2,-1}, {0,0,0},  {1,2,1} };  /* Sobel 3x3 mask for Iy=DyI */

int filtered_image_y[N][M];
int filtered_image_u[N][M];
int filtered_image_v[N][M];

int sobel_image_x[N+2][M+2];
int sobel_image_y[N+2][M+2];

int gradient_image[2*(N+2)][2*(M+2)];

int angle_image[N][M];
int magnitude_image[N][M];

// functions :


/// <summary>
/// Reads a YUV file and loads it into 3 matrices
/// </summary>
void read_image()
{
    int i, j;
    FILE* frame_c;
    if ((frame_c = fopen(filename, "rb")) == NULL)
    {
        printf("current frame doesn't exist\n");
        exit(-1);
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            current_y[i][j] = fgetc(frame_c);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            current_u[i][j] = fgetc(frame_c);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            current_v[i][j] = fgetc(frame_c);
        }
    }
    fclose(frame_c);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_yuv_image()
{
    int i, j;

    FILE* frame_yuv;
    frame_yuv = fopen(file_yuv, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_y[i][j], frame_yuv);
        }
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_u[i][j], frame_yuv);
        }
    }


    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_v[i][j], frame_yuv);
        }
    }

    fclose(frame_yuv);
}

/// <summary>
/// Writes the RGB contents of the 3 matrices into a new file
/// </summary>
void write_rgb_image()
{
    int i, j;

    FILE* frame_rgb;
    frame_rgb = fopen(file_rgb, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_r[i][j], frame_rgb);
        }
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_g[i][j], frame_rgb);
        }
    }


    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(current_b[i][j], frame_rgb);
        }
    }

    fclose(frame_rgb);
}

/// <summary>
/// Writes the RGB contents of the 3 matrices into a new file
/// </summary>
void write_grayscale_image()
{
    int i, j;

    FILE* frame_grayscale;
    frame_grayscale = fopen(file_grayscale, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(grayscale_image_y[i][j], frame_grayscale);
        }
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(grayscale_image_y[i][j], frame_grayscale);
        }
    }


    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(grayscale_image_y[i][j], frame_grayscale);
        }
    }

    fclose(frame_grayscale);
}


/// <summary>
/// This function converters an image from YUV to RGB
/// </summary>
void yuv_to_rgb() {
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            current_r[i][j] = current_y[i][j] + 1.140 * current_v[i][j];
            current_g[i][j] = current_y[i][j] - 0.395 * current_u[i][j] - 0.581 * current_v[i][j];
            current_b[i][j] = current_y[i][j] + 2.032 * current_u[i][j];
        }
    }
}


void grayscale_lightness()
{/*
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            grayscale_image[i][j] = 0.5 * (
                MIN_3(current_r[i][j], current_g[i][j], current_b[i][j])
                - MAX_3(current_r[i][j], current_g[i][j], current_b[i][j])
                );
        }
    }*/
}

void grayscale_avg()
{
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            grayscale_image_y[i][j] = (current_r[i][j] + current_g[i][j] + current_b[i][j])/3;
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            grayscale_image_u[i][j] = (current_r[i][j] + current_g[i][j] + current_b[i][j]) / 3;
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            grayscale_image_v[i][j] = (current_r[i][j] + current_g[i][j] + current_b[i][j]) / 3;
        }
    }
}

void grayscale_luminosity()
{
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            //grayscale_image[i][j] = 0.3*current_r[i][j] + 0.59*current_g[i][j] + 0.11*current_b[i][j];
        }
    }
}

/// <summary>
/// This function converts an image from RGB to Grayscale
/// </summary>
void rgb_to_grayscale(int sel) {
	//
    int i, j;

    switch (sel)
    {
    case 1:
        grayscale_lightness();
        break;
    case 2:
        grayscale_avg();
        break;
    case 3:
        grayscale_luminosity();
        break;
    default:
        printf("\nSelect 1 or 2\n\n");
        break;
    }
}

void padder()
{
    // 
    int i, j;

    for (i = 0;i < 2+2;i++)
    {
        for (j = 0;j < 2+2;j++)
        {
            //
            if (i == 0 || j == 0 || i == N + 2 || j == M + 2)
            {
                padded_image[i][j] = 0;
            }
            else
            {
                padded_image[i][j] = grayscale_image_y[i-1][j-1];
            }
        }
    }
}

/// <summary>
/// This function applies the gaussian filter on an image
/// </summary>
void gaussian_filter() {
    int i, j, k, s;
    // Step 1 : make a padding image
    padder();
    // Step 2 : for all elements of the image
    // Step 3 : for all the neighborhood
    // Step 4 : multiply and then sum the results
    // Step 5 : put the result into the i,j element
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : padded_image[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;
            int neighbohood_of_image[3][3] = {
                {padded_image[i - 1][j - 1], padded_image[i - 1][j], padded_image[i - 1][j + 1]},
                {padded_image[i][j - 1], padded_image[i][j], padded_image[i][j + 1]},
                {padded_image[i + 1][j - 1], padded_image[i + 1][j], padded_image[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += gaussian_kernel[k][s] * neighbohood_of_image[k][s];
                }
            }
            padded_image[i][j] = sum;
        }
    }
}

void sobel_filter_x()
{
    int i, j, k, s;
    // Step 2 : for all elements of the image
    // Step 3 : for all the neighborhood
    // Step 4 : multiply and then sum the results
    // Step 5 : put the result into the i,j element
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : padded_image[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;
            int neighbohood_of_image[3][3] = {
                {padded_image[i - 1][j - 1], padded_image[i - 1][j], padded_image[i - 1][j + 1]},
                {padded_image[i][j - 1], padded_image[i][j], padded_image[i][j + 1]},
                {padded_image[i + 1][j - 1], padded_image[i + 1][j], padded_image[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += sobel_x_kernel[k][s] * neighbohood_of_image[k][s];
                }
            }
            sobel_image_x[i][j] = sum;
        }
    }
}


void sobel_filter_y()
{
    int i, j, k, s;
    // Step 2 : for all elements of the image
    // Step 3 : for all the neighborhood
    // Step 4 : multiply and then sum the results
    // Step 5 : put the result into the i,j element
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : padded_image[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;
            int neighbohood_of_image[3][3] = {
                {padded_image[i - 1][j - 1], padded_image[i - 1][j], padded_image[i - 1][j + 1]},
                {padded_image[i][j - 1], padded_image[i][j], padded_image[i][j + 1]},
                {padded_image[i + 1][j - 1], padded_image[i + 1][j], padded_image[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += sobel_y_kernel[k][s] * neighbohood_of_image[k][s];
                }
            }
            sobel_image_y[i][j] = sum;
        }
    }
}

/// <summary>
/// Calculates the gradient image using Sobel filters
/// </summary>
void gradient_calc()
{
    sobel_filter_x();
    sobel_filter_y();

    for (int i = 0;i < (N + 2)*2;i++)
    {
        for (int j = 0;j < (M + 2)*2;j++)
        {
            //
            if (i < N + 2 && j < M + 2)
            {
                gradient_image[i][j] = sobel_image_x[i][j];
            }
            else if (i >= N + 2 && j > M + 2)
            {
                //
                gradient_image[i][j] = sobel_image_y[N-i][M-j];
            }
        }
    }

}

/// <summary>
/// Calculates the theta image
/// </summary>
void angle_calc()
{
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            angle_image[i][j] = atan(sobel_image_x[i][j] / sobel_image_y[i][j]) * 180 / PI;
        }
    }
}

/// <summary>
/// Calculates the magnitude image
/// </summary>
void magnitude_calc()
{
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            magnitude_image[i][j] = sqrt(pow(sobel_image_x[i][j],2) + pow(sobel_image_y[i][j],2));
        }
    }
}

/* DO NOT TOUCH this section
int find_max()
{
    int max = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < N;j++)
        {
            if (max < magnitude_image[i][j])
            {
                max = magnitude_image[i][j];
            }
        }
    }
    return max;
}

int find_min()
{
    int min = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < N;j++)
        {
            if (min > magnitude_image[i][j])
            {
                min = magnitude_image[i][j];
            }
        }
    }
    return min;
}

void colour_image()
{
    //
}
*/

int main()
{
	// Step 1: Load Image as YUV type from memory
	read_image();

    write_yuv_image(); // test if the YUV image is loaded correctly
	
	// Step 2: Convert YUV image to RGB
	yuv_to_rgb();

    write_rgb_image(); // test if the RGB image is converted correctly

	// Step 3: Convert RGB to Grayscale
	rgb_to_grayscale(2);
	
    write_grayscale_image(); // test if the grayscale image is converted correctly

	// Step 4: Apply the Gaussian filter on the Image
	// gaussian_filter();

	// Step 5: Calculate the angle and the magnitude og the image
    // gradient_calc();
    // angle_calc();
	// magnitude_calc();

	// Step 6: Colour the image
	// colour_image();

	// Step 8: Test

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
