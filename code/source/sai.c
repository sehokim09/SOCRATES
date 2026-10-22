/***************************************************************************/
/*                    SOCRATES SAI observation model                       */
/*                                                                         */
/*  See code/include/sai.h for the interface. The reference is the MATLAB  */
/*  evaluator single_ant_model.m with LookupAntGainSAI.m and                */
/*  LookupAntGainDualPol.m; expressions below follow its order of          */
/*  operations so that the two agree to the last bits wherever possible.   */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "sai.h"

#define SAI_LIGHTSPEED 299792458.0      /* m/s, MATLAB physconst("lightspeed") */

static const double saiPi  = 3.14159265358979323846;
static const char *saiGnName[4] = {"XX", "XY", "YX", "YY"};

/* --------------------------------------------------------------------- */
/*  Small helpers                                                         */
/* --------------------------------------------------------------------- */

/* MATLAB mod(a, m) for m > 0 */
static double saiMod(double a, double m)
{
    return a - floor(a/m)*m;
}

static void saiJoin(char *out, size_t n, const char *dir, const char *name)
{
    size_t len = strlen(dir);
    if (len > 0 && dir[len-1] != '/')
        snprintf(out, n, "%s/%s", dir, name);
    else
        snprintf(out, n, "%s%s", dir, name);
}

/* Text of a "value ! comment" line, trimmed. Returns 0 at end of file. */
static int saiReadValue(FILE *fp, char *val, size_t n)
{
    char line[512], *p, *bang;
    size_t len;

    if (fgets(line, sizeof line, fp) == NULL)
        return 0;
    if ((bang = strchr(line, '!')) != NULL)
        *bang = '\0';
    p = line;
    while (isspace((unsigned char) *p)) p++;
    len = strlen(p);
    while (len > 0 && isspace((unsigned char) p[len-1])) p[--len] = '\0';
    snprintf(val, n, "%s", p);
    return 1;
}

static int saiReadDouble(FILE *fp, const char *what, double *v)
{
    char val[256], *end;

    if (!saiReadValue(fp, val, sizeof val)) {
        fprintf(stderr, "saiLoad: Inp_SAI.txt ends before \"%s\"\n", what);
        return -1;
    }
    *v = strtod(val, &end);
    if (end == val || *end != '\0') {
        fprintf(stderr, "saiLoad: \"%s\" is not a number for %s\n", val, what);
        return -1;
    }
    return 0;
}

static int saiReadString(FILE *fp, const char *what, char *v, size_t n)
{
    if (!saiReadValue(fp, v, n) || v[0] == '\0') {
        fprintf(stderr, "saiLoad: Inp_SAI.txt has no value for %s\n", what);
        return -1;
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*  MATLAB unique(v, 'first' | 'last')                                    */
/* --------------------------------------------------------------------- */

static const double *saiSortKey;

static int saiCmpIdx(const void *a, const void *b)
{
    size_t i = *(const size_t *) a, j = *(const size_t *) b;
    if (saiSortKey[i] < saiSortKey[j]) return -1;
    if (saiSortKey[i] > saiSortKey[j]) return 1;
    return (i > j) - (i < j);           /* stable: equal values keep index order */
}

/* Sorted unique values of v[0..n-1]; idx receives, for each, the first or
   last index of that value in v. Returns the number of unique values. */
static size_t saiUnique(const double *v, size_t n, int keepLast, size_t *idx)
{
    size_t *ord, i, nu = 0;

    ord = (size_t *) malloc(n*sizeof(size_t));
    if (ord == NULL) return 0;
    for (i = 0; i < n; i++) ord[i] = i;
    saiSortKey = v;
    qsort(ord, n, sizeof(size_t), saiCmpIdx);
    for (i = 0; i < n; i++) {
        if (nu > 0 && v[ord[i]] == v[idx[nu-1]]) {
            if (keepLast) idx[nu-1] = ord[i];
        }
        else {
            idx[nu++] = ord[i];
        }
    }
    free(ord);
    return nu;
}

/* --------------------------------------------------------------------- */
/*  Reflectivity CDF                                                      */
/* --------------------------------------------------------------------- */

static int saiLoadCdf(const char *path, SaiCdf *C)
{
    FILE *fp;
    size_t cap = 1024, k, col, *iu;
    double f, x[SAI_NCDFCOL];

    if ((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, "saiLoad: cannot open CDF file %s\n", path);
        return -1;
    }
    C->n = 0;
    C->F = (double *) malloc(cap*sizeof(double));
    for (col = 0; col < SAI_NCDFCOL; col++)
        C->X[col] = (double *) malloc(cap*sizeof(double));
    while (fscanf(fp, " %lf , %lf , %lf , %lf , %lf", &f, &x[0], &x[1], &x[2], &x[3]) == 5) {
        if (C->n == cap) {
            cap *= 2;
            C->F = (double *) realloc(C->F, cap*sizeof(double));
            for (col = 0; col < SAI_NCDFCOL; col++)
                C->X[col] = (double *) realloc(C->X[col], cap*sizeof(double));
        }
        if (!isfinite(f) || !isfinite(x[0]) || !isfinite(x[1]) || !isfinite(x[2]) || !isfinite(x[3])) {
            fprintf(stderr, "saiLoad: non-finite value in row %zu of %s\n", C->n + 1, path);
            fclose(fp);
            return -1;
        }
        C->F[C->n] = f;
        for (col = 0; col < SAI_NCDFCOL; col++)
            C->X[col][C->n] = x[col];
        C->n++;
    }
    if (!feof(fp)) {
        fprintf(stderr, "saiLoad: unreadable row %zu of %s (expected F, X137, X255, X370, X1575)\n", C->n + 1, path);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    if (C->n < 2) {
        fprintf(stderr, "saiLoad: %s has fewer than two rows\n", path);
        return -1;
    }

    /* [Xu, iu] = unique(X(:,col), 'last');  Fu = F(iu) */
    iu = (size_t *) malloc(C->n*sizeof(size_t));
    for (col = 0; col < SAI_NCDFCOL; col++) {
        C->nu[col] = saiUnique(C->X[col], C->n, 1, iu);
        C->Xu[col] = (double *) malloc(C->nu[col]*sizeof(double));
        C->Fu[col] = (double *) malloc(C->nu[col]*sizeof(double));
        for (k = 0; k < C->nu[col]; k++) {
            C->Xu[col][k] = C->X[col][iu[k]];
            C->Fu[col][k] = C->F[iu[k]];
        }
    }
    free(iu);
    return 0;
}

double saiStepF(const SaiCdf *C, int col, double x)
{
    const double *Xu = C->Xu[col];
    size_t lo = 0, hi = C->nu[col] - 1, mid;

    if (isnan(x)) return NAN;
    if (x < Xu[0]) return 0.0;
    if (x >= Xu[hi]) return C->Fu[col][hi];
    while (hi - lo > 1) {               /* Xu[lo] <= x < Xu[hi] */
        mid = lo + (hi - lo)/2;
        if (Xu[mid] <= x) lo = mid;
        else hi = mid;
    }
    return C->Fu[col][lo];
}

/* gammaQuantile of single_ant_model.m applied to unique(X(:,col),'last'):
   [Fu2, iu2] = unique(Fu, 'first'); interp1(Fu2, Xu(iu2), p, 'linear', 'extrap') */
static double saiQuantile(const SaiCdf *C, int col, double p)
{
    size_t nu = C->nu[col], n2, k, *iu;
    double x0, x1, y0, y1, q;

    iu = (size_t *) malloc(nu*sizeof(size_t));
    if (iu == NULL) return NAN;
    n2 = saiUnique(C->Fu[col], nu, 0, iu);
    if (n2 < 2) { free(iu); return NAN; }
    k = 0;                              /* segment [k, k+1], end segments extrapolate */
    while (k + 2 < n2 && C->Fu[col][iu[k+1]] <= p) k++;
    x0 = C->Fu[col][iu[k]];  x1 = C->Fu[col][iu[k+1]];
    y0 = C->Xu[col][iu[k]];  y1 = C->Xu[col][iu[k+1]];
    q = y0 + (p - x0)*(y1 - y0)/(x1 - x0);
    free(iu);
    return q;
}

/* --------------------------------------------------------------------- */
/*  Antenna patterns                                                      */
/* --------------------------------------------------------------------- */

static int saiLoadGrid(const char *path, SaiGrid *G)
{
    FILE *fp;
    long size;
    char *buf, *p, *end;
    size_t cap = 65536, nVal = 0;
    int nInRow, nRow = 0;

    if ((fp = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "saiLoad: cannot open pattern file %s\n", path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    buf = (char *) malloc((size_t) size + 1);
    if (buf == NULL || fread(buf, 1, (size_t) size, fp) != (size_t) size) {
        fprintf(stderr, "saiLoad: cannot read %s\n", path);
        free(buf);
        fclose(fp);
        return -1;
    }
    buf[size] = '\0';
    fclose(fp);

    G->v = (double *) malloc(cap*sizeof(double));
    G->nTheta = 0;
    p = buf;
    while (*p != '\0') {                /* one row per line, comma-delimited */
        nInRow = 0;
        while (*p != '\n' && *p != '\0') {
            double val = strtod(p, &end);
            if (end == p) {
                fprintf(stderr, "saiLoad: unreadable value in row %d of %s\n", nRow + 1, path);
                free(buf);
                return -1;
            }
            if (nVal == cap) {
                cap *= 2;
                G->v = (double *) realloc(G->v, cap*sizeof(double));
            }
            G->v[nVal++] = val;
            nInRow++;
            p = end;
            while (*p == ' ' || *p == '\t' || *p == '\r') p++;
            if (*p == ',') p++;
        }
        if (*p == '\n') p++;
        if (nInRow == 0) continue;      /* blank line */
        if (nRow == 0) G->nTheta = nInRow;
        else if (nInRow != G->nTheta) {
            fprintf(stderr, "saiLoad: row %d of %s has %d values, row 1 has %d\n", nRow + 1, path, nInRow, G->nTheta);
            free(buf);
            return -1;
        }
        nRow++;
    }
    free(buf);
    G->nPhi = nRow;
    if (G->nTheta < 2 || G->nPhi < 2) {
        fprintf(stderr, "saiLoad: %s is %d x %d\n", path, G->nPhi, G->nTheta);
        return -1;
    }
    G->res = 180.0/(G->nTheta - 1);     /* derived, never hardcoded */
    if (fabs((G->nPhi - 1)*G->res - 360.0) > 1e-6) {
        fprintf(stderr, "saiLoad: %s is %d x %d: theta step %.4g deg does not span phi 0-360\n",
                path, G->nPhi, G->nTheta, G->res);
        return -1;
    }
    return 0;
}

/* Nearest-neighbour lookup (localLookup / localPick). NaN for non-finite angles. */
static double saiPick(const SaiGrid *G, double th, double ph)
{
    const double R2D = 180.0/saiPi;
    long iTheta, iPhi;

    if (!isfinite(th) || !isfinite(ph)) return NAN;
    ph = saiMod(ph + 2.0*saiPi, 2.0*saiPi);
    th = fmin(fmax(th, 0.0), saiPi);
    iTheta = (long) round(th*R2D / G->res);
    iPhi   = (long) round(ph*R2D / G->res);
    if (iTheta < 0) iTheta = 0;
    if (iTheta > G->nTheta - 1) iTheta = G->nTheta - 1;
    if (iPhi < 0) iPhi = 0;
    if (iPhi > G->nPhi - 1) iPhi = G->nPhi - 1;
    return G->v[iPhi*G->nTheta + iTheta];
}

/* Peak co-polar directivity of the circular port, localLoadCP:
   10 log10( 4 pi max(co^2) / sum_{phi rows 1..end-1} (co^2 + cx^2) sin(th) dth dph ) */
static double saiCpPeak_dB(const SaiGrid *co, const SaiGrid *cx)
{
    double dth = co->res*saiPi/180.0, th, dOm, tot = 0.0, vmax = co->v[0];
    int iPhi, iTheta, k;

    for (k = 0; k < co->nPhi*co->nTheta; k++)
        if (co->v[k] > vmax) vmax = co->v[k];
    for (iTheta = 0; iTheta < co->nTheta; iTheta++) {     /* column-major, as MATLAB sums */
        th = iTheta*co->res*saiPi/180.0;
        dOm = sin(th)*(dth*dth);
        for (iPhi = 0; iPhi < co->nPhi - 1; iPhi++) {
            k = iPhi*co->nTheta + iTheta;
            tot += (co->v[k]*co->v[k] + cx->v[k]*cx->v[k])*dOm;
        }
    }
    return 10.0*log10(4.0*saiPi*(vmax*vmax)/tot);
}

static int saiLoadPattern(const char *patDir, const char *tag, SaiPattern *P)
{
    char name[128], path[1024], cpDir[1024];
    int i;

    snprintf(P->tag, sizeof P->tag, "%s", tag);
    for (i = 0; i < 4; i++) {
        snprintf(name, sizeof name, "%s_gn%s.csv", tag, saiGnName[i]);
        saiJoin(path, sizeof path, patDir, name);
        if (saiLoadGrid(path, &P->gn[i])) return -1;
        if (P->gn[i].nPhi != P->gn[0].nPhi || P->gn[i].nTheta != P->gn[0].nTheta) {
            fprintf(stderr, "saiLoad: %s differs in size from %s_gnXX.csv\n", path, tag);
            return -1;
        }
    }
    saiJoin(cpDir, sizeof cpDir, patDir, "cp/");
    snprintf(name, sizeof name, "%s_L_norm_vol_num.csv", tag);
    saiJoin(path, sizeof path, cpDir, name);
    if (saiLoadGrid(path, &P->cpL)) return -1;
    snprintf(name, sizeof name, "%s_R_norm_vol_num.csv", tag);
    saiJoin(path, sizeof path, cpDir, name);
    if (saiLoadGrid(path, &P->cpR)) return -1;
    if (P->cpL.nPhi != P->cpR.nPhi || P->cpL.nTheta != P->cpR.nTheta) {
        fprintf(stderr, "saiLoad: cp/%s_L and cp/%s_R differ in size\n", tag, tag);
        return -1;
    }
    P->GcpPeak_dB = saiCpPeak_dB(&P->cpL, &P->cpR);
    return 0;
}

/* --------------------------------------------------------------------- */
/*  Load / free                                                           */
/* --------------------------------------------------------------------- */

static int saiCheckEtaBar(const SaiTx *T, double eta, double etaBar, const char *which)
{
    double ref = sqrt(1.0 + T->Nincoh*eta*eta) - 1.0;
    if (fabs(etaBar - ref) > 1e-9*fabs(ref)) {
        fprintf(stderr, "saiLoad: %s %s = %.17g but sqrt(1 + Nincoh*eta^2) - 1 = %.17g\n",
                T->name, which, etaBar, ref);
        return -1;
    }
    return 0;
}

int saiLoad(const char *inpPath, const char *antRoot, SaiModel *M)
{
    FILE *fp;
    char val[256], inpDir[1024], path[1024], patDir[1024], patSub[256];
    char cdfName[SAI_NSCORE][256], row[512], type[8], tag[40];
    const char *slash;
    double nTx;
    int i, k, ip, n;
    const char *portName[SAI_NSCORE] = {"X", "Y", "L"};

    memset(M, 0, sizeof *M);
    if ((fp = fopen(inpPath, "r")) == NULL) {
        fprintf(stderr, "saiLoad: cannot open %s\n", inpPath);
        return -1;
    }
    slash = strrchr(inpPath, '/');
    if (slash == NULL) snprintf(inpDir, sizeof inpDir, "./");
    else snprintf(inpDir, sizeof inpDir, "%.*s", (int) (slash - inpPath + 1), inpPath);

    if (!saiReadValue(fp, val, sizeof val)) {                          /* header */
        fclose(fp);
        goto truncated;
    }
    if (saiReadString(fp, "the source", M->source, sizeof M->source)
        || saiReadDouble(fp, "gamma", &M->gamma)
        || saiReadDouble(fp, "W_D", &M->WD)
        || saiReadDouble(fp, "eta_HV", &M->etaHV)
        || saiReadDouble(fp, "eta_L", &M->etaL)
        || saiReadDouble(fp, "the direct-arm loss", &M->dirLinLoss_dB)
        || saiReadString(fp, "the CDF file of port X", cdfName[SAI_SCORE_X], sizeof cdfName[0])
        || saiReadString(fp, "the CDF file of port Y", cdfName[SAI_SCORE_Y], sizeof cdfName[0])
        || saiReadString(fp, "the CDF file of port L", cdfName[SAI_SCORE_LCP], sizeof cdfName[0])
        || saiReadString(fp, "the pattern subdirectory", patSub, sizeof patSub)
        || saiReadString(fp, "the run tag", M->runTag, sizeof M->runTag)
        || saiReadDouble(fp, "the number of transmitters", &nTx)) {
        fclose(fp);
        return -1;
    }
    if (!(M->gamma > 0.0 && M->gamma < 1.0) || !(M->WD > 0.0)) {
        fprintf(stderr, "saiLoad: gamma = %g, W_D = %g out of range\n", M->gamma, M->WD);
        fclose(fp);
        return -1;
    }
    M->nTx = (int) nTx;
    if (M->nTx < 1 || M->nTx > SAI_MAX_TX || (double) M->nTx != nTx) {
        fprintf(stderr, "saiLoad: number of transmitters %g (1 to %d)\n", nTx, SAI_MAX_TX);
        fclose(fp);
        return -1;
    }

    /* transmitter table, after any "!" comment lines */
    i = 0;
    while (i < M->nTx && fgets(row, sizeof row, fp) != NULL) {
        SaiTx *T = &M->tx[i];
        char *p = row;
        while (isspace((unsigned char) *p)) p++;
        if (*p == '!' || *p == '\0') continue;
        n = sscanf(p, "%19s %7s %d %39s %lf %lf %lf %lf %lf %lf %lf %lf %d",
                   T->name, type, &T->cdfCol, tag, &T->GlinPeak_dB, &T->GcpPeak_dB,
                   &T->S0, &T->Nc, &T->TI, &T->Nincoh, &T->etaBarHV, &T->etaBarL, &T->useSpotbeam);
        if (n != 13) {
            fprintf(stderr, "saiLoad: transmitter row %d has %d of 13 fields: %s", i + 1, n, row);
            fclose(fp);
            return -1;
        }
        if (!strcmp(type, "SAI")) T->isGNSS = 0;
        else if (!strcmp(type, "GNSS")) T->isGNSS = 1;
        else {
            fprintf(stderr, "saiLoad: %s has type \"%s\" (SAI or GNSS)\n", T->name, type);
            fclose(fp);
            return -1;
        }
        if (T->cdfCol < 1 || T->cdfCol > SAI_NCDFCOL) {
            fprintf(stderr, "saiLoad: %s has CDF column %d (1 to %d)\n", T->name, T->cdfCol, SAI_NCDFCOL);
            fclose(fp);
            return -1;
        }
        T->cdfCol--;
        if (!(T->S0 > 0.0 && T->Nc > 0.0 && T->TI > 0.0 && T->Nincoh >= 1.0)) {
            fprintf(stderr, "saiLoad: %s has S0 %g, Nc %g, TI %g, Nincoh %g\n", T->name, T->S0, T->Nc, T->TI, T->Nincoh);
            fclose(fp);
            return -1;
        }
        if (saiCheckEtaBar(T, M->etaHV, T->etaBarHV, "etaBarHV")
            || saiCheckEtaBar(T, M->etaL, T->etaBarL, "etaBarL")) {
            fclose(fp);
            return -1;
        }
        for (k = 0; k < i; k++) {
            if (!strcmp(M->tx[k].name, T->name)) {
                fprintf(stderr, "saiLoad: transmitter %s listed twice\n", T->name);
                fclose(fp);
                return -1;
            }
        }
        /* pattern tag, loaded once */
        for (k = 0; k < M->nPat; k++)
            if (!strcmp(M->pat[k].tag, tag)) break;
        if (k == M->nPat) {
            saiJoin(patDir, sizeof patDir, antRoot, patSub);
            if (saiLoadPattern(patDir, tag, &M->pat[M->nPat])) {
                fclose(fp);
                saiFree(M);
                return -1;
            }
            M->nPat++;
        }
        T->pat = &M->pat[k];
        i++;
    }
    fclose(fp);
    if (i < M->nTx) goto truncated;

    /* CDF tables, resolved next to Inp_SAI.txt; one F grid for all ports */
    for (ip = 0; ip < SAI_NSCORE; ip++) {
        saiJoin(path, sizeof path, inpDir, cdfName[ip]);
        if (saiLoadCdf(path, &M->cdf[ip])) { saiFree(M); return -1; }
        if (M->cdf[ip].n != M->cdf[0].n || memcmp(M->cdf[ip].F, M->cdf[0].F, M->cdf[0].n*sizeof(double))) {
            fprintf(stderr, "saiLoad: the F grid of port %s differs from that of port X\n", portName[ip]);
            saiFree(M);
            return -1;
        }
        M->Ginv[ip] = saiQuantile(&M->cdf[ip], SAI_NCDFCOL - 1, 1.0 - M->gamma);
    }

    /* Gcp from the pattern integral is what the evaluator uses; the file value
       is the rounded saiAnt.G_cp_dB and serves as a check */
    for (i = 0; i < M->nTx; i++) {
        SaiTx *T = &M->tx[i];
        if (fabs(T->pat->GcpPeak_dB - T->GcpPeak_dB) > 0.01) {
            fprintf(stderr, "saiLoad: %s Gcp from the %s pattern integral is %.4f dBi, Inp_SAI.txt has %.4f\n",
                    T->name, T->pat->tag, T->pat->GcpPeak_dB, T->GcpPeak_dB);
            saiFree(M);
            return -1;
        }
        if (T->isGNSS) {
            /* RCG_threshold = 1 ./ (eta_bar .* S0 .* N_coh .* Ginv) */
            for (ip = 0; ip < SAI_NSCORE; ip++) {
                double eb = (ip == SAI_SCORE_LCP) ? T->etaBarL : T->etaBarHV;
                double Ginv = saiQuantile(&M->cdf[ip], T->cdfCol, 1.0 - M->gamma);
                T->thrRCG[ip] = 1.0/(eb*T->S0*T->Nc*Ginv);
            }
        }
    }
    return 0;

truncated:
    fprintf(stderr, "saiLoad: %s is truncated\n", inpPath);
    saiFree(M);
    return -1;
}

static void saiFreeGrid(SaiGrid *G)
{
    free(G->v);
    G->v = NULL;
}

void saiFree(SaiModel *M)
{
    int i, k;

    for (i = 0; i < M->nPat; i++) {
        for (k = 0; k < 4; k++) saiFreeGrid(&M->pat[i].gn[k]);
        saiFreeGrid(&M->pat[i].cpL);
        saiFreeGrid(&M->pat[i].cpR);
    }
    M->nPat = 0;
    for (i = 0; i < SAI_NSCORE; i++) {
        free(M->cdf[i].F);
        M->cdf[i].F = NULL;
        for (k = 0; k < SAI_NCDFCOL; k++) {
            free(M->cdf[i].X[k]);  free(M->cdf[i].Xu[k]);  free(M->cdf[i].Fu[k]);
            M->cdf[i].X[k] = M->cdf[i].Xu[k] = M->cdf[i].Fu[k] = NULL;
        }
    }
    M->nTx = 0;
}

const SaiTx *saiFindTx(const SaiModel *M, const char *soopName, const char *txName)
{
    int i;

    for (i = 0; i < M->nTx; i++)
        if (!strcmp(M->tx[i].name, soopName)) return &M->tx[i];
    for (i = 0; i < M->nTx; i++)
        if (!strcmp(M->tx[i].name, txName)) return &M->tx[i];
    return NULL;
}

/* --------------------------------------------------------------------- */
/*  Evaluation                                                            */
/* --------------------------------------------------------------------- */

/* Port gains [dBi] of one scoring (LookupAntGainSAI) */
static void saiGains(const SaiModel *M, const SaiTx *T, int score,
                     double elD, double azD, double elR, double azR,
                     double *G_D_dB, double *G_R_dB)
{
    const SaiPattern *P = T->pat;
    double a, b;
    int r = (score == SAI_SCORE_Y) ? 2 : 0;        /* port2 = YX, YY; port1 = XX, XY */

    if (score == SAI_SCORE_LCP) {
        /* reflected co-polar (L file), direct through the back (R file), no mismatch term */
        *G_R_dB = 20.0*log10(saiPick(&P->cpL, elR, azR)) + P->GcpPeak_dB;
        *G_D_dB = T->isGNSS ? NAN : 20.0*log10(saiPick(&P->cpR, elD, azD)) + P->GcpPeak_dB;
        return;
    }
    a = saiPick(&P->gn[r], elR, azR);
    b = saiPick(&P->gn[r+1], elR, azR);
    *G_R_dB = 10.0*log10(fabs(a)*fabs(a) + fabs(b)*fabs(b)) + T->GlinPeak_dB;
    if (T->isGNSS) {
        *G_D_dB = NAN;
    }
    else {
        a = saiPick(&P->gn[r], elD, azD);
        b = saiPick(&P->gn[r+1], elD, azD);
        *G_D_dB = 10.0*log10(fabs(a)*fabs(a) + fabs(b)*fabs(b)) + T->GlinPeak_dB - M->dirLinLoss_dB;
    }
}

unsigned saiEvaluate(const SaiModel *M, const SaiTx *T,
                     double rangeTR, double rangeTS, double rangeSR,
                     double elD, double azD, double elR, double azR,
                     SaiDiag *diag)
{
    unsigned mask = 0;
    double G_D_dB, G_R_dB, tau = NAN, kap = NAN;
    int s;

    if (!T->isGNSS) {
        tau = (rangeTS + rangeSR - rangeTR)/SAI_LIGHTSPEED;
        kap = 1.0 - 2.0*fmin(tau, T->TI)/T->TI;
        if (kap > -1.0) mask |= SAI_KAPPA_GT_M1;
    }
    if (diag != NULL) {
        diag->tauRD = tau;
        diag->kappa = kap;
    }

    for (s = 0; s < SAI_NSCORE; s++) {
        double eb = (s == SAI_SCORE_LCP) ? T->etaBarL : T->etaBarHV;
        const SaiCdf *C = &M->cdf[s];
        int pass;

        saiGains(M, T, s, elD, azD, elR, azR, &G_D_dB, &G_R_dB);

        if (T->isGNSS) {
            /* beta/alpha = G_R / R_R^2 > 1/(eta_bar S0 N_c F^-1(1-gamma)) */
            double ba = pow(10.0, (G_R_dB - 20.0*log10(rangeTS + rangeSR))/10.0);
            pass = ba > T->thrRCG[s];
            if (diag != NULL) {
                diag->G_D_dB[s] = G_D_dB;  diag->G_R_dB[s] = G_R_dB;
                diag->ba[s] = ba;  diag->thr[s] = T->thrRCG[s];
                diag->alpha[s] = diag->SD[s] = diag->beta[s] = NAN;
                diag->rhoLo[s] = diag->rhoHi[s] = diag->P[s] = NAN;
                diag->pass1[s] = 0;
            }
        }
        else {
            /* Test 1: S_D > S_D,th = 4/(eta_bar N_c - kappa - 4) */
            double alpha_dB = 20.0*log10(rangeTR) - G_D_dB;
            double alpha = pow(10.0, alpha_dB/10.0);
            double SD = pow(10.0, (10.0*log10(T->S0) - alpha_dB)/10.0);
            double NcMin = (4.0 + kap)/eb;
            int feasible = T->Nc > NcMin;
            double SDth = feasible ? 4.0/(eb*T->Nc - kap - 4.0) : INFINITY;
            int pass1 = feasible && (alpha < T->S0/(M->WD*SDth));
            double beta = NAN, rhoLo = NAN, rhoHi = NAN, P = 0.0;

            if (pass1) {
                /* Test 2: rho_2 < Gamma_e < rho_1, rho = C +/- sqrt(C^2 - N_D^2) */
                double ND = 1.0 + 1.0/SD;
                double Cc = (eb*T->Nc - kap - 2.0*ND)/2.0;
                double disc = Cc*Cc - ND*ND;
                double beta_dB = 20.0*log10(rangeTR/(rangeTS + rangeSR)) + (G_R_dB - G_D_dB);
                beta = pow(10.0, beta_dB/10.0);
                if (disc >= 0.0) {      /* always true once Test 1 passes (C > N_D) */
                    rhoHi = pow(10.0, (10.0*log10(Cc + sqrt(disc)) - beta_dB)/10.0);
                    rhoLo = pow(10.0, (10.0*log10(Cc - sqrt(disc)) - beta_dB)/10.0);
                    P = saiStepF(C, T->cdfCol, rhoHi) - saiStepF(C, T->cdfCol, rhoLo);
                }
            }
            pass = P > M->gamma;
            if (diag != NULL) {
                diag->G_D_dB[s] = G_D_dB;  diag->G_R_dB[s] = G_R_dB;
                diag->alpha[s] = alpha;  diag->SD[s] = SD;  diag->beta[s] = beta;
                diag->rhoLo[s] = rhoLo;  diag->rhoHi[s] = rhoHi;  diag->P[s] = P;
                diag->pass1[s] = pass1;
                diag->ba[s] = diag->thr[s] = NAN;
            }
        }
        if (pass) mask |= 1u << s;
    }
    return mask;
}
