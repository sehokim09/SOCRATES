#ifndef SOCRATES_PROPAGATOR_H
#define SOCRATES_PROPAGATOR_H

/***************************************************************************/
/*                      SOCRATES orbit propagator                          */
/*                                                                         */
/*  Unified propagator interface. All outputs are position/velocity in the */
/*  J2000/GCRF inertial frame (the same "N" frame 42 uses internally),     */
/*  in km and km/s.                                                        */
/*                                                                         */
/*  Two backends:                                                          */
/*    PROP_SGP4    - transmitters. Propagated from a *time series* of TLEs */
/*                   with the vendored Vallado SGP4/SDP4 (orbit/sgp4).      */
/*                   SGP4 works in TEME; converted here to J2000.           */
/*    PROP_NUMERIC - receivers. Numerical (Cowell) integration of the      */
/*                   Keplerian-designed orbit, reusing 42's force models.   */
/*                                                                         */
/*  Time convention: evaluation time is SOCRATES DynTime (TT seconds since */
/*  J2000). Frame/time conversions reuse 42's timekit + HiFiEarthPrecNute  */
/*  (GMST / IAU-76 precession-nutation, no Earth Orientation Parameters).  */
/***************************************************************************/

enum PropBackend { PROP_SGP4 = 0, PROP_NUMERIC = 1 };

/* One propagated object (transmitter or receiver). Backend handles are
   opaque so this header stays free of 42 / SGP4 macro pollution. */
typedef struct PropObject {
   int   backend;   /* PROP_SGP4 or PROP_NUMERIC                       */
   void *sgp4;      /* opaque TLE-series handle  (sgp4_backend.h)      */
   void *numeric;   /* opaque numeric-propagator handle               */
} PropObject;

/* -- Time helpers ------------------------------------------------------ */

/* SOCRATES DynTime (TT seconds since J2000) -> UTC Julian date.
   UTC = TT - 32.184 s - LeapSec. */
double PropDynTimeToUtcJD(double dynTimeTT);

/* UTC Julian date -> Unix epoch milliseconds (UTC). Matches the vendored
   TLE.c epoch convention (JD 2440587.5 = 1970-01-01T00:00:00Z). */
double PropUtcJDToUnixMillis(double jdUTC);

/* -- Frame conversion -------------------------------------------------- */

/* TEME (km, km/s) at UTC Julian date jdUTC -> J2000/GCRF (km, km/s).
   Reuses 42's HiFiEarthPrecNute (precession + nutation; no polar motion or
   UT1-UTC). Velocity uses the same rotation (precession-nutation rate
   neglected, consistent with the no-EOP choice). */
void PropTemeToJ2000(double jdUTC,
                     const double rTeme[3], const double vTeme[3],
                     double rJ2000[3], double vJ2000[3]);

/* -- Unified evaluation (implemented with the backends) ---------------- */

/* Evaluate object at SOCRATES DynTime (TT). Writes J2000 r,v (km, km/s).
   Returns 0 on success, nonzero on error. */
int PropEval(PropObject *obj, double dynTimeTT,
             double rJ2000[3], double vJ2000[3]);

#endif /* SOCRATES_PROPAGATOR_H */
