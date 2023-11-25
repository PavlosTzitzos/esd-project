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
#define file_colored "cherry_colored.yuv"               /* Image after final colouring */

#define PI 3.14159265                                   /* Pi number */

// double frame_optimized_1[N + 2][M + 2];
// double frame_optimized_2[N + 2][M + 2];
// double frame_optimized_3[N + 2][M + 2];


int frame_y[N][M];      /* Frame Y of original yuv image */
int frame_u[N][M];      /* Frame U of original yuv image */
int frame_v[N][M];      /* Frame V of original yuv image */
int frame_r[N][M];      /* Frame R of converted rgb image */
int frame_g[N][M];      /* Frame G of converted rgb image */
int frame_b[N][M];      /* Frame B of converted rgb image */
int frame_gs_rgb[N][M]; /* Frame of converted grayscale rgb image */
int frame_gs_y[N][M];   /* Frame Y of converted grayscale yuv image */
int frame_gs_u[N][M];   /* Frame U of converted grayscale yuv image */
int frame_gs_v[N][M];   /* Frame V of converted grayscale yuv image */

int frame_padded[N + 2][M + 2]; /* Frame (I) used to apply the filters */

int kernel_gaussian[3][3] = { {1,2,1},    {2,4,2},  {1,2,1} };  /* Gaussian 3x3 mask */
int kernel_sobel_x[3][3] = { {-1,0,1},   {-2,0,2}, {-1,0,1} }; /* Sobel 3x3 mask for Ix=DxI */
int kernel_sobel_y[3][3] = { {-1,-2,-1}, {0,0,0},  {1,2,1} };  /* Sobel 3x3 mask for Iy=DyI */

int frame_filtered_y[N + 2][M + 2]; /* Frame Y after appling the gaussian filter */

int frame_sobel_x[N + 2][M + 2]; /* Frame of image after appling the sobel filter x */
int frame_sobel_y[N + 2][M + 2]; /* Frame of image after appling the sobel filter y */

int frame_gradient[2 * (N + 2)][2 * (M + 2)];

int frame_angle[N][M];
int frame_magnitude[N][M];

int frame_scaled[N][M];

int frame_coloured_r[N][M];
int frame_coloured_g[N][M];
int frame_coloured_b[N][M];

int frame_coloured_y[N][M];
int frame_coloured_u[N][M];
int frame_coloured_v[N][M];


// functions :

/// <summary>
/// Finds the minimum of the two given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
inline int min2(int a, int b)
{
    if (a > b)
        return b;
    else if (a < b)
        return a;
    else
        return -1;
}

/// <summary>
/// Finds the maximum of the two given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
inline int max2(int a, int b)
{
    if (a > b)
        return a;
    else if (a < b)
        return b;
    else
        return 0;
}

/// <summary>
/// Finds the minimum of the three given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// <returns></returns>
inline int min3(int a, int b, int c)
{
    if (a == min2(a, b))
        return min2(a, c);
    else if (b == min2(a, b))
        return min2(b, c);
    else if (c == min2(a, c) && -1 == min2(a, b))
        return c;
    else
        return 0;
}

/// <summary>
/// Finds the maximum of the three given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// <returns></returns>
inline int max3(int a, int b, int c)
{
    if (a == max2(a, b))
        return max2(a, c);
    else if (b == max2(a, b))
        return max2(b, c);
    else if (c == max2(a, c) && -1 == max2(a, b))
        return c;
    else
        return -1;
}

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
        //exit(-1);
        
    }
    else {

        for (i = 0;i < N;i++)
        {
            for (j = 0;j < M;j++)
            {
                frame_y[i][j] = fgetc(frame_c);
            }
        }
        for (i = 0;i < N;i++)
        {
            for (j = 0;j < M;j++)
            {
                frame_u[i][j] = frame_y[i][j];//fgetc(frame_c);
            }
        }
        for (i = 0;i < N;i++)
        {
            for (j = 0;j < M;j++)
            {
                frame_v[i][j] = frame_y[i][j];//fgetc(frame_c);
            }
        }
        fclose(frame_c);
    }
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_colored_image()
{
    int i, j;

    FILE* frame_coloured_file;
    frame_coloured_file = fopen(file_colored, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_y[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_u[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_v[i][j], frame_coloured_file);
        }
    }
    fclose(frame_coloured_file);
}

/// <summary>
/// This function converters an image from Y'UV to RGB.
/// <para>Y is luminance and Y' is luma</para>
/// <para>R = Y' + 1.13983 V </para>
/// <para>G = Y' - 0.39465 U - 0.58060 V </para>
/// <para>B = Y' + 2.03211 U </para>
/// <para>For more info check the following link:</para>
/// <see cref="https://answers.opencv.org/question/57188/how-to-calculate-the-yuv-values-of-a-grayscale-image/"/>
/// </summary>
void yuv_to_rgb() {
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_r[i][j] = frame_y[i][j] + 1.140 * frame_v[i][j];
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_g[i][j] = frame_y[i][j] - 0.395 * frame_u[i][j] - 0.581 * frame_v[i][j];
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_b[i][j] = frame_y[i][j] + 2.032 * frame_u[i][j];
        }
    }
}

/// <summary>
/// This function converters an image from RGB grayscale to Y'UV.
/// <para>Y is luminance and Y' is luma</para>
/// <para>Y' = 0.299 R + 0.587 G + 0.114 B </para>
/// <para>U = -0.14713 R - 0.28886 G + 0.436 B </para>
/// <para>V = 0.615 R + -0.51499 G - 0.10001 B </para>
/// </summary>
void rgb_gs_to_yuv() {
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_y[i][j] = (int)round(0.299 * frame_gs_rgb[i][j] + 0.587 * frame_gs_rgb[i][j] + 0.114 * frame_gs_rgb[i][j]);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_u[i][j] = (int)round( - 0.147 * frame_gs_rgb[i][j] - 0.289 * frame_gs_rgb[i][j] + 0.436 * frame_gs_rgb[i][j]);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_v[i][j] = (int)round(0.615 * frame_gs_rgb[i][j] - 0.515 * frame_gs_rgb[i][j] - 0.100 * frame_gs_rgb[i][j]);
        }
    }
}

/// <summary>
/// This function converters an image from RGB grayscale to Y'UV.
/// <para>Y is luminance and Y' is luma</para>
/// <para>Y' = 0.299 R + 0.587 G + 0.114 B </para>
/// <para>U = -0.14713 R - 0.28886 G + 0.436 B </para>
/// <para>V = 0.615 R + -0.51499 G - 0.10001 B </para>
/// </summary>
void rgb_to_yuv() {
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_y[i][j] = (int)round(0.299 * frame_coloured_r[i][j] + 0.587 * frame_coloured_g[i][j] + 0.114 * frame_coloured_b[i][j]);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_u[i][j] = (int)round( - 0.147 * frame_coloured_r[i][j] - 0.289 * frame_coloured_g[i][j] + 0.436 * frame_coloured_b[i][j]);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_v[i][j] = (int)round(0.615 * frame_coloured_r[i][j] - 0.515 * frame_coloured_g[i][j] - 0.100 * frame_coloured_b[i][j]);
        }
    }
}

/// <summary>
/// Converts an RGB image to grayscale using the lightness method
/// </summary>
void grayscale_lightness()
{
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = (int)round(0.5 * (min3(frame_r[i][j], frame_g[i][j], frame_b[i][j]) - max3(frame_r[i][j], frame_g[i][j], frame_b[i][j])));
        }
    }
}

/// <summary>
/// Converts an RGB image to grayscale using the average method
/// <para>gray = (R + G + B) / 3</para>
/// </summary>
void grayscale_avg()
{
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = (frame_r[i][j] + frame_g[i][j] + frame_b[i][j]) / 3;
        }
    }

}

/// <summary>
/// Converts an RGB image to grayscale using the luminosity method
/// </summary>
void grayscale_luminosity()
{
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = (int)round(0.3 * frame_r[i][j] + 0.59 * frame_g[i][j] + 0.11 * frame_b[i][j]);
        }
    }
}

/// <summary>
/// This function converts an image from RGB to Grayscale
/// </summary>
/// <param name="sel">Select method</param>
void rgb_to_grayscale(int sel) {

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
        printf("\nSelect 1 , 2 or 3\n\n");
        break;
    }
}

/// <summary>
/// This function adds one line/row around the grayscale Y image
/// </summary>
void padder()
{
    int i, j;

    for (i = 0;i < 2 + 2;i++)
    {
        for (j = 0;j < 2 + 2;j++)
        {
            if (i == 0 || j == 0 || i == N + 2 || j == M + 2)
            {
                frame_padded[i][j] = 0;
            }
            else
            {
                frame_padded[i][j] = frame_gs_y[i - 1][j - 1];
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
            // element of padded image is : frame_padded[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;
            int neighbohood_of_image[3][3] = {
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += kernel_gaussian[k][s] * neighbohood_of_image[k][s];
                }
            }
            frame_filtered_y[i][j] = sum;
        }
    }
}

/// <summary>
/// This function applies the Sobel filter for the Ix image
/// </summary>
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
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += kernel_sobel_x[k][s] * neighbohood_of_image[k][s];
                }
            }
            frame_sobel_x[i][j] = sum;
        }
    }
}

/// <summary>
/// This function applies the Sobel filter for the Iy image
/// </summary>
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
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            for (k = 0;k < 3;k++)
            {
                for (s = 0; s < 3;s++)
                {
                    // element of kernel is : kernel[k][s]
                    sum += kernel_sobel_y[k][s] * neighbohood_of_image[k][s];
                }
            }
            frame_sobel_y[i][j] = sum;
        }
    }
}

/// <summary>
/// Calculates the gradient image using Sobel filters
/// </summary>
void gradient_calc()
{
    // Step 1 : Apply the sobel filters
    sobel_filter_x();
    sobel_filter_y();

    // Step 2 : Calculate the gradient
    for (int i = 0;i < (N + 2) * 2;i++)
    {
        for (int j = 0;j < (M + 2) * 2;j++)
        {
            if (i < N + 2 && j < M + 2)
            {
                frame_gradient[i][j] = frame_sobel_x[i][j];
            }
            else if (i >= N + 2 && j > M + 2)
            {
                frame_gradient[i][j] = frame_sobel_y[N - i][M - j];
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
            // handling division by 0 
            if (frame_sobel_y[i][j] == 0)
                frame_angle[i][j] = (int)round(atan(frame_sobel_x[i][j] / 0.01) * 180 / PI);
            else
                frame_angle[i][j] = (int)round(atan(frame_sobel_x[i][j] / frame_sobel_y[i][j]) * 180 / PI);
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
            frame_magnitude[i][j] = (int)round(sqrt(pow(frame_sobel_x[i][j], 2) + pow(frame_sobel_y[i][j], 2)));
        }
    }
}


/// <summary>
/// Scans the array to find te maximum , most basic method
/// </summary>
/// <returns></returns>
int find_max()
{
    int max = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (max < frame_magnitude[i][j])
            {
                max = frame_magnitude[i][j];
            }
        }
    }
    return max;
}

/// <summary>
/// Scans the array to find the minimum , most basic method
/// </summary>
/// <returns></returns>
int find_min()
{
    int min = 50;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (min > frame_magnitude[i][j])
            {
                min = frame_magnitude[i][j];
            }
        }
    }
    return min;
}

/// <summary>
/// Scaling function y=a*(x-x0)+y0
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
void linear_scaling(int min, int max)
{
    if (max != 0)
    {
        int i, j;
        int slope = 255 / (max - min);

        for (i = 0;i < N;i++)
        {
            for (j = 0; j < M;j++)
            {
                // for all elments of magnitude image do this :
                frame_scaled[i][j] = slope * (frame_magnitude[i][j] - min);
                // this is actually a straight line :
                // y = a*(x-x0) + y0 
                // where (x0,y0) = (min,0) , a = slope
                // and slope = (y1 - y0) / (x1 - x0) = (255 - 0) / (max - min)
                //
                // additionally round the result before saving it because we accept only integers
                // then cast it to int
            }
        }
    }
}

/// <summary>
/// Scales the magnitude image into the rgb area 0 to 255.
/// </summary>
void scale_magnitude_image()
{
    // Step 1 : find the minimum magnitude
    int min_magnitude = find_min();
    // Step 2 : find the maximum magnitude
    int max_magnitude = find_max();
    // Step 3 : linear scaling
    linear_scaling(min_magnitude, max_magnitude);
}

void colour_image_v1()
{
    int i, j;
    
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_r[i][j] = (int)round(frame_magnitude[i][j] * sin(frame_angle[i][j]));
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_g[i][j] = (int)round(frame_magnitude[i][j] * cos(frame_angle[i][j]));
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_b[i][j] = 0;
        }
    }
}


/// <summary>
/// Function to assign colors based on orientation and intensity
/// </summary>
void colour_image_v2()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            int red = 0, green = 0, blue = 0;

            if (frame_angle[i][j] > -45 && frame_angle[i][j] <= 45) {
                // Horizontal edge (Blue)
                blue = frame_scaled[i][j];
            }
            else if (frame_angle[i][j] > 45 && frame_angle[i][j] <= 135) {
                // Vertical edge (Yellow)
                red = green = frame_scaled[i][j];
            }
            else if (frame_angle[i][j] > 135 || frame_angle[i][j] <= -135) {
                // Horizontal edge (Blue)
                blue = frame_scaled[i][j];
            }
            else {
                // Inclined edge (Red/Green)
                red = green = frame_scaled[i][j];
            }

            // Set the resulting color
            frame_coloured_r[i][j] = red;       // Red component
            frame_coloured_g[i][j + 1] = green; // Green component
            frame_coloured_b[i][j + 2] = blue;  // Blue component
        }
    }
}


int main()
{
    printf("\nStep 1: Load Image as YUV type from memory\n\n");
    read_image();

    printf("\nStep 2: Convert YUV image to RGB\n\n");
    yuv_to_rgb();

    printf("\nStep 3: Convert RGB to Grayscale\n\n");
    rgb_to_grayscale(2);

    printf("\nStep 4: Apply the Gaussian filter on the Image\n\n");
    gaussian_filter();

    printf("\nStep 5: Calculate the grad , the angle and the magnitude of the image\n\n");
    gradient_calc();

    angle_calc();

    magnitude_calc();

    printf("\nStep 6: Scale the magnitude image\n\n");
    scale_magnitude_image();

    printf("\nStep 7: Colour the image\n\n");
    colour_image_v2();
    rgb_to_yuv();
    write_colored_image();

    printf("\nFinished...\n\n");
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
