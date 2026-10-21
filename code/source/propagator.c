/***************************************************************************/
/*                      SOCRATES orbit propagator                          */
/*                                                                         */
/*  Time and frame foundation shared by both propagator backends.          */
/*  See code/include/propagator.h for the interface contract.              */
/***************************************************************************/

#include <math.h>
#include "42.h"           /* DynTime, UTC, TT, LeapSec, timekit, mathkit, envkit */
#include "propagator.h"

/* --------------------------------------------------------------------- */
/*  Time helpers                                                          */
/* --------------------------------------------------------------------- */

/* TT (seconds since J2000) -> UTC Julian date.
   TT = TAI + 32.184 s,  UTC = TAI - LeapSec  =>  UTC = TT - 32.184 - LeapSec. */
double PropDynTimeToUtcJD(double dynTimeTT)
{
   double utcSec = dynTimeTT - 32.184 - LeapSec;
   return TimeToJD(utcSec);
}

/* UTC Julian date -> Unix epoch milliseconds (UTC).
   JD 2440587.5 = 1970-01-01T00:00:00Z; 86400000 ms per day.  This matches the
   epoch convention used by the vendored TLE.c (getRVForDate). */
double PropUtcJDToUnixMillis(double jdUTC)
{
   return (jdUTC - 2440587.5) * 86400000.0;
}

/* --------------------------------------------------------------------- */
/*  Frame conversion: TEME -> J2000/GCRF                                  */
/* --------------------------------------------------------------------- */
/*
   42 DCM convention is v_A = C_A_B * v_B (second subscript is the source frame).
   HiFiEarthPrecNute returns:
       C_TEME_TETE : v_TEME  = C_TEME_TETE  * v_TETE     (TETE  -> TEME)
       C_TETE_J2000: v_TETE  = C_TETE_J2000 * v_J2000    (J2000 -> TETE)
   To go TEME -> J2000 we apply the transposes in reverse:
       v_TETE  = C_TEME_TETE^T  * v_TEME
       v_J2000 = C_TETE_J2000^T * v_TETE
   MTxV(M,V,Vout) computes Vout = M^T * V.
*/
void PropTemeToJ2000(double jdUTC,
                     const double rTeme[3], const double vTeme[3],
                     double rJ2000[3], double vJ2000[3])
{
   double C_TEME_TETE[3][3], C_TETE_J2000[3][3];
   double rin[3], vin[3], rTete[3], vTete[3];
   int i;

   for (i = 0; i < 3; i++) { rin[i] = rTeme[i]; vin[i] = vTeme[i]; }

   HiFiEarthPrecNute(jdUTC, C_TEME_TETE, C_TETE_J2000);

   /* position */
   MTxV(C_TEME_TETE,  rin,   rTete);
   MTxV(C_TETE_J2000, rTete, rJ2000);

   /* velocity (same rotation; precession-nutation rate neglected) */
   MTxV(C_TEME_TETE,  vin,   vTete);
   MTxV(C_TETE_J2000, vTete, vJ2000);
}
