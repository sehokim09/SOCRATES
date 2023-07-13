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

#include "42.h"
#include "socrates.h"
#include <ctype.h>

void calloc_double(double *var, size_t size)
{
	var = (double *)calloc(size, sizeof(double));
	if (var == NULL) {
		printf("calloc_double returned null pointer.  Bailing out!\n");
		exit(1);
	}
}

void cprintf(double complex z, char delimiter)
{
    printf("%le%+lei%c", creal(z), cimag(z), delimiter);
}

long decodeStr(char *s)
{
    unsigned long i;

    for(i=0;i<strlen(s);i++) s[i] = toupper(s[i]);

    if (!strcmp(s,"FALSE")) return FALSE;
    else if (!strcmp(s,"TRUE")) return TRUE;

    else if (!strcmp(s,"FIXED")) return FIXED;
    else if (!strcmp(s,"INORBIT")) return INORBIT;
    else if (!strcmp(s,"DESIGN")) return DESIGN;
    else if (!strcmp(s,"RETRIEVAL")) return RETRIEVAL;
    
    else if (!strcmp(s,"SPACE")) return SPACE;
    else if (!strcmp(s,"TOWER")) return TOWER;

    else if (!strcmp(s,"EASE2")) return EASE2;

    else if (!strcmp(s,"USER_DEFINED")) return USER_DEFINED;
    else if (!strcmp(s,"SCHATTEN")) return SCHATTEN;
    else if (!strcmp(s,"NOMINAL")) return NOMINAL_TIMING;
    else if (!strcmp(s,"EARLY")) return EARLY_TIMING;
    else if (!strcmp(s,"LATE")) return LATE_TIMING;
    else if (!strcmp(s,"MEAN_ERROR")) return MEAN_F10;
    else if (!strcmp(s,"PLUSTWOSIG")) return PLUSTWOSIG_F10;
    else if (!strcmp(s,"MINUSTWOSIG")) return MINUSTWOSIG_F10;

    else if (!strcmp(s,"IMPORT")) return ORB_IMPORT;
    else if (!strcmp(s,"42")) return ORB_42;
    else if (!strcmp(s,"42_DECAY")) return ORB_42_DECAY;

    else if (!strcmp(s,"SINGLE")) return SINGLE;
    else if (!strcmp(s,"MULTI")) return MULTI;

    else if (!strcmp(s,"USCRN")) return USCRN;
    else if (!strcmp(s,"OTHERS")) return OTHERS;
    
    else if (!strcmp(s,"EXPO")) return EXPO;
    else if (!strcmp(s,"POME")) return POME;

    else if (!strcmp(s,"POLY2ND")) return POLY2ND;
    else if (!strcmp(s,"POLY3RD")) return POLY3RD;
    else if (!strcmp(s,"DISCRETE_SLAB")) return DISCRETE_SLAB;
    else if (!strcmp(s,"LOGISTIC")) return LOGISTIC;

    else if (!strcmp(s,"IDEAL")) return IDEAL;
    else if (!strcmp(s,"GAUSSIAN")) return GAUSSIAN;
    
    else if (!strcmp(s,"RX_FSW")) return RX_FSW;

    else if (!strcmp(s,"WALKER")) return WALKER;
    else if (!strcmp(s,"HYBRID")) return HYBRID;

    else {
        printf(">> Bogus input %s in decodeStr (init.c:%d)\n",s,__LINE__);
        exit(1);
    }
}

void readLineParsor(FILE *infile, double *data, const char *delimiter)
{
    char *token;
    char line[3600];
    int i;

    fgets(line, sizeof line, infile);
    token = strtok(line, delimiter);
    i = 0;
    while(token!=NULL){	
        data[i++] = atof(token);
        token = strtok(NULL, delimiter);
    }
}

void readLineParsorStr(FILE *infile, char **data)
{
    char *token;
    char line[3600];
    char delimiter[3] = ", ";
    int i;

    fgets(line, sizeof line, infile);
    token = strtok(line, delimiter);
    i = 0;
    while(token!=NULL && strcmp(token,"!")){
        strcpy(data[i++], token);
        token = strtok(NULL, delimiter);
    }
}

long UniformRandomDiscrete(struct RandomProcessType *RP, int lb, int ub)
{
    return (long) floor(lb+(ub-lb+1)*UniformRandom(RP));
}

void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds); 
} 

double factorial(int n){
  int i;
  double fact=1;
  for(i=n;i>=1;i--){
    fact=fact*i;
  }
  return fact;
}

int msta1(double x,int mp)
{
    double a0,f0,f1,f;
    int i,n0,n1,nn;

    a0 = fabs(x);
    n0 = (int)(1.1*a0)+1;
    f0 = 0.5*log10(6.28*n0)-n0*log10(1.36*a0/n0)-mp;
    n1 = n0+5;
    f1 = 0.5*log10(6.28*n1)-n1*log10(1.36*a0/n1)-mp;
    for (i=0;i<20;i++) {
        nn = n1-(n1-n0)/(1.0-f0/f1);
        f = 0.5*log10(6.28*nn)-nn*log10(1.36*a0/nn)-mp;
        if (abs(nn-n1) < 1) break;
        n0 = n1;
        f0 = f1;
        n1 = nn;
        f1 = f;
    }
    return nn;
}
int msta2(double x,int n,int mp)
{
    double a0,ejn,hmp,f0,f1,f,obj;
    int i,n0,n1,nn;

    a0 = fabs(x);
    hmp = 0.5*mp;
    ejn = 0.5*log10(6.28*n)-n*log10(1.36*a0/n);
    if (ejn <= hmp) {
        obj = mp;
        n0 = (int)(1.1*a0);
        if (n0 < 1) n0 = 1;
    }
    else {
        obj = hmp+ejn;
        n0 = n;
    }
    f0 = 0.5*log10(6.28*n0)-n0*log10(1.36*a0/n0)-obj;
    n1 = n0+5;
    f1 = 0.5*log10(6.28*n1)-n1*log10(1.36*a0/n1)-obj;
    for (i=0;i<20;i++) {
        nn = n1-(n1-n0)/(1.0-f0/f1);
        f = 0.5*log10(6.28*nn)-nn*log10(1.36*a0/nn)-obj;
        if (abs(nn-n1) < 1) break;
        n0 = n1;
        f0 = f1;
        n1 = nn;
        f1 = f;
    }
    return nn+10;
}

int cbessjy01(double complex z, double complex *cj0, double complex *cj1, double complex *cy0,double complex *cy1,double complex *cj0p,
    double complex *cj1p,double complex *cy0p,double complex *cy1p)
{
    double complex z1,z2,cr,cp,cs,cp0,cq0,cp1,cq1,ct1,ct2,cu;
    double a0,w0,w1;
    int k,kz;

    static double a[] = {
        -7.03125e-2,
         0.112152099609375,
        -0.5725014209747314,
         6.074042001273483,
        -1.100171402692467e2,
         3.038090510922384e3,
        -1.188384262567832e5,
         6.252951493434797e6,
        -4.259392165047669e8,
         3.646840080706556e10,
        -3.833534661393944e12,
         4.854014686852901e14,
        -7.286857349377656e16,
         1.279721941975975e19};
    static double b[] = {
         7.32421875e-2,
        -0.2271080017089844,
         1.727727502584457,
        -2.438052969955606e1,
         5.513358961220206e2,
        -1.825775547429318e4,
         8.328593040162893e5,
        -5.006958953198893e7,
         3.836255180230433e9,
        -3.649010818849833e11,
         4.218971570284096e13,
        -5.827244631566907e15,
         9.476288099260110e17,
        -1.792162323051699e20};
    static double a1[] = {
         0.1171875,
        -0.1441955566406250,
         0.6765925884246826,
        -6.883914268109947,
         1.215978918765359e2,
        -3.302272294480852e3,
         1.276412726461746e5,
        -6.656367718817688e6,
         4.502786003050393e8,
        -3.833857520742790e10,
         4.011838599133198e12,
        -5.060568503314727e14,
         7.572616461117958e16,
        -1.326257285320556e19};
    static double b1[] = {
        -0.1025390625,
         0.2775764465332031,
        -1.993531733751297,
         2.724882731126854e1,
        -6.038440767050702e2,
         1.971837591223663e4,
        -8.902978767070678e5,
         5.310411010968522e7,
        -4.043620325107754e9,
         3.827011346598605e11,
        -4.406481417852278e13,
         6.065091351222699e15,
        -9.833883876590679e17,
         1.855045211579828e20};

    a0 = cabs(z);
    z2 = z*z;
    z1 = z;
    if (a0 == 0.0) {
        *cj0 = 1;
        *cj1 = 0;
        *cy0 = -1e308;
        *cy1 = -1e308;
        *cj0p = 0;
        *cj1p = 0.5;
        *cy0p = 1e308;
        *cy1p = 1e308;
        return 0;
    }
    if (creal(z) < 0.0) z1 = -z;
    if (a0 <= 12.0) {
        *cj0 = 1;
        cr = 1;
        for (k=1;k<=40;k++) {
            cr *= -0.25*z2/(double)(k*k);
            *cj0 += cr;
            if (cabs(cr) < cabs(*cj0)*EPS) break;
        }
        *cj1 = 1;
        cr = 1;
        for (k=1;k<=40;k++) {
            cr *= -0.25*z2/(k*(k+1.0));
            *cj1 += cr;
            if (cabs(cr) < cabs(*cj1)*EPS) break;
        }
        *cj1 *= 0.5*z1;
        w0 = 0.0;
        cr = 1;
        cs = 0;
        for (k=1;k<=40;k++) {
            w0 += 1.0/k;
            cr *= -0.25*z2/(double)(k*k);
            cp = cr*w0;
            cs += cp;
            if (cabs(cp) < cabs(cs)*EPS) break;
        }
        *cy0 = M_2_PI*((clog(0.5*z1)+EL)* (*cj0)-cs);
        w1 = 0.0;
        cr = 1;
        cs = 1;
        for (k=1;k<=40;k++) {
            w1 += 1.0/k;
            cr *= -0.25*z2/(k*(k+1.0));
            cp = cr*(2.0*w1+1.0/(k+1.0));
            cs += cp;
            if (cabs(cp) < cabs(cs)*EPS) break;
        }
        *cy1 = M_2_PI*((clog(0.5*z1)+EL)*(*cj1)-1.0/z1-0.25*z1*cs);
    }
    else {
        if (a0 >= 50.0) kz = 8;         // can be changed to 10
        else if (a0 >= 35.0) kz = 10;   //   "      "     "  12
        else kz = 12;                   //   "      "     "  14
        ct1 = z1 - M_PI_4;
        cp0 = 1;
        for (k=0;k<kz;k++) {
            cp0 += a[k]*cpow(z1,-2.0*k-2.0);
        }
        cq0 = -0.125/z1;
        for (k=0;k<kz;k++) {
            cq0 += b[k]*pow(z1,-2.0*k-3.0);
        }
        cu = csqrt(M_2_PI/z1);
        *cj0 = cu*(cp0*ccos(ct1)-cq0*csin(ct1));
        *cy0 = cu*(cp0*csin(ct1)+cq0*ccos(ct1));
        ct2 = z1 - 0.75*M_PI;
        cp1 = 1;
        for (k=0;k<kz;k++) {
            cp1 += a1[k]*cpow(z1,-2.0*k-2.0);
        }
        cq1 = 0.375/z1;
        for (k=0;k<kz;k++) {
            cq1 += b1[k]*cpow(z1,-2.0*k-3.0);
        }
        *cj1 = cu*(cp1*ccos(ct2)-cq1*csin(ct2));
        *cy1 = cu*(cp1*csin(ct2)+cq1*ccos(ct2));
    }
    if (creal(z) < 0.0) {
        if (cimag(z) < 0.0) {
            *cy0 -= 2.0*I1*(*cj0);
            *cy1 = -(*cy1-2.0*I1*(*cj1));
        }
        else if (cimag(z) > 0.0) {
            *cy0 += 2.0*I1*(*cj0);
            *cy1 = -(*cy1+2.0*I1*(*cj1));
        }
        *cj1 = -*cj1;
    }
    *cj0p = -*cj1;
    *cj1p = *cj0-*cj1/z;
    *cy0p = -*cy1;
    *cy1p = *cy0-*cy1/z;
    return 0;
}

int cbessjyna(int n, double complex z, int *nm, double complex *cj, double complex *cy, double complex *cjp, double complex *cyp)
{
    double complex cbj0,cbj1,cby0,cby1,cj0,cjk,cj1,cf,cf1,cf2;
    double complex cs,cg0,cg1,cyk,cyl1,cyl2,cylk,cp11,cp12,cp21,cp22;
    double complex ch0,ch1,ch2;
    double complex cbjp, cbyp;
    double a0,yak,ya1,ya0,wa;
    int m,k,lb,lb0;
    
    if (n < 0) return 1;
    a0 = cabs(z);
    *nm = n;
    if (a0 < 1.0e-100) {
        for (k=0;k<=n;k++) {
            cj[k] = 0;
            cy[k] = -1e308;
            cjp[k] = 0;
            cyp[k] = 1e308;
        }
        cj[0] = 1;
        if(n>0)
            cjp[1] = 0.5;
        return 0;
    }
    if(n<1){
        cbessjy01(z,&cj[0],&cbj1,&cy[0],&cby1,&cjp[0],&cbjp,&cyp[0],&cbyp);
        cbj0 = cj[0];
        cby0 = cy[0];
    }
    else{
        cbessjy01(z,&cj[0],&cj[1],&cy[0],&cy[1],&cjp[0],&cjp[1],&cyp[0],&cyp[1]);
        cbj0 = cj[0];
        cbj1 = cj[1];
        cby0 = cy[0];
        cby1 = cy[1];
    }
    if (n <= 1) return 0;
    if (n < (int)0.25*a0) {
        cj0 = cbj0;
        cj1 = cbj1;
        for (k=2;k<=n;k++) {
            cjk = 2.0*(k-1.0)*cj1/z-cj0;
            cj[k] = cjk;
            cj0 = cj1;
            cj1 = cjk;
        }
    }
    else {
        m = msta1(a0,200);
        if (m < n) *nm = m;
        else m = msta2(a0,n,15);
        cf2 = 0;
        cf1 = 1.0e-100;
        for (k=m;k>=0;k--) {
            cf = 2.0*(k+1.0)*cf1/z-cf2;
            if (k <=*nm) cj[k] = cf;
            cf2 = cf1;
            cf1 = cf;
        }
        if (cabs(cbj0) > cabs(cbj1)) cs = cbj0/cf;
        else cs = cbj1/cf2;
        for (k=0;k<=*nm;k++) {
            cj[k] *= cs;
        }
    }
    for (k=2;k<=*nm;k++) {
        cjp[k] = cj[k-1]-(double)k*cj[k]/z;
    }
    ya0 = cabs(cby0);
    lb = 0;
    cg0 = cby0;
    cg1 = cby1;
    for (k=2;k<=*nm;k++) {
        cyk = 2.0*(k-1.0)*cg1/z-cg0;
        yak = cabs(cyk);
        ya1 = cabs(cg0);
        if ((yak < ya0) && (yak < ya1)) lb = k;
        cy[k] = cyk;
        cg0 = cg1;
        cg1 = cyk;
    }
    lb0 = 0;
    if ((lb > 4) && (cimag(z) != 0.0)) {
        while (lb != lb0) {
            ch2 = 1;
            ch1 = 0;
            lb0 = lb;
            for (k=lb;k>=1;k--) {
                ch0 = 2.0*k*ch1/z-ch2;
                ch2 = ch1;
                ch1 = ch0;
            }
            cp12 = ch0;
            cp22 = ch2;
            ch2 = 0;
            ch1 = 1;
            for (k=lb;k>=1;k--) {
                ch0 = 2.0*k*ch1/z-ch2;
                ch2 = ch1;
                ch1 = ch0;
            }
            cp11 = ch0;
            cp21 = ch2;
            if (lb == *nm)
                cj[lb+1] = 2.0*lb*cj[lb]/z-cj[lb-1];
            if (cabs(cj[0]) > cabs(cj[1])) {
                cy[lb+1] = (cj[lb+1]*cby0-2.0*cp11/(M_PI*z))/cj[0];
                cy[lb] = (cj[lb]*cby0+2.0*cp12/(M_PI*z))/cj[0];
            }
            else {
                cy[lb+1] = (cj[lb+1]*cby1-2.0*cp21/(M_PI*z))/cj[1];
                cy[lb] = (cj[lb]*cby1+2.0*cp22/(M_PI*z))/cj[1];
            }
            cyl2 = cy[lb+1];
            cyl1 = cy[lb];
            for (k=lb-1;k>=0;k--) {
                cylk = 2.0*(k+1.0)*cyl1/z-cyl2;
                cy[k] = cylk;
                cyl2 = cyl1;
                cyl1 = cylk;
            }
            cyl1 = cy[lb];
            cyl2 = cy[lb+1];
            for (k=lb+1;k<n;k++) {
                cylk = 2.0*k*cyl2/z-cyl1;
                cy[k+1] = cylk;
                cyl1 = cyl2;
                cyl2 = cylk;
            }
            for (k=2;k<=*nm;k++) {
                wa = cabs(cy[k]);
                if (wa < cabs(cy[k-1])) lb = k;
            }
        }
    }
    for (k=2;k<=*nm;k++) {
        cyp[k] = cy[k-1]-(double)k*cy[k]/z;
    }
    return 0;
}

int cbessh(int n, double complex z, int *nm, double complex *ch, double complex *chp)
{
    int err, k, nm1;
    int n1 = n+1;
    int n2 = n+2;
    double complex cj[n2], cy[n2], cjp[n2], cyp[n2], ch1;

    err = cbessjyna(n1, z, &nm1, &cj[0], &cy[0], &cjp[0], &cyp[0]);
    *nm = nm1 - 1;
    for(k=0; k<=*nm; k++){
        ch[k] = cj[k] + I1 * cy[k];
    }
    ch1 = cj[nm1] + I1 * cy[nm1];

    for(k=1; k<=*nm; k++){
        chp[k-1] = (double)(k-1)*ch[k-1]/z - ch[k];
    }
    chp[*nm] = (double)(*nm)*ch[*nm]/z - ch1;
    return err;
}

/* unifpdf (from Matlab statistics toolbox)
  UNIFPDF Uniform (continuous) probability density function (pdf).
   Y = UNIFPDF(X,A,B) returns the continuous uniform pdf on the
   interval [A,B] at the values in X. By default A = 0 and B = 1.
   The size of Y is the common size of the input arguments. A scalar input
   functions as a constant matrix of the same size as the other inputs.

   Reference:
      [1]  M. Abramowitz and I. A. Stegun, "Handbook of Mathematical
      Functions", Government Printing Office, 1964, 26.1.34.
   Copyright 1993-2000 The MathWorks, Inc.
   $Revision: 2.9 $  $Date: 2000/05/26 18:53:54 $
*/
double unifpdf(double x, double a, double b)
{
    double y;
    if(x>=a && x<= b && a < b)
        y = 1 / (b - a);
    else
        y = NAN;
    return y;    
}

double trapzxy(double x1, double x2, double y1, double y2)
{
    return 0.5 * (y2 + y1) * (x2 - x1);
}

#define FUNC(x) ((*func)(x))
double trapzd(double (*func)(double), double a, double b, long n)
{
    double x,tnm,sum,del;
    static double s;
    long it,j;

    if (n == 1) {
            return (s=0.5*(b-a)*(FUNC(a)+FUNC(b)));
    } else {
            for (it=1,j=1;j<n-1;j++) it <<= 1;
            tnm=it;
            del=(b-a)/tnm;              /*This is the spacing of points to be
                                          added. */
            x=a+0.5*del;
            for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FUNC(x);
            s=0.5*(s+(b-a)*sum/tnm);  /*This replaces s by its refined value.*/
            return s;
    }
}
#undef FUNC

double qtrap(double (*func)(double), double a, double b, double eps, double imax)
{
    double trapzd(double (*func)(double), double a, double b, long n);
    long i;
    double s,olds;

    olds = -1.0e30;
    for (i=1;i<=imax;i++) {
        s=trapzd(func,a,b,i);
        if (i > 5)
            if (fabs(s-olds) < eps*fabs(olds) || (s == 0.0 && olds == 0.0))
                return s;
        olds=s;
    }
    printf(">> Too many steps in routine qtrap\n");
    return 0.0;
}

/* Returns the integral of the function func from a to b. The parameters eps can be set to the
desired fractional accuracy and imax so that 2 to the power imax-1 is the maximum allowed
number of steps. Integration is performed by Simpson's rule. W.H.Press et al, Numerical Recipes in C */
double qsimp(double (*func)(double), double a, double b, double eps, double imax)
{
    double trapzd(double (*func)(double), double a, double b, long n);
    long i;
    double s,st,ost=0.0,os=0.0;
    for (i=1; i<=imax;i++) {
        st = trapzd(func,a,b,i);
        s = (4.0*st - ost)/3.0;
        if (i > 5) // Avoid spurious early convergence.
            if (fabs(s-os) < eps*fabs(os) || (s == 0.0 && os == 0.0))
                return s;
        os = s;
        ost = st;
    }
    printf(">> Too many steps in routine qsimp\n");
    return 0.0;
}

double hinkley_func(double mean1, double mean2, double std1, double std2, double corr_coeff, double w, long moment)
{   
    double pdfw, meanw;
    double a, w_argument;

    a = sqrt( (w*w)/(std1*std1) - 2*corr_coeff*w/(std1*std2) + 1/(std2*std2) ); 
    w_argument = (mean2*w - mean1)/(std1*std2*a);
    pdfw = (mean2*(std1*std1*std1)*(std2*std2)*(a*a)-(mean2*w-mean1)*(std2*w-corr_coeff*std1))/
		    ( sqrt(TwoPi)*(std1*std1*std1*std1)*(std2*std2*std2)*(a*a*a)) * exp( -(w_argument*w_argument)/2);

    if(moment == 0) // unity check
    {
        return pdfw;
    }
    else if(moment == 1) // mean
    {
        return pdfw*w;
    }
    else if(moment == 2) // var
    {
        meanw = hinkley_func(mean1, mean2, std1, std2, corr_coeff, w, 1);
        return pdfw*(w-meanw)*(w-meanw);
    }
    else{
        printf(">> Error in hinkley_func\n");
        return 0;
    }
}

double qsimp_hinkley(double mean1, double mean2, double std1, double std2, double corr_coeff, double a, double b, double eps, double imax, long moment)
{
    // trapzd
    double x, tnm, sum, del;
    long it, j;

    // qsimp
    long i;
    double s, st, ost=0.0, os=0.0;

    for (i=1; i<=imax;i++) {
        // trapzd
        if (i == 1) {
            st=0.5*(b-a)*(hinkley_func(mean1, mean2, std1, std2, corr_coeff, a, moment)+hinkley_func(mean1, mean2, std1, std2, corr_coeff, b, moment));
        } else {
            for (it=1,j=1;j<i-1;j++)
                it <<= 1;
            tnm=it;
            del = (b-a) / tnm; // This is the spacing of points to be added
            x = a + 0.5*del;
            for (sum=0.0,j=1; j<=it; j++,x+=del)
                sum += hinkley_func(mean1, mean2, std1, std2, corr_coeff, x, moment);
            st = 0.5*(st+(b-a)*sum/tnm); // This replaces st by its refined value    
        }
        // qsimp
        s = (4.0*st - ost)/3.0;
        if (i > 5) // Avoid spurious early convergence.
            if (fabs(s-os) < eps*fabs(os) || (s == 0.0 && os == 0.0))
                return s;
        os = s;
        ost = st;
    }
    printf(">> Too many steps in routine qsimp_hinkley\n");
    return 0.0;
}

double* linspace(double a, double b, int n, double u[])
{
    double c;
    int i;
    
    /* make sure number of points and array are valid */
    if(n < 2 || u == 0)
        return (void*)0;
    
    /* step size */
    c = (b - a)/(n - 1);
    
    /* fill vector */
    for(i = 0; i < n - 1; ++i)
        u[i] = a + i*c;
    
    /* fix last entry to b */
    u[n - 1] = b;
    
    /* done */
    return u;
}

// Function implements the approximate PDF of the ratio of RVs:  W = X1/X2 from (5) in Hinkley (1970)
void hinkley_pdf(double mean1, double mean2, double std1, double std2, double corr_coeff, double hinkley[2])
{
	double unity_check, varw;
    double w0, wn;
    double eps = 1.0e-3;
    long imax = 20;

	// allocate memory
	//w = (double *)malloc(npts*sizeof(double));

	//w =  linspace( (mean1/mean2)-20*sqrt(std1*std2), (mean1/mean2)+20*sqrt(std1*std2), npts);
	//linspace( (mean1-40*sqrt(std1))/(mean2), (mean1+40*sqrt(std1))/(mean2), npts, w);

    w0 = (mean1-40*sqrt(std1))/(mean2);
    wn = (mean1+40*sqrt(std1))/(mean2);
    unity_check = qsimp_hinkley(mean1, mean2, std1, std2, corr_coeff, w0, wn, eps, imax, 0); 
    // meanw = qsimp_hinkley(mean1, mean2, std1, std2, corr_coeff, w0, wn, eps, imax, 1); 
    varw = qsimp_hinkley(mean1, mean2, std1, std2, corr_coeff, w0, wn, eps, imax, 2); 

    hinkley[0] = unity_check;
    hinkley[1] = varw;
}

// Function implements the approximate PDF of the ratio of RVs:  W = X1/X2 from (5) in Hinkley (1970)
void hinkley_pdf2(double mean1, double mean2, double std1, double std2, double corr_coeff, long npts, double hinkley[2])
{
    double w0, wn, w1, w2, step, area;
    double pdfw1, pdfw2, unity_check, meanw, varw;
    long ipts;

    w0 = (mean1-40*sqrt(std1))/(mean2);
    wn = (mean1+40*sqrt(std1))/(mean2);

    // step size
    step = (wn - w0)/(npts - 1);
    
    for(unity_check=0.0, meanw=0.0, ipts=1; ipts<npts; ipts++){
        w1 = w0 + (ipts-1)*step;
        w2 = w0 + ipts*step;
        // fix last entry to wn
        if(ipts == npts-1 )
            w2 = wn;

        pdfw1 = hinkley_func(mean1, mean2, std1, std2, corr_coeff, w1, 0);
        pdfw2 = hinkley_func(mean1, mean2, std1, std2, corr_coeff, w2, 0);

        area = trapzxy(w1, w2, pdfw1, pdfw2);
        unity_check += area;

        area = trapzxy(w1, w2, w1*pdfw1, w2*pdfw2);
        meanw += area;
    }
    for(varw=0.0, ipts=1; ipts<npts; ipts++){
        w1 = w0 + (ipts-1)*step;
        w2 = w0 + ipts*step;
        // fix last entry to wn
        if(ipts == npts-1)
            w2 = wn;

        pdfw1 = hinkley_func(mean1, mean2, std1, std2, corr_coeff, w1, 0);
        pdfw2 = hinkley_func(mean1, mean2, std1, std2, corr_coeff, w2, 0);

        area = trapzxy(w1, w2, (w1-meanw)*(w1-meanw)*pdfw1, (w2-meanw)*(w2-meanw)*pdfw2);
        varw += area;
    }
    hinkley[0] = unity_check;
    hinkley[1] = varw;
}

double rand01(void)
{
   return rand()/(double)RAND_MAX;
}

double randxy(double x, double y)
{
   return (y - x)*rand01() + x;
}

double AWGN_generator()
{/* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */
  double temp1;
  double temp2;
  double result;
  int p;

  p = 1;

  while( p > 0 )
  {
	temp2 = ( rand() / ( (double)RAND_MAX ) ); /*  rand() function generates an
                                                       integer between 0 and  RAND_MAX,
                                                       which is defined in stdlib.h.
                                                   */

    if ( temp2 == 0 )
    {// temp2 is >= (RAND_MAX / 2)
      p = 1;
    }// end if
    else
    {// temp2 is < (RAND_MAX / 2)
       p = -1;
    }// end else

  }// end while()

  temp1 = cos( ( 2.0 * (double)Pi ) * rand() / ( (double)RAND_MAX ) );
  result = sqrt( -2.0 * log( temp2 ) ) * temp1;

  return result;	// return the generated random sample to the caller

}

double var(double *x, long dim)
{
    double var, sum = 0, avg;
    long i;
    
    for(i=0; i<dim; i++){
        sum += x[i];
    }
    avg = sum / (double) dim;
    sum = 0;
    for(i=0; i<dim; i++){
        sum += (x[i] - avg) * (x[i] - avg);
    }
    
    return var = sum / (double) dim;
}

double roundzero(double x)
{
	return (fabs(x) < EPS ? 0 : x);
}

double newatan2(double y, double x)
{
    if(fabs(x) == 0 && fabs(y) == 0){
        if(signbit(x) != POS_ZERO && signbit(y) == POS_ZERO)
            return (atan2(y,x) - Pi);
        else if(signbit(x) != POS_ZERO && signbit(y) != POS_ZERO)
            return (atan2(y,x) + Pi);
        else
            return atan2(y,x);
    }
    else
        return atan2(y,x);
}

double atan2TwoPi(double y, double x)
{
    double res; 
    res = atan2(y,x);
    if(res<0)
        res += TwoPi;
    return res;
}

/* Return evanescent SQRT for waves problems */
double complex sqrte(double complex z)
{
	double complex y;

	if(cimag(z)==0 && creal(z)<0)
		y = -I1 * csqrt(abs(z));
	else
		y = csqrt(z);

	return y;
}

/* Return X to the Y power.  */
double newpow(double x, double y)
{
    int sign=1;
    double r;
    if (x<0){
        sign = (-1);
        x *= sign;
    }
    r = pow(x, y);
    return (r*sign);
}

double Limit_max(double x, double max)
{
      return(x > max ? max : x);
}

/* Conversion from Spherical to Cartesian coordinate system */
void sph2car(double theta, double phi, double AR, double AT, double AP, double axis[3])
{
	double st, ct, sp, cp;

	st = sin(theta);
	ct = cos(theta);
	sp = sin(phi);
	cp = cos(phi);

	axis[0] = AR*st*cp + AT*ct*cp - AP*sp;
	axis[1] = AR*st*sp + AT*ct*sp + AP*cp;
	axis[2] = AR*ct - AT*st;
}

void copyV3(double dest[3], double sour[3])
{
    dest[0] = sour[0];
    dest[1] = sour[1];
    dest[2] = sour[2];
}

/* Complex Vector Dot Product */
double complex VoV_complex(double complex A[3], double complex B[3])
{
    return(conj(A[0])*conj(B[0])+conj(A[1])*conj(B[1])+conj(A[2])*conj(B[2]));
//      return(A[0]*conj(B[0])+A[1]*conj(B[1])+A[2]*conj(B[2]));
}

/*  4x4 Matrix times 4x1 Vector */
void M4xV4_complex(double complex M[4][4], double complex V[4], double complex W[4])
{
      W[0] = V[0]*M[0][0] + V[1]*M[0][1] + V[2]*M[0][2] + V[3]*M[0][3];
      W[1] = V[0]*M[1][0] + V[1]*M[1][1] + V[2]*M[1][2] + V[3]*M[1][3];
      W[2] = V[0]*M[2][0] + V[1]*M[2][1] + V[2]*M[2][2] + V[3]*M[2][3];
      W[3] = V[0]*M[3][0] + V[1]*M[3][1] + V[2]*M[3][2] + V[3]*M[3][3];
}

/*  2x2 Matrix times 2x1 Vector */
void M2xV2_complex(double complex M[2][2], double complex V[2], double complex W[2])
{
      W[0] = V[0]*M[0][0] + V[1]*M[0][1];
      W[1] = V[0]*M[1][0] + V[1]*M[1][1];
}

/*  Transpose of 3x3 Matrix times 3x1 Complex Vector */
void MTxV_complex (double M[3][3], double V[3], double complex W[3])
{
      W[0] = M[0][0]*V[0] + M[1][0]*V[1] + M[2][0]*V[2];
      W[1] = M[0][1]*V[0] + M[1][1]*V[1] + M[2][1]*V[2];
      W[2] = M[0][2]*V[0] + M[1][2]*V[1] + M[2][2]*V[2];
}

/*  4x4 Complex Matrix Product */
void M4xM4_complex(double complex A[4][4], double complex B[4][4], double complex C[4][4])
{
      C[0][0]=A[0][0]*B[0][0]+A[0][1]*B[1][0]+A[0][2]*B[2][0]+A[0][3]*B[3][0];
      C[0][1]=A[0][0]*B[0][1]+A[0][1]*B[1][1]+A[0][2]*B[2][1]+A[0][3]*B[3][1];
      C[0][2]=A[0][0]*B[0][2]+A[0][1]*B[1][2]+A[0][2]*B[2][2]+A[0][3]*B[3][2];
      C[0][3]=A[0][0]*B[0][3]+A[0][1]*B[1][3]+A[0][2]*B[2][3]+A[0][3]*B[3][3];
      C[1][0]=A[1][0]*B[0][0]+A[1][1]*B[1][0]+A[1][2]*B[2][0]+A[1][3]*B[3][0];
      C[1][1]=A[1][0]*B[0][1]+A[1][1]*B[1][1]+A[1][2]*B[2][1]+A[1][3]*B[3][1];
      C[1][2]=A[1][0]*B[0][2]+A[1][1]*B[1][2]+A[1][2]*B[2][2]+A[1][3]*B[3][2];
	  C[1][3]=A[1][0]*B[0][3]+A[1][1]*B[1][3]+A[1][2]*B[2][3]+A[1][3]*B[3][3];
      C[2][0]=A[2][0]*B[0][0]+A[2][1]*B[1][0]+A[2][2]*B[2][0]+A[2][3]*B[3][0];
      C[2][1]=A[2][0]*B[0][1]+A[2][1]*B[1][1]+A[2][2]*B[2][1]+A[2][3]*B[3][1];
      C[2][2]=A[2][0]*B[0][2]+A[2][1]*B[1][2]+A[2][2]*B[2][2]+A[2][3]*B[3][2];
      C[2][3]=A[2][0]*B[0][3]+A[2][1]*B[1][3]+A[2][2]*B[2][3]+A[2][3]*B[3][3];
      C[3][0]=A[3][0]*B[0][0]+A[3][1]*B[1][0]+A[3][2]*B[2][0]+A[3][3]*B[3][0];
      C[3][1]=A[3][0]*B[0][1]+A[3][1]*B[1][1]+A[3][2]*B[2][1]+A[3][3]*B[3][1];
      C[3][2]=A[3][0]*B[0][2]+A[3][1]*B[1][2]+A[3][2]*B[2][2]+A[3][3]*B[3][2];
      C[3][3]=A[3][0]*B[0][3]+A[3][1]*B[1][3]+A[3][2]*B[2][3]+A[3][3]*B[3][3];
}

/*  2x2 Complex Matrix Product */
void M2xM2_complex(double complex A[2][2], double complex B[2][2], double complex C[2][2])
{
      C[0][0]=A[0][0]*B[0][0]+A[0][1]*B[1][0];
      C[0][1]=A[0][0]*B[0][1]+A[0][1]*B[1][1];
      C[1][0]=A[1][0]*B[0][0]+A[1][1]*B[1][0];
      C[1][1]=A[1][0]*B[0][1]+A[1][1]*B[1][1];
}

void CopyMT3(double A[3][3], double B[3][3])
{
    B[0][0] = A[0][0]; B[0][1] = A[1][0]; B[0][2] = A[2][0];
    B[1][0] = A[0][1]; B[1][1] = A[1][1]; B[1][2] = A[2][1];
    B[2][0] = A[0][2]; B[2][1] = A[1][2]; B[2][2] = A[2][2];
}

/* Inverse of a 2x2 Complex Matrix */
void MINV2_complex(double complex A[2][2], double complex B[2][2])
{
      double complex DET;
      
      DET = A[0][0]*A[1][1] - A[1][0]*A[0][1];
      
      if (DET == 0.0) {
         printf("Attempted inversion of singular matrix in MINV2.  Bailing out.\n");
         exit(1);
      }
      else {
         B[0][0]= A[1][1]/DET;
         B[0][1]=-A[0][1]/DET;
         B[1][0]=-A[1][0]/DET;
         B[1][1]= A[0][0]/DET;
      }
}

void RotX(double th, double R[3][3])
{
	R[0][0] = 1;
	R[0][1] = 0;
	R[0][2] = 0;
	R[1][0] = 0;
	R[1][1] = cos(th);
	R[1][2] = -sin(th);
	R[2][0] = 0;
	R[2][1] = sin(th);
	R[2][2] = cos(th);
}

void RotY(double th, double R[3][3])
{
	R[0][0] = cos(th);
	R[0][1] = 0;
	R[0][2] = sin(th);
	R[1][0] = 0;
	R[1][1] = 1;
	R[1][2] = 0;
	R[2][0] = -sin(th);
	R[2][1] = 0;
	R[2][2] = cos(th);
}

void RotZ(double th, double R[3][3])
{
	R[0][0] = cos(th);
	R[0][1] = -sin(th);
	R[0][2] = 0;
	R[1][0] = sin(th);
	R[1][1] = cos(th);
	R[1][2] = 0;
	R[2][0] = 0;
	R[2][1] = 0;
	R[2][2] = 1;
}

void newFindCLN(double r[3], double v[3], double CLN[3][3])
{
      double L1[3],L2[3],L3[3],m,h[3],rr,hh;
      long i;

      h[0] = r[1]*v[2]-r[2]*v[1];
      h[1] = r[2]*v[0]-r[0]*v[2];
      h[2] = r[0]*v[1]-r[1]*v[0];
      rr = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];
      hh = h[0]*h[0]+h[1]*h[1]+h[2]*h[2];

      for(i=0;i<3;i++) {
         L3[i] = -r[i];
         L2[i] = -h[i];
      }

      m=sqrt(rr);
      L3[0] /= m;
      L3[1] /= m;
      L3[2] /= m;

      if (hh == 0.0) { /* Rectlinear Motion */
         PerpBasis(L3,L1,L2);
      }
      else {
         m=sqrt(L2[0]*L2[0]+L2[1]*L2[1]+L2[2]*L2[2]);
         L2[0] /= m;
         L2[1] /= m;
         L2[2] /= m;

         L1[0] = L2[1]*L3[2]-L2[2]*L3[1];
         L1[1] = L2[2]*L3[0]-L2[0]*L3[2];
         L1[2] = L2[0]*L3[1]-L2[1]*L3[0];

         m=sqrt(L1[0]*L1[0]+L1[1]*L1[1]+L1[2]*L1[2]);
         L1[0] /= m;
         L1[1] /= m;
         L1[2] /= m;
      }

      for(i=0;i<3;i++){
         CLN[0][i] = L1[i];
         CLN[1][i] = L2[i];
         CLN[2][i] = L3[i];
      }
}

/*
 Function that performs Gauss-Elimination and returns the Upper triangular matrix and solution of equations:
There are two options to do this in C.
1. Pass the augmented matrix (a) as the parameter, and calculate and store the upperTriangular(Gauss-Eliminated Matrix) in it.
2. Use malloc and make the function of pointer type and return the pointer.
This program uses the first option.
*/
void gaussEliminationLS(int m, int n, double a[m][n], double x[n-1])
{
    int i,j,k;
    for(i=0;i<m-1;i++){
        //Partial Pivoting
        for(k=i+1;k<m;k++){
            //If diagonal element(absolute vallue) is smaller than any of the terms below it
            if(fabs(a[i][i])<fabs(a[k][i])){
                //Swap the rows
                for(j=0;j<n;j++){                
                    double temp;
                    temp=a[i][j];
                    a[i][j]=a[k][j];
                    a[k][j]=temp;
                }
            }
        }
        //Begin Gauss Elimination
        for(k=i+1;k<m;k++){
            double  term=a[k][i]/ a[i][i];
            for(j=0;j<n;j++){
                a[k][j]=a[k][j]-term*a[i][j];
            }
        }
         
    }
    //Begin Back-substitution
    for(i=m-1;i>=0;i--){
        x[i]=a[i][n-1];
        for(j=i+1;j<n-1;j++){
            x[i]=x[i]-a[i][j]*x[j];
        }
        x[i]=x[i]/a[i][i];
    }             
}

void GRS80ToECEF(double glat, double glong, double alt, double p[3])
{
      double a = 6378137.0;
      double f = 1.0/298.257222100882711;
      double e2 = f*(2.0-f);
      double X;
      double CosLat,SinLat,CosLng,SinLng;

      CosLat = cos(glat);
      SinLat = sin(glat);
      CosLng = cos(glong);
      SinLng = sin(glong);

      X = sqrt(1.0-e2*SinLat*SinLat);

      p[0] = (a/X+alt)*CosLat*CosLng;
      p[1] = (a/X+alt)*CosLat*SinLng;
      p[2] = (a/X*(1.0-e2)+alt)*SinLat;
}
/**********************************************************************/
void ECEFToGRS80(double p[3], double *glat, double *glong, double *alt)
{
      double a = 6378137.0;
      double f = 1.0/298.257222100882711;
      double b = a*(1.0-f);
      double e2 = f*(2.0-f);
      double ep2 = f*(2.0-f)/(1.0-f)/(1.0-f);
      double r,E2,F,G,C,S,P,Q,r0,U,V,Z0;

      double OneMinusE2,Z1,SpolyG,Qpoly;

      OneMinusE2 = 1.0-e2;

      r = sqrt(p[0]*p[0]+p[1]*p[1]);

      E2 = a*a-b*b;

      Z1 = b*p[2];

      F = 54.0*Z1*Z1;

      G = r*r+OneMinusE2*p[2]*p[2]-e2*E2;

      Z1 = e2*r/G;
      C = Z1*Z1*F/G;

      S = pow(1.0+C+sqrt(C*C+2.0*C),1.0/3.0);

      SpolyG = (S+1.0/S+1.0)*G;
      P = F/(3.0*SpolyG*SpolyG);

      Q = sqrt(1.0+2.0*e2*e2*P);

      Qpoly = 1.0+Q;
      r0 = -P*e2*r/Qpoly+sqrt(0.5*a*a*Qpoly/Q-P*OneMinusE2*p[2]*p[2]/(Q*Qpoly)-0.5*P*r*r);

      Z1 = r-e2*r0;
      Z1 *= Z1;

      U = sqrt(Z1+p[2]*p[2]);

      V = sqrt(Z1+OneMinusE2*p[2]*p[2]);


      Z1 = b*b/a/V;
      Z0 = Z1*p[2];

      *alt = U*(1.0-Z1);
      *glat = atan((p[2]+ep2*Z0)/r);
      *glong = atan2(p[1],p[0]);
}

/**********************************************************************/
/*  Convert Year, Month, Day, Hour, Minute and Second to Julian Day   */
/*  Valid for any positive JD.                                        */
/*  Year, Month, Day assumed in Gregorian calendar. (Not true < 1582) */
/*  Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.  */

double YMDHMS2JD(long Year, long Month, long Day, 
               long Hour, long Minute, double Second)
{
      long A,B;
      double JD;

      if (Month < 3) {
         Year--;
         Month+=12;
      }

      A = Year/100;
      B = 2 - A + A/4;

      JD = floor(365.25*(Year+4716)) + floor(30.6001*(Month+1))
           + Day + B - 1524.5;

      JD += ((double) Hour) / 24.0 +
            ((double) Minute) / 1440.0 +
            Second / 86400.0;

      return(JD);
}

/**********************************************************************/
/*   Convert Julian Day to Year, Month, Day, Hour, Minute, and Second */
/*   Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991. */

void JD2YMDHMS(double JD,long *Year, long *Month, long *Day,
                         long *Hour, long *Minute, double *Second)
{
      double Z,F,A,B,C,D,E,alpha;
      double FD;

      Z= floor(JD+0.5);
      F=(JD+0.5)-Z;

      if (Z < 2299161.0) {
         A = Z;
      }
      else {
         alpha = floor((Z-1867216.25)/36524.25);
         A = Z+1.0+alpha - floor(alpha/4.0);
      }

      B = A + 1524.0;
      C = floor((B-122.1)/365.25);
      D = floor(365.25*C);
      E = floor((B-D)/30.6001);

      FD = B - D - floor(30.6001*E) + F;
      *Day = (long) FD;

      if (E < 14.0) {
         *Month = (long) (E - 1.0);
         *Year = (long) (C - 4716.0);
      }
      else {
         *Month = (long) (E - 13.0);
         *Year = (long) (C - 4715.0);
      }

      FD = FD - floor(FD);
      FD = FD * 24.0;
      *Hour = (long) FD;
      FD = FD - floor(FD);
      FD = FD * 60.0;
      *Minute = (long) FD;
      FD = FD - floor(FD);
      *Second = FD * 60.0;

}

void ease2_map_info(struct MapType *map, char projection)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    map->equatorial_radius_m = 6378137.0; // WGS84
    map->eccentricity = 0.081819190843; // WGS84
    map->e2 = map->eccentricity * map->eccentricity;
    map->e4 = map->e2 * map->e2;
    map->e6 = map->e2 * map->e4;
    if(projection == 'N'){
        map->reference_latitude = HalfPi;
        map->reference_longitude = 0.0;
    }
    else if(projection == 'S'){
        map->reference_latitude = -HalfPi;
        map->reference_longitude = 0.0;
    }
    else if(projection == 'M'){
        map->reference_latitude = 0.0;
        map->reference_longitude = 0.0;
        map->second_reference_latitude = 30.0*D2R;
        map->sin_phi1 = sin(map->second_reference_latitude);
        map->cos_phi1 = cos(map->second_reference_latitude);
        map->kz = map->cos_phi1 / sqrt(1.0 - map->e2 * map->sin_phi1 * map->sin_phi1);
    }
    else{
        printf("Error. Wrong projection name.\n");
        exit(1);
    }
}

void ease2_grid_info(struct EASEgridType *grid, char grid_name[8])
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022

    struct EASEgridType grid_info[22];

    strcpy(grid_info[0].gpd, "N25.00");   grid_info[0].map_scale_m = 25000.0;
    grid_info[0].nCol = 720;            grid_info[0].nRow  = 720;
    
    strcpy(grid_info[1].gpd, "N12.50"); grid_info[1].map_scale_m = 12500.0;
    grid_info[1].nCol = 1440;           grid_info[1].nRow  = 1440;
    
    strcpy(grid_info[2].gpd, "N6.25"); grid_info[2].map_scale_m = 6250.0;
    grid_info[2].nCol = 2880;           grid_info[2].nRow  = 2880;
    
    strcpy(grid_info[3].gpd, "N36.00");   grid_info[3].map_scale_m = 36000.0;
    grid_info[3].nCol = 500;            grid_info[3].nRow  = 500;
    
    strcpy(grid_info[4].gpd, "N9.00");   grid_info[4].map_scale_m = 9000.0;
    grid_info[4].nCol = 2000;           grid_info[4].nRow  = 2000;
    
    strcpy(grid_info[5].gpd, "N3.00");   grid_info[5].map_scale_m = 3000.0;
    grid_info[5].nCol = 6000;           grid_info[5].nRow  = 6000;
    
    strcpy(grid_info[6].gpd, "N1.00");   grid_info[6].map_scale_m = 1000.0;
    grid_info[6].nCol = 18000;          grid_info[6].nRow  = 18000;
    
    strcpy(grid_info[7].gpd, "N100.00");  grid_info[7].map_scale_m = 100000.0;
    grid_info[7].nCol = 180;            grid_info[7].nRow  = 180;
    
    strcpy(grid_info[8].gpd, "S25.00");   grid_info[8].map_scale_m = 25000.0;
    grid_info[8].nCol = 720;            grid_info[8].nRow  = 720;
    
    strcpy(grid_info[9].gpd, "S12.50"); grid_info[9].map_scale_m = 12500.0;
    grid_info[9].nCol = 1440;           grid_info[9].nRow  = 1440;
    
    strcpy(grid_info[10].gpd, "S6.25"); grid_info[10].map_scale_m = 6250.0;
    grid_info[10].nCol = 2880;           grid_info[10].nRow  = 2880;
    
    strcpy(grid_info[11].gpd, "S36.00");   grid_info[11].map_scale_m = 36000.0;
    grid_info[11].nCol = 500;            grid_info[11].nRow  = 500;
    
    strcpy(grid_info[12].gpd, "S9.00");   grid_info[12].map_scale_m = 9000.0;
    grid_info[12].nCol = 2000;           grid_info[12].nRow  = 2000;
    
    strcpy(grid_info[13].gpd, "S3.00");   grid_info[13].map_scale_m = 3000.0;
    grid_info[13].nCol = 6000;           grid_info[13].nRow  = 6000;
    
    strcpy(grid_info[14].gpd, "S1.00");   grid_info[14].map_scale_m = 1000.0;
    grid_info[14].nCol = 18000;          grid_info[14].nRow  = 18000;
    
    strcpy(grid_info[15].gpd, "M25.00");   grid_info[15].map_scale_m = 25025.2600081;
    grid_info[15].nCol = 1388;           grid_info[15].nRow  = 584;
    
    strcpy(grid_info[16].gpd, "M12.50"); grid_info[16].map_scale_m = 12512.63000405;
    grid_info[16].nCol = 2776;           grid_info[16].nRow  = 1168;
    
    strcpy(grid_info[17].gpd, "M6.25"); grid_info[17].map_scale_m = 6256.315002025;
    grid_info[17].nCol = 5552;           grid_info[17].nRow  = 2336;
    
    strcpy(grid_info[18].gpd, "M36.00");   grid_info[18].map_scale_m = 36032.220840584;
    grid_info[18].nCol = 964;            grid_info[18].nRow  = 406;
    
    strcpy(grid_info[19].gpd, "M9.00");   grid_info[19].map_scale_m = 9008.055210146;
    grid_info[19].nCol = 3856;           grid_info[19].nRow  = 1624;
    
    strcpy(grid_info[20].gpd, "M3.00");   grid_info[20].map_scale_m = 3002.6850700487;
    grid_info[20].nCol = 11568;          grid_info[20].nRow  = 4872;
    
    strcpy(grid_info[21].gpd, "M1.00");   grid_info[21].map_scale_m = 1000.89502334956;
    grid_info[21].nCol = 34704;          grid_info[21].nRow  = 14616;
    
    long idx = -1;
    int i;

    for(i=0; i<22; i++){
        grid_info[i].s0 = ( (double)grid_info[i].nRow  - 1.0 ) / 2.0;
        grid_info[i].r0 = ( (double)grid_info[i].nCol - 1.0 ) / 2.0;
        grid_info[i].projection = grid_info[i].gpd[0];
        if(!strcmp(grid_name, grid_info[i].gpd)){
            idx = i;
            break;
        }
    }
    
    if(idx == -1){
        printf("Error. Unknown EASE2 name.\n");
        exit(1);
    }

    grid->projection = grid_info[idx].projection;
    grid->map_scale_m = grid_info[idx].map_scale_m;
    grid->nRow = grid_info[idx].nRow;
    grid->nCol = grid_info[idx].nCol;
    grid->s0 = grid_info[idx].s0;
    grid->r0 = grid_info[idx].r0;
}

double easeconv_normalize_radians(double inlon)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    double lon;

    if(inlon < -Pi)
        lon = inlon + TwoPi;
    else if(inlon > Pi)
        lon = inlon - TwoPi;
    else
        lon = inlon;

    return lon;
}

void wgs84_convert_xy(char projection, double lat, double lon, double *x, double *y)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    double epsilon, qp, rho, q, dlon, sin_phi;
    
    epsilon = 1.0E-6; // epsilon for test in neighborhood of pole for azimuthal projections

    //ease2_map_info(Map, projection);

    dlon = lon - Map->reference_longitude;    
    dlon = easeconv_normalize_radians( dlon );
    
    sin_phi = sin(lat);
      
    q = (1.0 - Map->e2) * ((sin_phi/(1.0 - Map->e2*sin_phi*sin_phi)) - (1.0/(2.0*Map->eccentricity))*log((1.0 - Map->eccentricity*sin_phi)/(1.0 + Map->eccentricity*sin_phi)));   
    qp = 1.0 - ((1.0 - Map->e2)/(2.0*Map->eccentricity)*log((1.0 - Map->eccentricity)/(1.0 + Map->eccentricity)));
    if(projection == 'N'){
        if (fabs(qp - q) < epsilon)
            rho = 0.0;
        else
            rho = Map->equatorial_radius_m * sqrt(qp - q);

        *x = rho * sin(dlon);
        *y = -1 * rho * cos(dlon);
    }
    else if(projection == 'S'){
        if (fabs(qp + q) < epsilon)
            rho = 0.0;
        else
            rho = Map->equatorial_radius_m * sqrt(qp + q);

        *x = rho * sin(dlon);
        *y = rho * cos(dlon);
    }
    else if(projection == 'M'){
        *x = Map->equatorial_radius_m * Map->kz * dlon;
        *y = (Map->equatorial_radius_m*q) / (2.0*Map->kz);
    }
    else{
        printf("Error. Wrong projection.\n");
        exit(1);
    }
}

void wgs84_convert(struct EASEgridType *grid, double lat, double lon, double *s, double *r)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    double x, y;

    //ease2_grid_info(grid_name);
    
    wgs84_convert_xy(grid->projection, lat, lon, &x, &y);
    
    // Convert xy coordinates to row/col for this particular grid
    *s = grid->s0 - ( y / grid->map_scale_m );
    *r = grid->r0 + ( x / grid->map_scale_m );
    
    // Check for actual grid boundaries
    if ((*r< -0.5) || (*r >= (grid->nCol - 0.5)) || (*s < -0.5) || (*s >= (grid->nRow - 0.5))){
        printf("Error. Point not on grid.\n");
        exit(1);
    }
}

void wgs84_inverse_xy(char projection, double x, double y, double *lat, double *lon)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    double qp, rho, beta, lam;

    //ease2_map_info(Map, projection);

    // qp is the function q evaluated at phi = 90.0
    qp = (1.0 - Map->e2) * ((1.0/(1.0 - Map->e2)) - (1.0/(2.0*Map->eccentricity))*log((1.0 - Map->eccentricity)/(1.0 + Map->eccentricity)));
    
    if(projection == 'N'){
        rho = sqrt( ( x * x ) + ( y * y ) );
        beta = asin( 1.0 - ( rho*rho / ( Map->equatorial_radius_m*Map->equatorial_radius_m * qp ) ) );
        lam = atan2( x, ( -1.0 * y ) );
    }
    else if(projection == 'S'){
        rho = sqrt( ( x * x ) + ( y * y ) );
        beta = -1 * asin( 1.0 - ( rho*rho / ( Map->equatorial_radius_m*Map->equatorial_radius_m * qp ) ) );
        lam = atan2( x,  y );
    }
    else if(projection == 'M'){
        beta = asin( 2.0 * y * Map->kz/( Map->equatorial_radius_m * qp ) );
        lam = x / ( Map->equatorial_radius_m * Map->kz );
    }
    else{
        printf("Error. Wrong projection.\n");
        exit(1);
    }
    
    *lat = beta + (((Map->e2/3.0) + ((31.0/180.0)*Map->e4) + ((517.0/5040.0)*Map->e6)) * sin(2*beta)) + ((((23.0/360.0)*Map->e4) + ((251.0/3780.0)*Map->e6))*sin(4*beta)) + (((761.0/45360.0)*Map->e6)*sin(6.0*beta));
    *lon = easeconv_normalize_radians( Map->reference_longitude + lam );
}

void wgs84_inverse(struct EASEgridType *grid, double s, double r, double *lat, double *lon)
{
//   Source and credit: https://github.com/nsidc/easegrids-easeconv
//   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    double x, y;

    //ease2_grid_info(grid_name);
      
    // Convert (row, col) to xy coordinates for this projection
    y = ( grid->s0 - s ) * grid->map_scale_m;
    x = ( r - grid->r0 ) * grid->map_scale_m;
    
    wgs84_inverse_xy(grid->projection, x, y, lat, lon);
}

// Convert geographic coordinates (lat,lon) to azimuthal equal-area or cylindirical equal-area grid (row,col) coordinates
void ease2_LL2RC(struct EASEgridType *grid, double lat, double lon, long *row, long *col)
{
    double x, y;
    
    wgs84_convert_xy(grid->projection, lat, lon, &x, &y);
    
    // Convert xy coordinates to row/col for this particular grid
    *row = (long) round(grid->s0 - ( y / grid->map_scale_m )); // row
    *col = (long) round(grid->r0 + ( x / grid->map_scale_m )); // col
}

// Compute latitude and longtidue coordinates of EASE2.0 grid
void ease2_inverseM(struct EASEgridType *grid)
{
    long iRow, iCol, dummy2;
    double dummy1;
    
    for(iRow=0; iRow<grid->nRow; iRow++){
        wgs84_inverse(grid, iRow, 0, &grid->lat[iRow], &dummy1);
        ease2_LL2RC(grid, grid->lat[iRow], dummy1, &grid->row[iRow], &dummy2);
    }

    for(iCol=0; iCol<grid->nCol; iCol++){
        wgs84_inverse(grid, 0, iCol, &dummy1, &grid->lon[iCol]);
        ease2_LL2RC(grid, dummy1, grid->lon[iCol], &dummy2, &grid->col[iCol]);
    }
}

void ease2_userdefined_LL2RC(struct EASEgridType *G, double lat, double lon, long *row, long *col)
{
	double q, x, y;
	double sin_phi, dlon;

    dlon = lon - Map->reference_longitude;    
    dlon = easeconv_normalize_radians( dlon );

    sin_phi = sin(lat);
    
    q = (1.0 - Map->e2) * ((sin_phi/(1.0 - Map->e2*sin_phi*sin_phi)) - (1.0/(2.0*Map->eccentricity))*log((1.0 - Map->eccentricity*sin_phi)/(1.0 + Map->eccentricity*sin_phi)));
    x = Map->equatorial_radius_m * Map->kz * dlon;
    y = (Map->equatorial_radius_m*q) / (2.0*Map->kz);

	*col = round((x - G->x0) / G->map_scale_m);
	*row = round((G->y0 - y) / G->map_scale_m);
}

void ease2_userdefined(void)
{
    int iGrid, iRow, iCol;
    struct EASEgridType *G;
    double q, qp, beta, dlon, sin_phi;
    double x, y;
    long dummy;

	for(iGrid=0; iGrid<NGrid; iGrid++){
		G = &Grid[iGrid];

        dlon = G->lon0 - Map->reference_longitude;    
        dlon = easeconv_normalize_radians( dlon );

        sin_phi = sin(G->lat0);
		// Global Cylindrical EASE-Grid 2.0 Inverse Formulae;
        q = (1.0 - Map->e2) * ((sin_phi/(1.0 - Map->e2*sin_phi*sin_phi)) - (1.0/(2.0*Map->eccentricity))*log((1.0 - Map->eccentricity*sin_phi)/(1.0 + Map->eccentricity*sin_phi)));   
        // qp is the function q evaluated at phi = 90.0
        qp = (1.0 - Map->e2) * ((1.0/(1.0 - Map->e2)) - (1.0/(2.0*Map->eccentricity))*log((1.0 - Map->eccentricity)/(1.0 + Map->eccentricity)));

        G->x0 = Map->equatorial_radius_m * Map->kz * dlon;
        G->y0 = (Map->equatorial_radius_m*q) / (2.0*Map->kz);

		for(iCol=0; iCol<G->nCol; iCol++){
			x = G->x0 + G->map_scale_m*iCol;
			G->lon[iCol] = Map->reference_longitude + x / ( Map->equatorial_radius_m * Map->kz );
            G->lon[iCol] = easeconv_normalize_radians( G->lon[iCol] );
            ease2_userdefined_LL2RC(G, G->lat0, G->lon[iCol], &dummy, &G->col[iCol]);
		}
		for(iRow=0; iRow<G->nRow; iRow++){
			y = G->y0 - G->map_scale_m*iRow;
            beta = asin( 2.0 * y * Map->kz/( Map->equatorial_radius_m * qp ) );            
			G->lat[iRow] = beta + (((Map->e2/3.0) + ((31.0/180.0)*Map->e4) + ((517.0/5040.0)*Map->e6)) * sin(2*beta)) + ((((23.0/360.0)*Map->e4) + ((251.0/3780.0)*Map->e6))*sin(4*beta)) + (((761.0/45360.0)*Map->e6)*sin(6.0*beta));
            ease2_userdefined_LL2RC(G, G->lat[iRow], G->lon0, &G->row[iRow], &dummy);
		}
    }
}

void calcRunningMedian(long cnt, long idx, double x, double *mid, double *midLow, double *midUp)
{
    if(cnt == 1) mid[idx] = x;
    else if(cnt == 2){
        if(x < mid[idx]){
            midLow[idx] = x;
            midUp[idx] = mid[idx];
        }
        else{
            midLow[idx] = mid[idx];
            midUp[idx] = x;
        }
        mid[idx] = (x+mid[idx])/2;
    }
    else{
        if(cnt%2==0){
            if(x < midLow[idx]){
                midUp[idx] = mid[idx];
                mid[idx] = (midLow[idx]+mid[idx])/2;  
            } 
            else if(x >= midLow[idx] && x < mid[idx]){
                midLow[idx] = x;
                midUp[idx] = mid[idx];
                mid[idx] = (x+mid[idx])/2;
            } 
            else if(x >= mid[idx] && x < midUp[idx]){
                midLow[idx] = mid[idx];
                midUp[idx] = x;
                mid[idx] = (x+mid[idx])/2;  
            }
            else if(x >= midUp[idx]){
                midLow[idx] = mid[idx];
                mid[idx] = (midUp[idx]+mid[idx])/2;
            }
        }
        else{
            if(x < midLow[idx]){
                mid[idx] = midLow[idx];
                midLow[idx] = x;
            }
            else if(x >= midLow[idx] && x < midUp[idx]){
                mid[idx] = x;
            }
            else if(x >= midUp[idx]){
                mid[idx] = midUp[idx];
                midUp[idx] = x;
            }
        }
    }
    //printf("cnt=%ld, x=%lf, mid=%lf %lf %lf\n", cnt, x, midLow[idx], mid[idx], midUp[idx]);
}