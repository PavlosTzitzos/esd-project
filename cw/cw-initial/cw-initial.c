
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Give all initial values here :
#define N 372                                           /* Frame dimension for QCIF format */
#define M 496                                           /* Frame dimension for QCIF format */

#define filename "cherry_496x372.yuv"                   /* The starting image */
#define file_yuv "cherry_yuv_readed.yuv"                /* Testing loaded image */
#define file_rgb "cherry_rgb_converted.yuv"             /* RGB image */
#define file_grayscale "cherry_grayscale_converted.yuv" /* Grayscale image */
#define file_gaussian_f "cherry_gaussian.yuv"           /* Image after appling the gaussian filter */
#define file_gradient "cherry_gradient.yuv"             /* Gradient Image */
#define file_angle "cherry_angle.yuv"                   /* Angle Image */
#define file_magnitude "cherry_magnitude.yuv"           /* Magnitude Image */
#define file_colored "cherry_colored.yuv"               /* Image after final colouring */

#define PI 3.14159265                                   /* Pi number */

#pragma arm section zidata="ram"
int test_array[3][3] = { {0,1,2},{1,2,3},{2,3,0} }; // this is for tests only

int frame_y[N][M];      /* Frame Y of original yuv image */
int frame_u[N][M];      /* Frame U of original yuv image */
int frame_v[N][M];      /* Frame V of original yuv image */
int frame_r[N][M];      /* Frame R of converted rgb image */
int frame_g[N][M];      /* Frame G of converted rgb image */
int frame_b[N][M];      /* Frame B of converted rgb image */
int frame_r_scaled[N][M];      /* Frame R of converted rgb image normalized */
int frame_g_scaled[N][M];      /* Frame G of converted rgb image normalized */
int frame_b_scaled[N][M];      /* Frame B of converted rgb image normalized */
int frame_gs_rgb[N][M]; /* Frame of converted grayscale rgb image */
int frame_gs_y[N][M];   /* Frame Y of converted grayscale yuv image */
int frame_gs_u[N][M];   /* Frame U of converted grayscale yuv image */
int frame_gs_v[N][M];   /* Frame V of converted grayscale yuv image */

int frame_padded[N+2][M+2]; /* Frame (I) used to apply the filters */

int kernel_gaussian[3][3] = { {1,2,1},    {2,4,2},  {1,2,1} };  /* Gaussian 3x3 mask */
//tex:
//$\begin{align*} G_{kernel} = \begin{bmatrix} 1 & 2 & 1 \\ 2 & 4 & 2 \\ 1 & 2 & 1 \end{bmatrix} \end{align*}$

int kernel_sobel_x[3][3]  = { {-1,0,1},   {-2,0,2}, {-1,0,1} }; /* Sobel 3x3 mask for Ix=DxI */
//tex:
//$\begin{align*} S_{kernel, x} = \begin{bmatrix} -1 & 0 & 1 \\ -2 & 0 & 2 \\ -1 & 0 & 1 \end{bmatrix} \end{align*}$

int kernel_sobel_y[3][3]  = { {-1,-2,-1}, {0,0,0},  {1,2,1} };  /* Sobel 3x3 mask for Iy=DyI */
//tex:
//$\begin{align*} S_{kernel, y} = \begin{bmatrix} -1 & -2 & -1 \\ 0 & 0 & 0 \\ 1 & 2 & 1 \end{bmatrix} \end{align*}$

int frame_filtered_y[N+2][M+2]; /* Frame Y after appling the gaussian filter */

int frame_sobel_x[N+2][M+2]; /* Frame of image after appling the sobel filter x */
int frame_sobel_y[N+2][M+2]; /* Frame of image after appling the sobel filter y */

int frame_gradient[2*(N+2)][2*(M+2)];

int frame_angle[N][M];
int frame_magnitude[N][M];

int frame_scaled[N][M];

int frame_coloured_r[N][M];
int frame_coloured_g[N][M];
int frame_coloured_b[N][M];

int frame_coloured_y[N][M];
int frame_coloured_u[N][M];
int frame_coloured_v[N][M];
#pragma arm section


int round(double a)
{
	return (int)(a+0.5);
}

// functions :

/// <summary>
/// Finds the minimum of the two given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
int min2(int a, int b)
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
int max2(int a, int b)
{
    if (a > b)
        return a;
    else if (a < b)
        return b;
    else
        return -1;
}

/// <summary>
/// Finds the minimum of the three given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// <returns></returns>
int min3(int a, int b, int c)
{
    if (a == min2(a, b))
        return min2(a, c);
    else if (b == min2(a, b))
        return min2(b, c);
    else if (c == min2(a, c) && -1 == min2(a, b))
        return c;
    else
        return -1;
}

/// <summary>
/// Finds the maximum of the three given integers
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// <returns></returns>
int max3(int a, int b, int c)
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
        exit(-1);
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
                frame_u[i][j] = fgetc(frame_c);
            }
        }
        for (i = 0;i < N;i++)
        {
            for (j = 0;j < M;j++)
            {
                frame_v[i][j] = fgetc(frame_c);
            }
        }
        fclose(frame_c);
    }
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
            fputc(frame_y[i][j], frame_yuv);
        }
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_u[i][j], frame_yuv);
        }
    }


    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_v[i][j], frame_yuv);
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
            fputc(frame_r_scaled[i][j], frame_rgb);
        }
    }

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_g_scaled[i][j], frame_rgb);
        }
    }


    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_b_scaled[i][j], frame_rgb);
        }
    }

    fclose(frame_rgb);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_grayscale_image()
{
    int i, j;

    FILE* frame_grayscale;
    frame_grayscale = fopen(file_grayscale, "wb");

    for (i = 0;i < N+2;i++)
    {
        for (j = 0;j < M+2;j++)
        {
            fputc(frame_gs_y[i][j], frame_grayscale);
        }
    }
    /*
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_gs_u[i][j], frame_grayscale);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_gs_v[i][j], frame_grayscale);
        }
    }
    */
    fclose(frame_grayscale);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_gaussian_image()
{
    int i, j;

    FILE* frame_gaussian;
    frame_gaussian = fopen(file_gaussian_f, "wb");

    for (i = 0;i < N+2;i++)
    {
        for (j = 0;j < M+2;j++)
        {
            fputc(frame_filtered_y[i][j], frame_gaussian);
        }
    }
    fclose(frame_gaussian);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_gradient_image()
{
    int i, j;

    FILE* frame_gradient_file;
    frame_gradient_file = fopen(file_gradient, "wb");

    for (i = 0;i < 2 * (N + 2);i++)
    {
        for (j = 0;j < 2 * (M + 2);j++)
        {
            fputc(frame_gradient[i][j], frame_gradient_file);
        }
    }
    fclose(frame_gradient_file);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_angle_image()
{
    int i, j;

    FILE* frame_angle_file;
    frame_angle_file = fopen(file_angle, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_angle[i][j], frame_angle_file);
        }
    }
    fclose(frame_angle_file);
}

/// <summary>
/// Writes the YUV contents of the 3 matrices into a new file
/// </summary>
void write_magnitude_image()
{
    int i, j;

    FILE* frame_magnitude_file;
    frame_magnitude_file = fopen(file_magnitude, "wb");

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_magnitude[i][j], frame_magnitude_file);
        }
    }
    fclose(frame_magnitude_file);
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
            fputc(frame_coloured_y[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_coloured_u[i][j], frame_coloured_file);
        }
    }
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            fputc(frame_coloured_v[i][j], frame_coloured_file);
        }
    }
    fclose(frame_coloured_file);
}

/// <summary>
/// This function converters an image from Y'UV to RGB.
/// <para>For more info check the following link:</para>
/// <see cref="https://answers.opencv.org/question/57188/how-to-calculate-the-yuv-values-of-a-grayscale-image/"/>
/// </summary>
void yuv_to_rgb() {
    //tex:
    //$\begin{align*} \begin{bmatrix} R \\ G \\ B \end{bmatrix} = \begin{bmatrix} 1 & 0 & 1.13983 \\ 1 & -0.39465 & -0.58060 \\ 1 & 2.03211 & 0 \end{bmatrix} \cdot \begin{bmatrix} Y \\ U \\ V \end{bmatrix} \end{align*}$
    
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_r[i][j] = frame_y[i][j] + 1.140 * frame_v[i][j];
            frame_g[i][j] = frame_y[i][j] - 0.395 * frame_u[i][j] - 0.581 * frame_v[i][j];
            frame_b[i][j] = frame_y[i][j] + 2.032 * frame_u[i][j];
        }
    }
}

/// <summary>
/// This function converters an image from RGB grayscale to Y'UV.
/// <para>Y is luminance and Y' is luma</para>
/// </summary>
void rgb_gs_to_yuv() {
    //tex:
    //$\begin{align*} \begin{bmatrix} Y' \\ U \\ V \end{bmatrix} = \begin{bmatrix} 0.299 & 0.587 & 0.114 \\ -0.14713 & -0.28886 & 0.436 \\ 0.615 & -0.51499 & -0.10001 \end{bmatrix} \cdot \begin{bmatrix} R \\ G \\ B \end{bmatrix} \end{align*}$

    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_y[i][j] =  0.299*frame_gs_rgb[i][j] + 0.587 * frame_gs_rgb[i][j] + 0.114 * frame_gs_rgb[i][j];
            frame_gs_u[i][j] = -0.147*frame_gs_rgb[i][j] - 0.289 * frame_gs_rgb[i][j] + 0.436 * frame_gs_rgb[i][j];
            frame_gs_v[i][j] =  0.615*frame_gs_rgb[i][j] - 0.515 * frame_gs_rgb[i][j] - 0.100 * frame_gs_rgb[i][j];
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
    //tex:
    //$\begin{align*} \begin{bmatrix} Y' \\ U \\ V \end{bmatrix} = \begin{bmatrix} 0.299 & 0.587 & 0.114 \\ -0.14713 & -0.28886 & 0.436 \\ 0.615 & -0.51499 & -0.10001 \end{bmatrix} \cdot \begin{bmatrix} R \\ G \\ B \end{bmatrix} \end{align*}$
    
    int i, j;

    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_coloured_y[i][j] =  0.299 * frame_coloured_r[i][j] + 0.587 * frame_coloured_g[i][j] + 0.114 * frame_coloured_b[i][j];
            frame_coloured_u[i][j] = -0.147 * frame_coloured_r[i][j] - 0.289 * frame_coloured_g[i][j] + 0.436 * frame_coloured_b[i][j];
            frame_coloured_v[i][j] =  0.615 * frame_coloured_r[i][j] - 0.515 * frame_coloured_g[i][j] - 0.100 * frame_coloured_b[i][j];
        }
    }
}

/// <summary>
/// Converts an RGB image to grayscale using the lightness method
/// </summary>
void grayscale_lightness()
{
    //tex:
    //$\begin{align*} I_{grayscale} = \frac{min(R,G,B) - max(R,G,B)}{2}\end{align*}$

    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = 0.5 * ( min3(frame_r[i][j], frame_g[i][j], frame_b[i][j]) - max3(frame_r[i][j], frame_g[i][j], frame_b[i][j]) );
        }
    }
}

/// <summary>
/// Converts an RGB image to grayscale using the average method
/// <para>gray = (R + G + B) / 3</para>
/// </summary>
void grayscale_avg()
{
    //tex:
    //$\begin{align*} I_{grayscale} = \frac{R + G + B}{3} \end{align*}$

    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = (frame_r[i][j] + frame_g[i][j] + frame_b[i][j])/3;
        }
    }
    
}

/// <summary>
/// Converts an RGB image to grayscale using the luminosity method
/// </summary>
void grayscale_luminosity()
{
    //tex:
    //$\begin{align*} I_{grayscale} = 0.3 R + 0.59 G + 0.11 B\end{align*}$
    int i, j;
    for (i = 0;i < N;i++)
    {
        for (j = 0;j < M;j++)
        {
            frame_gs_rgb[i][j] = 0.3*frame_r[i][j] + 0.59*frame_g[i][j] + 0.11*frame_b[i][j];
        }
    }
}

/// <summary>
/// This function converts an image from RGB to Grayscale
/// </summary>
/// <param name="sel">Select method</param>
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
/// This function adds one line/row around the grayscale Y image
/// </summary>
void padder()
{
    int i, j;

    //tex:
    //$\begin{align*} I_{y , padded} = \begin{bmatrix} 0 & \cdots & 0 \\ \vdots & I_{y,grayscale} & \vdots \\ 0 & \cdots & 0 \end{bmatrix} \end{align*}$
    
    for (i = 0;i < N+2;i++)
    {
        for (j = 0;j < M+2;j++)
        {
            if (i == 0 || j == 0 || i == N + 1 || j == M + 1)
            {
                frame_padded[i][j] = 0;
            }
            else
            {
                frame_padded[i][j] = frame_gs_y[i-1][j-1];
            }
        }
    }
}

/// <summary>
/// This function applies the gaussian filter on an image
/// </summary>
void gaussian_filter() {
    int i, j, k, s;

    //tex:
    //Convolution: $\begin{align*} I_{grayscale,filtered} = ( G_{kernel} \bigstar I_{grayscale} )(x,y) = \sum _{i=0} ^{N-1} \sum _{j=0} ^{M-1} G_{kernel}(x-i,y-j) I_{grayscale}(i,j)  \end{align*}$
    
    // Step 1 : make a padded copy of the image
    // Step 2 : for all elements of the image
    // Step 3 : for all the neighborhood
    // Step 4 : multiply and then sum the results
    // Step 5 : put the result into the i,j element

    padder();
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : frame_padded[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;

            //tex:
            //$\begin{align*} I_{neighborhood} = \sum _{i=-1} ^{1} \left( \sum _{j=-1} ^1 I_{padded}(i,j) \right) = \begin{bmatrix} I_{padded}(i-1,j-1) & I_{padded}(i-1,j) & I_{padded}(i-1,j+1) \\ I_{padded}(i,j-1) & I_{padded}(i,j) & I_{padded}(i,j+1) \\ I_{padded}(i+1,j-1) & I_{padded}(i+1,j) & I_{padded}(i+1,j+1) \end{bmatrix} \end{align*}$
            
            
            int neighbohood_of_image[3][3] = {
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            //tex:
            //$\begin{align*} I_{grayscale , filtered} = \sum _{k=0} ^{3} \left( \sum _{s=0} ^3 G_{kernel}(k,s) \cdot I_{neighborhood}(k,s) \right) \end{align*}$
            
            
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

    //tex:
    //Convolution: $\begin{align*} I_{x,filtered} = ( S_{x,kernel} \bigstar I_{grayscale,filtered} )(x,y) = \sum _{i=0} ^{N-1} \sum _{j=0} ^{M-1} S_{x,kernel}(x-i,y-j) I_{grayscale,filtered}(i,j)  \end{align*}$
    
    
    // Step 1 : for all elements of the image
    // Step 2 : for all the neighborhood
    // Step 3 : multiply and then sum the results
    // Step 4 : put the result into the i,j element
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : padded_image[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;

            //tex:
            //$\begin{align*} I_{neighborhood} = \sum _{i=-1} ^{1} \left( \sum _{j=-1} ^1 I_{grayscale,filtered}(i,j) \right) \end{align*}$

            
            int neighbohood_of_image[3][3] = {
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            //tex:
            //$\begin{align*} I_{x , filtered} = \sum _{k=0} ^{3} \left( \sum _{s=0} ^3 S_{x,kernel}(k,s) \cdot I_{neighborhood}(k,s) \right) \end{align*}$
            
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
    
    //tex:
    //Convolution: $\begin{align*} I_{y,filtered} = ( S_{y,kernel} \bigstar I_{grayscale,filtered} )(x,y) = \sum _{i=0} ^{N-1} \sum _{j=0} ^{M-1} S_{y,kernel}(x-i,y-j) I_{grayscale,filtered}(i,j)  \end{align*}$
    
    
    // Step 1 : for all elements of the image
    // Step 2 : for all the neighborhood
    // Step 3 : multiply and then sum the results
    // Step 4 : put the result into the i,j element
    for (i = 1;i < N;i++)
    {
        for (j = 1;j < M;j++)
        {
            // element of padded image is : padded_image[i][j]
            // element of the initial image is : grayscale_image_y[i-1][j-1]
            int sum = 0;

            //tex:
            //$\begin{align*} I_{neighborhood} = \sum _{i=-1} ^{1} \left( \sum _{j=-1} ^1 I_{grayscale,filtered}(i,j) \right) \end{align*}$

            
            int neighbohood_of_image[3][3] = {
                {frame_padded[i - 1][j - 1], frame_padded[i - 1][j], frame_padded[i - 1][j + 1]},
                {frame_padded[i][j - 1], frame_padded[i][j], frame_padded[i][j + 1]},
                {frame_padded[i + 1][j - 1], frame_padded[i + 1][j], frame_padded[i + 1][j + 1]}
            };

            //tex:
            //$\begin{align*} I_{y , filtered} = \sum _{k=0} ^{3} \left( \sum _{s=0} ^3 S_{y,kernel}(k,s) \cdot I_{neighborhood}(k,s) \right) \end{align*}$

            
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
    //tex:
    //$\begin{align*} I_x = \frac{\partial I}{\partial x} \end{align*}$
    // ,
    //$\begin{align*} I_y = \frac{\partial I}{\partial y} \end{align*}$

    sobel_filter_x();
    sobel_filter_y();
    
    // Step 2 : Calculate the gradient
    //tex:
    //$\begin{align*} \nabla I = \begin{bmatrix} I_x & I_y \end{bmatrix}' \end{align*}$

    for (int i = 0;i < (N + 2)*2;i++)
    {
        for (int j = 0;j < (M + 2)*2;j++)
        {
            if (i < N + 2 && j < M + 2)
            {
                frame_gradient[i][j] = frame_sobel_x[i][j];
            }
            else if (i >= N + 2 && j > M + 2)
            {
                frame_gradient[i][j] = frame_sobel_y[N-i][M-j];
            }
        }
    }

}

/// <summary>
/// Calculates the theta image
/// </summary>
void angle_calc()
{
    //tex:
    //$\begin{align*} \theta = \arctan{ \left( \frac{I_x}{I_y} \right) } \end{align*}$
    
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            // handling division by 0 
            if(frame_sobel_y[i][j] == 0)
                frame_angle[i][j] = atan(frame_sobel_x[i][j] / 0.01) * 180 / PI;
            else
                frame_angle[i][j] = atan(frame_sobel_x[i][j] / frame_sobel_y[i][j]) * 180 / PI;
        }
    }
}

/// <summary>
/// Calculates the magnitude image
/// </summary>
void magnitude_calc()
{
    //tex:
    //$\begin{align*} | \nabla I | = \sqrt{{I_x} ^2 + {I_y} ^2} \end{align*}$
    
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            frame_magnitude[i][j] = sqrt(pow(frame_sobel_x[i][j],2) + pow(frame_sobel_y[i][j],2));
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
    int min = 0;
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
void linear_scaling(int min , int max)
{
    int i, j;

    //tex:
    //$\begin{align*} y = a (x - x_0) + y_0 \end{align*}$
    
    double slope = 255.0 / (max - min);

    for (i = 0;i < N;i++)
    {
        for (j = 0; j < M;j++)
        {
            // for all elments of magnitude image do this :
            frame_scaled[i][j] = (int)round(slope * (frame_magnitude[i][j] - min));
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
    linear_scaling(min_magnitude,max_magnitude);
}

/// <summary>
/// Scans the array to find te maximum , most basic method
/// </summary>
/// <returns></returns>
int find_max_r()
{
    int max = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (max < frame_r[i][j])
            {
                max = frame_r[i][j];
            }
        }
    }
    return max;
}

/// <summary>
/// Scans the array to find the minimum , most basic method
/// </summary>
/// <returns></returns>
int find_min_r()
{
    int min = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (min > frame_r[i][j])
            {
                min = frame_r[i][j];
            }
        }
    }
    return min;
}

/// <summary>
/// Scans the array to find te maximum , most basic method
/// </summary>
/// <returns></returns>
int find_max_g()
{
    int max = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (max < frame_g[i][j])
            {
                max = frame_g[i][j];
            }
        }
    }
    return max;
}

/// <summary>
/// Scans the array to find the minimum , most basic method
/// </summary>
/// <returns></returns>
int find_min_g()
{
    int min = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (min > frame_g[i][j])
            {
                min = frame_g[i][j];
            }
        }
    }
    return min;
}

/// <summary>
/// Scans the array to find te maximum , most basic method
/// </summary>
/// <returns></returns>
int find_max_b()
{
    int max = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (max < frame_b[i][j])
            {
                max = frame_b[i][j];
            }
        }
    }
    return max;
}

/// <summary>
/// Scans the array to find the minimum , most basic method
/// </summary>
/// <returns></returns>
int find_min_b()
{
    int min = 0;
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < M;j++)
        {
            if (min > frame_b[i][j])
            {
                min = frame_b[i][j];
            }
        }
    }
    return min;
}

/// <summary>
/// Scales the RGB image to 0 to 255 after converting it from the original YUV
/// </summary>
void scale_rgb_image()
{
    int i,j;

    int min_r = find_min_r();
    int min_g = find_min_g();
    int min_b = find_min_b();
    int max_r = find_max_r();
    int max_g = find_max_g();
    int max_b = find_max_b();

    float slope_r = 255.0 / (max_r - min_r);
    double slope_g = 255.0 / (max_g - min_g);
    double slope_b = 255.0 / (max_b - min_b);
    for (i = 0;i < N;i++)
    {
        for (j = 0; j < M;j++)
        {
            frame_r_scaled[i][j] = (int)round(slope_r * (frame_r[i][j] - min_r));
            frame_g_scaled[i][j] = (int)round(slope_g * (frame_g[i][j] - min_g));
            frame_b_scaled[i][j] = (int)round(slope_b * (frame_b[i][j] - min_b));
        }
    }
}


/// <summary>
/// Function to assign colors based on orientation and intensity
/// </summary>
void colour_image()
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
    // Step 1: Load Image as YUV type from memory
    read_image();

    write_yuv_image(); // test if the YUV image is loaded correctly

    // Step 2: Convert YUV image to RGB
    yuv_to_rgb();
    scale_rgb_image();
    write_rgb_image(); // test if the RGB image is converted correctly

    // Step 3: Convert RGB to Grayscale
    rgb_to_grayscale(3);// use luminosity method sel=3 (prefered)
    //rgb_gs_to_yuv();
    //scale_yuv_image();
    write_grayscale_image(); // test if the grayscale image is converted correctly

    // Step 4: Apply the Gaussian filter on the Image
    gaussian_filter();
    write_gaussian_image();

    // Step 5: Calculate the angle and the magnitude og the image
    gradient_calc();
    write_gradient_image();

    angle_calc();
    write_angle_image();

    magnitude_calc();
    write_magnitude_image();

    // Step 6: Scale the magnitude image
    scale_magnitude_image();

    // Step 7: Colour the image
    colour_image();
    rgb_to_yuv();
    write_colored_image();

    // Step 8: Test

    return 0;
}
