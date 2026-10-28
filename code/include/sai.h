#ifndef SOCRATES_SAI_H
#define SOCRATES_SAI_H

/***************************************************************************/
/*                    SOCRATES SAI observation model                       */
/*                                                                         */
/*  Decides, for one specular point, whether it is a usable observation    */
/*  under the tests of the ASR-Design manuscript (Appendix A), exactly as  */
/*  the MATLAB evaluator single_ant_model.m does:                          */
/*    SAI  (ORBCOMM, UFO, MUOS)   Test 1 (minimum direct SNR) and Test 2   */
/*                                (effective reflectivity window)          */
/*    GNSS (GPS, Galileo, ...)    known-code RCG test                      */
/*  on three port scorings: linear X and Y at the H&V budget, and the      */
/*  synthesized LHCP port at the LHCP budget.                              */
/*                                                                         */
/*  Every payload and requirement number is read from the SAI Observation  */
/*  Model section of Inp_Design.txt, which createInpDesign writes from     */
/*  DesignParams.receiver.model (the Receiver Design step). The            */
/*  module has no SOCRATES or 42 dependency, so the standalone checker     */
/*  sai_check links it on its own.                                         */
/***************************************************************************/

#include <stddef.h>

#define SAI_MAX_TX   16
#define SAI_NSCORE   3          /* port scorings X, Y, Lcp */
#define SAI_NCDFCOL  4          /* 137, 255, 370, 1575 MHz */

/* Title of the Inp_Design.txt section that holds the SAI model */
#define SAI_SECTION_MARKER "SAI Observation Model"

/* Port scorings, also the bit positions of the saiEvaluate mask */
enum { SAI_SCORE_X = 0, SAI_SCORE_Y = 1, SAI_SCORE_LCP = 2 };

#define SAI_PASS_X       0x1
#define SAI_PASS_Y       0x2
#define SAI_PASS_LCP     0x4
#define SAI_KAPPA_GT_M1  0x8    /* diagnostic: SAI point with kappa > -1 */
#define SAI_PASS_HV      (SAI_PASS_X | SAI_PASS_Y)

/* One pattern file, FIELD amplitudes, v[iPhi*nTheta + iTheta],
   rows phi = 0:res:360 from +x, columns theta = 0:res:180 from nadir */
typedef struct {
    int nPhi, nTheta;
    double res;                 /* deg, derived as 180/(nTheta-1) */
    double *v;
} SaiGrid;

/* One antenna tag: dual-linear port files and circular (cp/) files */
typedef struct {
    char tag[40];
    SaiGrid gn[4];              /* XX, XY, YX, YY */
    SaiGrid cpL, cpR;           /* LHCP (co) and RHCP (cross) of the L port */
    double GcpPeak_dB;          /* peak co-polar directivity from the pattern integral */
} SaiPattern;

/* One port CDF table, F shared by the four frequency columns, plus the
   step-lookup arrays of MATLAB [Xu, iu] = unique(X(:,col), 'last') */
typedef struct {
    size_t n;
    double *F;
    double *X[SAI_NCDFCOL];
    size_t nu[SAI_NCDFCOL];
    double *Xu[SAI_NCDFCOL], *Fu[SAI_NCDFCOL];
} SaiCdf;

typedef struct {
    char name[20];
    int isGNSS, cdfCol, useSpotbeam;        /* cdfCol is 0-based */
    double S0, Nc, TI, Nincoh;
    double etaBarHV, etaBarL;               /* single-look budgets, from eta and Nincoh */
    double GlinPeak_dB;
    const SaiPattern *pat;
    double thrRCG[SAI_NSCORE];              /* GNSS threshold per scoring */
} SaiTx;

typedef struct {
    char source[128];                       /* payload file and timestamp */
    char runTag[64];
    double gamma, WD, etaHV, etaL, dirLinLoss_dB;
    SaiCdf cdf[SAI_NSCORE];                 /* ports X, Y, L; Lcp uses L */
    double Ginv[SAI_NSCORE];                /* F^-1(1-gamma) at 1575 MHz, per scoring */
    int nTx;
    SaiTx tx[SAI_MAX_TX];
    int nPat;
    SaiPattern pat[SAI_MAX_TX];
} SaiModel;

/* Optional per-point diagnostics, indexed by scoring */
typedef struct {
    double tauRD, kappa;                    /* SAI only */
    double G_D_dB[SAI_NSCORE], G_R_dB[SAI_NSCORE];
    double alpha[SAI_NSCORE], SD[SAI_NSCORE], beta[SAI_NSCORE];
    double rhoLo[SAI_NSCORE], rhoHi[SAI_NSCORE], P[SAI_NSCORE];
    int pass1[SAI_NSCORE];
    double ba[SAI_NSCORE], thr[SAI_NSCORE]; /* GNSS only */
} SaiDiag;

/* Read the SAI_SECTION_MARKER section of an Inp_Design.txt (CDF files
   resolved from the working directory, the SOCRATES root) and the pattern
   files under antRoot/<pattern subdirectory>. Returns 0 on success; on
   error prints the reason to stderr and returns nonzero. */
int saiLoad(const char *designPath, const char *antRoot, SaiModel *M);
void saiFree(SaiModel *M);

/* Transmitter entry for a SoOp: matched on the SoOp name first (ORBCOMM,
   UFO, MUOS), then on the Tx name (GNSS members of GPS, GE, GENB).
   NULL when neither is in the SAI section. */
const SaiTx *saiFindTx(const SaiModel *M, const char *soopName, const char *txName);

/* Evaluate one specular point. Ranges in metres, angles in radians in the
   nadir-referenced antenna frame, as stored in the specular-point files.
   Returns the SAI_PASS_* / SAI_KAPPA_GT_M1 bitmask. diag may be NULL. */
unsigned saiEvaluate(const SaiModel *M, const SaiTx *T,
                     double rangeTR, double rangeTS, double rangeSR,
                     double elD, double azD, double elR, double azR,
                     SaiDiag *diag);

/* Step CDF lookup, MATLAB interp1(Xu, Fu, x, 'previous', 'extrap') with
   F = 0 below the table. Exposed for the self-test. */
double saiStepF(const SaiCdf *C, int col, double x);

#endif /* SOCRATES_SAI_H */
