// esd-v1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// This disables some warnings in Visual Studio
#pragma warning(disable : 4996)

#include <stdio.h>

// Give all initial values here :
#define N 372 /* frame dimension for QCIF format */
#define M 496 /* frame dimension for QCIF format */
#define filename "cherry_496x372.yuv"
#define file_yuv "cherry_yuv_readed.yuv"
#define file_rgb "cherry_rgb_converted.raw"
#define file_grayscale "cherry_grayscale_converted.raw"

#define MIN_2(a,b) ((a) < (b) ? (a) : (b))
#define MIN_3(a,b,c) MIN_2(MIN_2(a,b),c))
#define MAX_2(a,b) ((a) < (b) ? (b) : (a))
#define MAX_3(a,b,c) MAX_2(MAX_2(a,b),c))


int current_y[N][M];
int current_u[N][M];
int current_v[N][M];

int current_r[N][M];
int current_g[N][M];
int current_b[N][M];

int grayscale_image_y[N][M];
int grayscale_image_u[N][M];
int grayscale_image_v[N][M];

int gaussian_kernel[3][3] = { {1,2,1},{2,4,2},{1,2,1} };

int filtered_image_y[N][M];
int filtered_image_u[N][M];
int filtered_image_v[N][M];

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

/// <summary>
/// This function applies the gaussian filter on an image
/// </summary>
void filter() {
    //
    int i, j, k;

    for (i = 0;i < N*M;i++)
    {
        for (j = 0;j < 3;j++)
        {
            for (k = 0;k < 3;k++)
            {
                filtered_image_y[i][j] = gaussian_kernel[j][k] * grayscale_image_y[i][j];
            }
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
	// filter();

	// Step 5: Calculate the angle and the magnitude og the image
	// angle();
	// magnitude();

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
