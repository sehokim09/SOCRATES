/***************************************************************************/
/*                 SOCRATES SAI observation model checker                  */
/*                                                                         */
/*  Standalone validation of sai.c against the MATLAB evaluator            */
/*  single_ant_model.m (doc/SAI_model_validation.md).                      */
/*                                                                         */
/*  sai_check --selftest INP_SAI ANT_ROOT [CDF_CSV]                         */
/*      Unit checks: Test 1 at kappa = -1 against the printed manuscript   */
/*      form, rho1*rho2 = N_D^2, the step CDF lookup against a direct      */
/*      evaluation of its definition (and, with CDF_CSV, the x/F pairs for */
/*      a MATLAB interp1 cross-check), and Gcp against Inp_SAI.txt.        */
/*                                                                         */
/*  sai_check [--alias NAME=SAT]... [--out DIR] INP_SAI ANT_ROOT FILE.nc...*/
/*      Evaluates every specular point of the single-receiver files, in    */
/*      the order given, and prints the pass fractions per transmitter.    */
/*      NAME=SAT reads transmitter NAME from the SAT variables of the      */
/*      files (UFO=MUOS for UFO on the MUOS orbits). With --out, writes    */
/*      per transmitter the per-point bitmask <Tx>_flags.u8 (SAI_PASS_*,   */
/*      SAI_KAPPA_GT_M1), the per-port values <Tx>_val.f64 (n x 3, ports   */
/*      X, Y, Lcp: P for SAI, beta/alpha for GNSS, little-endian doubles), */
/*      and summary.txt (point counts per file, GNSS thresholds).          */
/*      Points are pooled in file order, per-file point order kept; MUOS   */
/*      rows are its spot-beam points only, as in single_ant_model.m.      */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <netcdf.h>
#include "sai.h"

#define MAX_ALIAS 16
#define MAX_FILES 64

static const char *scoreName[SAI_NSCORE] = {"X", "Y", "Lcp"};

static void usage(void)
{
    fprintf(stderr,
        "usage: sai_check --selftest INP_SAI ANT_ROOT [CDF_CSV]\n"
        "       sai_check [--alias NAME=SAT]... [--out DIR] INP_SAI ANT_ROOT FILE.nc...\n");
    exit(2);
}

/* --------------------------------------------------------------------- */
/*  Self-test                                                             */
/* --------------------------------------------------------------------- */

/* Deterministic uniform in [0,1) (64-bit LCG), so runs are reproducible */
static double lcg01(unsigned long long *s)
{
    *s = *s*6364136223846793005ULL + 1442695040888963407ULL;
    return (double) (*s >> 11)*(1.0/9007199254740992.0);
}

/* F at x from the definition, on the raw table: the F of the last row
   holding the largest X <= x; 0 below the table */
static double stepRef(const SaiCdf *C, int col, double x)
{
    size_t i, best = 0;
    int found = 0;
    for (i = 0; i < C->n; i++) {
        double v = C->X[col][i];
        if (v <= x && (!found || v >= C->X[col][best])) { best = i; found = 1; }
    }
    return found ? C->F[best] : 0.0;
}

static int selftest(const SaiModel *M, const char *cdfCsv)
{
    int nFail = 0, t, s, k, col, ip;
    unsigned long long seed = 20261021ULL;
    const double c = 299792458.0;
    double worst, worstExcess, worstPlain;
    FILE *fp = NULL;

    /* 1. Test 1 at kappa = -1: alpha < (eta_bar Nc - 3) S0 / (4 W_D) */
    printf("[1] Test 1 at kappa = -1 against alpha < (eta_bar*Nc - 3)*S0/(4*W_D)\n");
    worst = 0.0;
    for (t = 0; t < M->nTx; t++) {
        const SaiTx *T = &M->tx[t];
        if (T->isGNSS) continue;
        for (s = 0; s < SAI_NSCORE; s++) {
            double eb = (s == SAI_SCORE_LCP) ? T->etaBarL : T->etaBarHV;
            double thrPrinted = (eb*T->Nc - 3.0)*T->S0/(4.0*M->WD);
            double thrCode = T->S0/(M->WD*(4.0/(eb*T->Nc - (-1.0) - 4.0)));
            double rel = fabs(thrCode - thrPrinted)/thrPrinted;
            double elD = 120.0*M_PI/180.0, azD = 0.3, elR = 30.0*M_PI/180.0, azR = 1.1;
            double TR0 = 1.0e6, SR = 6.0e5, GD_dB, side[2] = {1.0 - 1e-9, 1.0 + 1e-9};
            int okSide[2];
            SaiDiag d;
            /* kappa = -1 needs tau_RD >= T_I: R_R - R_D = 2 c T_I */
            saiEvaluate(M, T, TR0, TR0 + 2.0*c*T->TI - SR, SR, elD, azD, elR, azR, &d);
            GD_dB = d.G_D_dB[s];
            for (k = 0; k < 2; k++) {
                double TR = sqrt(thrPrinted*side[k]*pow(10.0, GD_dB/10.0));
                saiEvaluate(M, T, TR, TR + 2.0*c*T->TI - SR, SR, elD, azD, elR, azR, &d);
                okSide[k] = (k == 0) ? d.pass1[s] == 1 : d.pass1[s] == 0;
                if (d.kappa != -1.0) okSide[k] = 0;
            }
            if (rel > worst) worst = rel;
            if (rel > 1e-12 || !okSide[0] || !okSide[1]) {
                nFail++;
                printf("    FAIL %s %s: rel %.3g, pass just below %d, fail just above %d\n",
                       T->name, scoreName[s], rel, okSide[0], okSide[1]);
            }
        }
    }
    printf("    threshold expressions agree to %.3g (limit 1e-12); pass/fail flips at alpha_th*(1 -/+ 1e-9)\n", worst);

    /* 2. rho1*rho2 = N_D^2 wherever Test 1 passes. rho2 = C - sqrt(C^2 - N_D^2)
       is computed as single_ant_model.m does and loses digits to cancellation
       when C >> N_D, so the limit is 1e-12 plus the conditioning of that form,
       eps*4*C^2/N_D^2 */
    printf("[2] rho1*rho2 = N_D^2\n");
    worst = 0.0;
    worstExcess = 0.0;
    worstPlain = 0.0;
    k = 0;
    for (t = 0; t < M->nTx; t++) {
        const SaiTx *T = &M->tx[t];
        int n;
        if (T->isGNSS) continue;
        for (n = 0; n < 2000; n++) {
            double elD = (91.0 + 60.0*lcg01(&seed))*M_PI/180.0, azD = 2.0*M_PI*lcg01(&seed);
            double elR = 60.0*lcg01(&seed)*M_PI/180.0, azR = 2.0*M_PI*lcg01(&seed);
            double TR = pow(10.0, 5.0 + 3.0*lcg01(&seed)), SR = 5e5 + 1e6*lcg01(&seed);
            double TS = TR + 2.0*c*T->TI*(0.5 + lcg01(&seed)) - SR;
            SaiDiag d;
            if (TS <= 0.0) continue;
            saiEvaluate(M, T, TR, TS, SR, elD, azD, elR, azR, &d);
            for (s = 0; s < SAI_NSCORE; s++) {
                double ND, Cc, rel, lim;
                if (!d.pass1[s]) continue;
                ND = 1.0 + 1.0/d.SD[s];
                Cc = d.beta[s]*(d.rhoHi[s] + d.rhoLo[s])/2.0;
                rel = fabs(d.rhoHi[s]*d.rhoLo[s]*d.beta[s]*d.beta[s] - ND*ND)/(ND*ND);
                lim = 1e-12 + DBL_EPSILON*4.0*Cc*Cc/(ND*ND);
                if (rel > worstPlain) worstPlain = rel;
                if (rel/lim > worstExcess) { worstExcess = rel/lim; worst = rel; }
                k++;
            }
        }
    }
    if (worstExcess > 1.0 || k == 0) nFail++;
    printf("    %d points past Test 1, worst relative error %.3g; worst error / (1e-12 + eps*4C^2/N_D^2) = %.3g%s\n",
           k, worstPlain, worstExcess, (worstExcess > 1.0 || k == 0) ? "  FAIL" : "");

    /* 3. Step CDF lookup against its definition */
    printf("[3] Step CDF lookup, interp1(Xu, Fu, x, 'previous', 'extrap'), F = 0 below the table\n");
    if (cdfCsv != NULL && (fp = fopen(cdfCsv, "w")) == NULL) {
        fprintf(stderr, "cannot write %s\n", cdfCsv);
        return 1;
    }
    if (fp != NULL) fprintf(fp, "port,col,x,F\n");
    for (ip = 0; ip < SAI_NSCORE; ip++) {
        const SaiCdf *C = &M->cdf[ip];
        for (col = 0; col < SAI_NCDFCOL; col++) {
            double lo = C->X[col][0], hi = C->X[col][C->n - 1];
            int nBad = 0, nBelow = 0, nAbove = 0, nTie = 0;
            for (k = 0; k < 1000; k++) {
                double x, f;
                if (k < 50)       { x = lo*(0.01 + 0.98*lcg01(&seed)); nBelow++; }
                else if (k < 100) { x = hi*(1.0 + 10.0*lcg01(&seed)); nAbove++; }
                else if (k < 200) { x = C->X[col][(size_t) (lcg01(&seed)*C->n)]; nTie++; }
                else              x = lo*pow(hi/lo, lcg01(&seed));
                f = saiStepF(C, col, x);
                if (f != stepRef(C, col, x)) nBad++;
                if (fp != NULL) fprintf(fp, "%s,%d,%.17g,%.17g\n", scoreName[ip], col + 1, x, f);
            }
            if (nBad) nFail++;
            printf("    port %-3s col %d: %4d of 1000 differ (%d below, %d above, %d on a step)%s\n",
                   scoreName[ip], col + 1, nBad, nBelow, nAbove, nTie, nBad ? "  FAIL" : "");
        }
    }
    if (fp != NULL) {
        fclose(fp);
        printf("    x/F pairs written to %s for the MATLAB interp1 cross-check\n", cdfCsv);
    }

    /* 4. Gcp from the pattern integral against Inp_SAI.txt (saiAnt.G_cp_dB) */
    printf("[4] Gcp from the pattern integral against saiAnt.G_cp_dB in Inp_SAI.txt\n");
    for (t = 0; t < M->nTx; t++) {
        const SaiTx *T = &M->tx[t];
        double dG = T->pat->GcpPeak_dB - T->GcpPeak_dB;
        if (fabs(dG) > 0.01) nFail++;
        printf("    %-8s %-16s integral %.6f dBi, file %.4f dBi, diff %+.6f dB%s\n", T->name, T->pat->tag,
               T->pat->GcpPeak_dB, T->GcpPeak_dB, dG, fabs(dG) > 0.01 ? "  FAIL" : "");
    }

    printf("selftest: %s (%d failure%s)\n", nFail ? "FAILED" : "passed", nFail, nFail == 1 ? "" : "s");
    return nFail ? 1 : 0;
}

/* --------------------------------------------------------------------- */
/*  Per-point evaluation of single-receiver files                         */
/* --------------------------------------------------------------------- */

static double *readVar(int nc, const char *var, const char *sat, size_t n)
{
    char name[96];
    int id, rv;
    double *a = (double *) malloc((n ? n : 1)*sizeof(double));

    snprintf(name, sizeof name, "%s_%s", var, sat);
    if (a == NULL || (rv = nc_inq_varid(nc, name, &id)) || (rv = nc_get_var_double(nc, id, a))) {
        fprintf(stderr, "sai_check: cannot read %s: %s\n", name, a ? nc_strerror(rv) : "out of memory");
        exit(1);
    }
    return a;
}

int main(int argc, char **argv)
{
    const char *aliasName[MAX_ALIAS], *aliasSat[MAX_ALIAS], *outDir = NULL;
    const char *files[MAX_FILES];
    int nAlias = 0, nFiles = 0, i, t, f;
    SaiModel M;
    long long cnt[SAI_MAX_TX][6], perFile[SAI_MAX_TX][MAX_FILES];
    FILE *fFlags[SAI_MAX_TX], *fVal[SAI_MAX_TX], *fSum = NULL;
    char path[1024];

    if (argc >= 2 && !strcmp(argv[1], "--selftest")) {
        int rc;
        if (argc < 4) usage();
        if (saiLoad(argv[2], argv[3], &M)) return 1;
        rc = selftest(&M, argc > 4 ? argv[4] : NULL);
        saiFree(&M);
        return rc;
    }

    i = 1;
    while (i < argc && !strncmp(argv[i], "--", 2)) {
        if (!strcmp(argv[i], "--alias") && i + 1 < argc && nAlias < MAX_ALIAS) {
            char *eq = strchr(argv[i+1], '=');
            if (eq == NULL) usage();
            *eq = '\0';
            aliasName[nAlias] = argv[i+1];
            aliasSat[nAlias++] = eq + 1;
            i += 2;
        }
        else if (!strcmp(argv[i], "--out") && i + 1 < argc) {
            outDir = argv[i+1];
            i += 2;
        }
        else usage();
    }
    if (argc - i < 3) usage();
    if (saiLoad(argv[i], argv[i+1], &M)) return 1;
    for (f = i + 2; f < argc; f++) {
        if (nFiles == MAX_FILES) { fprintf(stderr, "sai_check: more than %d files\n", MAX_FILES); return 1; }
        files[nFiles++] = argv[f];
    }
    memset(cnt, 0, sizeof cnt);
    memset(perFile, 0, sizeof perFile);

    if (outDir != NULL) {
        for (t = 0; t < M.nTx; t++) {
            snprintf(path, sizeof path, "%s/%s_flags.u8", outDir, M.tx[t].name);
            fFlags[t] = fopen(path, "wb");
            snprintf(path, sizeof path, "%s/%s_val.f64", outDir, M.tx[t].name);
            fVal[t] = fopen(path, "wb");
            if (fFlags[t] == NULL || fVal[t] == NULL) {
                fprintf(stderr, "sai_check: cannot write in %s\n", outDir);
                return 1;
            }
        }
    }

    for (f = 0; f < nFiles; f++) {
        int nc, rv;
        if ((rv = nc_open(files[f], NC_NOWRITE, &nc))) {
            fprintf(stderr, "sai_check: %s: %s\n", files[f], nc_strerror(rv));
            return 1;
        }
        for (t = 0; t < M.nTx; t++) {
            const SaiTx *T = &M.tx[t];
            const char *sat = T->name;
            char dimName[64];
            int dimID, *spot = NULL, a;
            size_t n, k;
            double *TR, *TS, *SR, *elD, *azD, *elR, *azR;

            for (a = 0; a < nAlias; a++)
                if (!strcmp(aliasName[a], T->name)) sat = aliasSat[a];
            snprintf(dimName, sizeof dimName, "nSp_%s", sat);
            if ((rv = nc_inq_dimid(nc, dimName, &dimID)) || (rv = nc_inq_dimlen(nc, dimID, &n))) {
                fprintf(stderr, "sai_check: %s has no %s (use --alias %s=SAT): %s\n", files[f], dimName, T->name, nc_strerror(rv));
                return 1;
            }
            TR = readVar(nc, "rangeTR", sat, n);  TS = readVar(nc, "rangeTS", sat, n);  SR = readVar(nc, "rangeSR", sat, n);
            elD = readVar(nc, "lookAngElD", sat, n);  azD = readVar(nc, "lookAngAzD", sat, n);
            elR = readVar(nc, "lookAngElR", sat, n);  azR = readVar(nc, "lookAngAzR", sat, n);
            if (T->useSpotbeam) {
                int id;
                spot = (int *) malloc((n ? n : 1)*sizeof(int));
                if (spot == NULL || (rv = nc_inq_varid(nc, "flagSpotbeam", &id)) || (rv = nc_get_var_int(nc, id, spot))) {
                    fprintf(stderr, "sai_check: %s: cannot read flagSpotbeam\n", files[f]);
                    return 1;
                }
            }
            for (k = 0; k < n; k++) {
                SaiDiag d;
                unsigned m;
                if (spot != NULL && !spot[k]) continue;
                m = saiEvaluate(&M, T, TR[k], TS[k], SR[k], elD[k], azD[k], elR[k], azR[k], outDir ? &d : NULL);
                cnt[t][0]++;
                if (m & SAI_PASS_X) cnt[t][1]++;
                if (m & SAI_PASS_Y) cnt[t][2]++;
                if (m & SAI_PASS_LCP) cnt[t][3]++;
                if ((m & SAI_PASS_HV) == SAI_PASS_HV) cnt[t][4]++;
                if (m & SAI_KAPPA_GT_M1) cnt[t][5]++;
                perFile[t][f]++;
                if (outDir != NULL) {
                    unsigned char b = (unsigned char) m;
                    double v[SAI_NSCORE];
                    int s;
                    for (s = 0; s < SAI_NSCORE; s++) v[s] = T->isGNSS ? d.ba[s] : d.P[s];
                    fwrite(&b, 1, 1, fFlags[t]);
                    fwrite(v, sizeof(double), SAI_NSCORE, fVal[t]);
                }
            }
            free(TR); free(TS); free(SR); free(elD); free(azD); free(elR); free(azR); free(spot);
        }
        nc_close(nc);
    }

    printf("SAI model %s, %d file(s)\n", M.source, nFiles);
    printf("%-8s %10s %8s %8s %8s %8s %8s %6s\n", "Tx", "points", "pass X", "pass Y", "min", "H&V", "LHCP", "k>-1");
    for (t = 0; t < M.nTx; t++) {
        double n = (double) cnt[t][0], vX = cnt[t][1]/n, vY = cnt[t][2]/n;
        printf("%-8s %10lld %8.4f %8.4f %8.4f %8.4f %8.4f %6lld\n", M.tx[t].name, cnt[t][0], vX, vY,
               vX < vY ? vX : vY, cnt[t][4]/n, cnt[t][3]/n, cnt[t][5]);
    }

    if (outDir != NULL) {
        for (t = 0; t < M.nTx; t++) { fclose(fFlags[t]); fclose(fVal[t]); }
        snprintf(path, sizeof path, "%s/summary.txt", outDir);
        if ((fSum = fopen(path, "w")) == NULL) { fprintf(stderr, "sai_check: cannot write %s\n", path); return 1; }
        fprintf(fSum, "source %s\n", M.source);
        for (f = 0; f < nFiles; f++) fprintf(fSum, "file %d %s\n", f + 1, files[f]);
        for (t = 0; t < M.nTx; t++) {
            fprintf(fSum, "count %s", M.tx[t].name);
            for (f = 0; f < nFiles; f++) fprintf(fSum, " %lld", perFile[t][f]);
            fprintf(fSum, "\n");
            if (M.tx[t].isGNSS)
                fprintf(fSum, "thr %s %.17g %.17g %.17g\n", M.tx[t].name, M.tx[t].thrRCG[0], M.tx[t].thrRCG[1], M.tx[t].thrRCG[2]);
        }
        fclose(fSum);
    }
    saiFree(&M);
    return 0;
}
