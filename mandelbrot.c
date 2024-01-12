/*
c program:
--------------------------------
 1. draws Mandelbrot set for Fc(z)=z*z +c
 using Mandelbrot algorithm ( boolean escape time )
-------------------------------
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <omp.h>
#include <time.h>

void write_file(char* file_name, char* comment, int * data, int maxX, int maxY)
{
    FILE * fp;
    static unsigned char color[3];
    int iX,iY;
    /*create new file,give it a name and open it in binary mode  */
    fp= fopen(file_name,"wb"); /* b -  binary mode */
    /*write ASCII header to the file*/
    fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,maxX,maxY,255);
    /* write image data bytes to the file*/
    for(iY=0;iY<maxY;iY++)
    {
        for(iX=0;iX<maxX;iX++)
        {
            color[0] = data[iY*maxX+iX] % 256;
            color[1] = (data[iY*maxX+iX] >> 8) % 256;
            color[2] = (data[iY*maxX+iX] >> 16) % 256;
            /*write color to the file*/
            fwrite(color,1,3,fp);
        }
    }
    fclose(fp);
}

int main()
{
    /* screen ( integer) coordinate */
    int iX,iY;

    const int globalIXmax = 1920;
    const int globalIYmax = 1080;
    const int IterationMax= 65535;

    /* world ( double) coordinate = parameter plane*/
    double Cx,Cy;
    double CxMin=-2.5;
    double CxMax=1.5;
    double CyMin=-2.0;
    double CyMax=2.0;
    /* color component ( R or G or B) is coded from 0 to 255 */
    /* it is 24 bit color RGB file */
    char filename[128]="";
    char *comment="# ";/* comment should start with # */
    /* Z=Zx+Zy*i  ;   Z0 = 0 */
    double Zx, Zy;
    double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */

    /* TODO Part 1: change this based on MPI rank and size, which should be captured before */
    int iXmin=0;
    int iYmin=0;
    const int iXmax = globalIXmax;
    const int iYmax = globalIYmax;

    sprintf(filename,"mandelbrot.ppm");

    /* */
    double PixelWidth=(CxMax-CxMin)/globalIXmax;
    double PixelHeight=(CyMax-CyMin)/globalIYmax;

    /* holds iterations in */
    int* fin_iterations;
    fin_iterations=malloc((iXmax-iXmin)*(iYmax-iYmin)*sizeof(int));
    assert(fin_iterations);


    /* holds all iterations at rank 0 */
    int* all_fin_iterations;
    /* TODO Part 1: only allocate this at rank 0*/
    all_fin_iterations=malloc((globalIXmax)*(globalIYmax)*sizeof(int));
    assert(all_fin_iterations);

    /*  */
    int Iteration;
    /* bail-out value , radius of circle ;  */
    const double EscapeRadius=2;
    double ER2=EscapeRadius*EscapeRadius;

    /* TODO Part 2: Modularity: Rank 0 might take the user input for zooming, which can create a different CyMin, CxMin, CyMax, and CxMax, which should then also change PixelWidth and PixelHeight export this to a function! */
    /* TODO Part 3: Implement the input for rank 0 (WASD=up-left-down-right),E:zoom in (x2),Q:Zoom out, X:Exit program*/

    time_t start = time(NULL);

    /* compute */
#pragma omp parallel for private(Cy,Cx,Zx,Zy,Zx2,Zy2,iX,Iteration) shared(fin_iterations)
    for(iY=iYmin;iY<iYmax;iY++)
    {
        Cy=CyMin + iY*PixelHeight;
        if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
        for(iX=iXmin;iX<iXmax;iX++)
        {
            Cx=CxMin + iX*PixelWidth;
            /* initial value of orbit = critical point Z= 0 */
            Zx=0.0;
            Zy=0.0;
            Zx2=Zx*Zx;
            Zy2=Zy*Zy;
            /* */
            for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
            {
                Zy=2*Zx*Zy + Cy;
                Zx=Zx2-Zy2 +Cx;
                Zx2=Zx*Zx;
                Zy2=Zy*Zy;
            };
            fin_iterations[(iY-iYmin)*(iXmax-iXmin)+iX-iXmin]=Iteration;
        }

    }

    time_t end = time(NULL);

    /* TODO Part 1: collect global data at rank 0 in all_fin_iterations */
    /* MPI_???(); */
    /* sequential: free (all_fin_iterations); all_fin_iterations = fin_iterations; */
    free (all_fin_iterations);
    all_fin_iterations = fin_iterations;

    /* TODO Part 1:only rank 0 should write file */

    write_file(filename, comment, all_fin_iterations, globalIXmax, globalIYmax);

    printf("Total time: %ld\n", end - start);

    return 0;
}
