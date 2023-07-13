 /* Copyright (C) 2023 Seho Kim

	This file is part of SOCRATES.

    SOCRATES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SOCRATES is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */
    
#ifndef __UTIL_H__
#define __UTIL_H__

#include "sigkit.h"

void calloc_double(double *var, size_t size);
void cprintf(double complex z, char delimiter);
long decodeStr(char *s);
void readLineParsor(FILE *infile, double *data, const char *delimiter);
void readLineParsorStr(FILE *infile, char **data);
long UniformRandomDiscrete(struct RandomProcessType *RP, int lb, int ub);
void delay(int number_of_seconds);
int msta1(double x,int mp);
int msta2(double x,int n,int mp);
int cbessjy01(double complex z, double complex *cj0, double complex *cj1, double complex *cy0,double complex *cy1,double complex *cj0p,
    double complex *cj1p,double complex *cy0p,double complex *cy1p);
int cbessjyna(int n, double complex z, int *nm, double complex *cj, double complex *cy, double complex *cjp, double complex *cyp);
int cbessh(int n, double complex z, int *nm, double complex *ch, double complex *chp);
double unifpdf(double x, double a, double b);
void hinkley_pdf(double mean1, double mean2, double std1, double std2, double corr_coeff, double hinkley[2]);
void hinkley_pdf2(double mean1, double mean2, double std1, double std2, double corr_coeff, long npts, double hinkley[2]);
double* linspace(double a, double b, int n, double u[]);
double rand01(void);
double randxy(double x, double y);
double AWGN_generator();
double var(double *x, long dim);
double roundzero(double x);
double newatan2(double y, double x);
double atan2TwoPi(double y, double x);
double complex sqrte(double complex z);
double newpow(double x, double y);
double Limit_max(double x, double max);
void sph2car(double theta, double phi, double AR, double AT, double AP, double axis[3]);
void copyV3(double dest[3], double sour[3]);
double complex VoV_complex(double complex A[3], double complex B[3]);
void M4xV4_complex(double complex M[4][4], double complex V[4], double complex W[4]);
void M2xV2_complex(double complex M[2][2], double complex V[2], double complex W[2]);
void MTxV_complex (double M[3][3], double V[3], double complex W[3]);
void M4xM4_complex(double complex A[4][4], double complex B[4][4], double complex C[4][4]);
void M2xM2_complex(double complex A[2][2], double complex B[2][2], double complex C[2][2]);
void CopyMT3(double A[3][3], double B[3][3]);
void MINV2_complex(double complex A[2][2], double complex B[2][2]);
void RotX(double th, double R[3][3]);
void RotY(double th, double R[3][3]);
void RotZ(double th, double R[3][3]);
void newFindCLN(double r[3], double v[3], double CLN[3][3]);
void gaussEliminationLS(int m, int n, double a[m][n], double x[n-1]);
void GRS80ToECEF(double glat, double glong, double alt, double p[3]);
void ECEFToGRS80(double p[3], double *glat, double *glong, double *alt);
double YMDHMS2JD(long Year, long Month, long Day, long Hour, long Minute, double Second);
void JD2YMDHMS(double JD,long *Year, long *Month, long *Day, long *Hour, long *Minute, double *Second);
void ease2_map_info(struct MapType *map, char projection);
void ease2_grid_info(struct EASEgridType *grid, char grid_name[8]);
void ease2_inverseM(struct EASEgridType *grid);
void ease2_LL2RC(struct EASEgridType *grid, double lat, double lon, long *row, long *col);
void ease2_userdefined(void);

#endif
