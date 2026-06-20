# Vendored SGP4 (public domain)

Source: https://github.com/aholinch/sgp4  (directory `src/c/`)
License: The Unlicense (public domain) — see repository `LICENSE`.
Origin: faithful port of David Vallado's reference SGP4 ("SGP4 Version 2016-03-09",
revisiting Spacetrack Report #3 / AIAA 2006-6753).

Files vendored here:
- `SGP4.c`, `SGP4.h` — the SGP4/SDP4 propagator (deep-space path auto-selected
  internally when orbital period > 225 min, so MEO/GEO are handled by the same call).
- `TLE.c`, `TLE.h`  — TLE line parsing and convenience `getRV`/`getRVForDate` helpers.

Notes:
- `SGP4.c` was reconstructed from the upstream file; the trailing `jday()` function
  (the only function after `gstime`) is the standard Vallado implementation. Functional
  correctness is confirmed by the SGP4-VER.TLE verification vectors (see ../sgp4/data
  and the verification harness).
- Output frame is TEME (True Equator, Mean Equinox), units km and km/s.
  Conversion TEME -> inertial/ECEF is handled in the SOCRATES propagator layer.
- Do NOT edit these files to fit SOCRATES; keep them as a clean upstream snapshot and
  wrap them in the propagator interface instead.
