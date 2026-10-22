# SAI observation model: validation

Validation of the SOCRATES implementation of the observation tests of the
ASR-Design manuscript (`code/source/sai.c`, used by `readSpDataNc` in
`design.c`) against the MATLAB reference evaluator `single_ant_model.m`.
Plan: `ASR_SAI_Design/docs/SOCRATES_SAI_update.md`, Section 7.

## Summary

| Check | Result |
|---|---|
| 1. Unit checks (`sai_check --selftest`) | all pass |
| 2. Per-point agreement with MATLAB, pooled 7-file sample | **0 mismatches** in 149,736,297 points x 3 ports; P identical, beta/alpha to 3.3e-14 |
| 3. Pass fractions against `evaluation_summary.m` | all 14 marginal values reproduced to 4 decimals; joint H&V within 0.0007 of min(vX, vY) |
| 4. Constellation run | 1-receiver 50 deg run: within 0.004 of `payload.passByOrbit`; `createFoM.m` runs on the new file. 6/500/50/1 on the server: see 4c |

## Model and inputs

- Payload: `solved_payload.m` of 2026-10-21 14:07 (`GNSS_EIRP_MODE = "margin"`, 1 dB,
  `GNSS_SIG = [1575.42 26.934]`, `N_chan = [2 2 1 1 1 1 1]`), RX_POL = "HVC".
- `sim/SAI-2610/Inp_SAI.txt` and `reflectivity_cdf_{X,Y,L}.txt`, written by
  `ASR/code/payload/export_socrates_inputs.m` from the `single_ant_model.m` workspace,
  numbers in `%.17g` (exact doubles); pattern files in `ant_pat/sai/` and `ant_pat/sai/cp/`.
- The evaluator was run in a scratch copy of the ASR tree so that its own outputs
  (`evaluation_summary.m`, `manuscript/images/reflectivity_CDFs.*`) did not touch the
  workspace; the regenerated `evaluation_summary.m` is identical to the workspace copy.
- Branch `feature/sai-manuscript-model` from `master` 47d2ac3:

  | Commit | Content |
  |---|---|
  | 3941cbd | `Inp_SAI.txt`, CDF tables, pattern files |
  | 6ae94a9 | `sai.c`, `sai.h` |
  | d92e5a7 | `design.c` integration |
  | 130c45d | outputs (`obsFreqL`, counters, run tag), `createFoM.m` tag argument, `SAI-2610` project |
  | b339140 | `sai_check` |

Derived values, recomputed independently from the manuscript expressions:
eta_HV = 0.051975, eta_L = 0.029550; S0 within 4e-6 of the Section 2.1 formula
(GLONASS 3.7e-5, because the evaluator recomputes T_sky at 1602 MHz while
`solved_payload.m` lists one T_n for all GNSS).

## 1. Unit checks

`./sai_check --selftest sim/SAI-2610/Inp_SAI.txt ./ant_pat/ cdf.csv`

| Check | Result |
|---|---|
| Test 1 at kappa = -1 against `alpha < (eta_bar Nc - 3) S0 / (4 W_D)` | expressions agree to 1.4e-16; pass/fail flips at alpha_th (1 -/+ 1e-9) on all 9 SAI port scorings |
| rho1 rho2 = N_D^2 | 13,530 points past Test 1; worst relative error 2.7e-9, 0.20 of the limit 1e-12 + eps 4C^2/N_D^2 (see note) |
| Step CDF lookup | 0 of 12,000 differ from the definition (per port and column: 50 below, 50 above, 100 on a step, 800 inside); **0 of 12,000 differ from MATLAB `interp1(Xu, Fu, x, 'previous', 'extrap')`** with F = 0 below the table |
| Gcp from the pattern integral | within 0.00005 dB of `saiAnt.G_cp_dB`: 2.854857, 3.432698, 3.879672, 6.894960 dBi |

Note on rho1 rho2: `rho2 = C - sqrt(C^2 - N_D^2)` loses digits to cancellation when
C >> N_D (MUOS, C ~ 4000, gives about 1e-9). `single_ant_model.m` uses this form and
`sai.c` keeps it so the two agree; a limit of 1e-12 alone cannot be met by it.

## 2. Per-point agreement with the MATLAB reference

C: `sai_check --alias UFO=MUOS --out DIR ...` (pooled in the order of
`payload.geomFiles`). MATLAB: `single_ant_model; SAI_REF_OUT = DIR; export_sai_reference`.
Rows are identical in number and order (per-file counts equal for every transmitter).
MUOS rows are its spot-beam points only; UFO rows are all points on the MUOS orbits.

| Tx | points | X | Y | Lcp | H&V mismatches | kappa > -1 (C/M) | values where flags agree |
|---|---|---|---|---|---|---|---|
| ORBCOMM | 1,417,506 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | P identical |
| UFO | 5,026,198 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | P identical |
| MUOS | 3,410,706 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | P identical |
| GPS | 31,012,772 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | beta/alpha 3.3e-14, threshold 1.4e-16 |
| Galileo | 28,756,947 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | beta/alpha 3.3e-14, threshold identical |
| GLONASS | 25,834,388 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | beta/alpha 3.3e-14, threshold 1.4e-16 |
| Beidou | 54,277,780 | 100% (0) | 100% (0) | 100% (0) | 0 | 0/0 | beta/alpha 3.3e-14, threshold identical |

No flag differs, so there is no mismatch to explain; the anticipated causes (ties in
the nearest-neighbour rounding, a CDF value exactly on a step) did not occur.
F^-1(1 - gamma) at 1575 MHz is 0.00443718 (X), 0.02796671 (Y), 0.03043024 (L) in both.

## 3. Pass fractions against `evaluation_summary.m`

| | ORBCOMM | UFO | MUOS | GPS | Galileo | GLONASS | Beidou |
|---|---|---|---|---|---|---|---|
| min(vX, vY), C | 0.9336 | 0.8254 | 0.9206 | 0.4053 | 0.6374 | 0.1763 | 0.2030 |
| `evalSum.HV_hi` | 0.9336 | 0.8254 | 0.9206 | 0.4053 | 0.6374 | 0.1763 | 0.2030 |
| joint H&V, C | 0.9336 | 0.8254 | 0.9199 | 0.4053 | 0.6374 | 0.1763 | 0.2030 |
| `evalSum.HV_lo` | 0.9324 | 0.8254 | 0.9199 | 0.4053 | 0.6374 | 0.1485 | 0.1649 |
| LHCP product, C | 0.9695 | 1.0000 | 1.0000 | 1.0000 | 1.0000 | 0.7378 | 0.7492 |
| `evalSum.LHCP` | 0.9695 | 1.0000 | 1.0000 | 1.0000 | 1.0000 | 0.7378 | 0.7492 |

The joint H&V fraction is at most min(vX, vY) and within 0.0007 of it (MUOS); for
the other six the weaker port's passes are a subset of the stronger port's.

## 4. Constellation run

### 4a. One receiver, 50 deg, 500 km (local)

`perf` on a 1-receiver constellation built from `6_500_50_1_Rx1.nc`; every point is
counted once (valid + invalid = points in the file, none off the grid).

| SoOp | H&V ratio | `passByOrbit(:,2,1)` | difference |
|---|---|---|---|
| ORBCOMM | 0.9291 | 0.9312 | -0.0021 |
| UFO | 0.8924 | 0.8917 | +0.0007 |
| MUOS | 0.9323 | 0.9336 | -0.0013 |
| GPS | 0.4359 | 0.4326 | +0.0033 |
| GE | 0.5522 | - | |
| GENB | 0.3570 | - | |

`passByOrbit` is the optimizer's worse-port marginal on one receiver; the C ratio is
the joint H&V. The H&V `obsFreq`, `revisitTime_*` and `accessTime*_*` outputs are
identical before and after the output changes of 130c45d.

### 4b. `createFoM.m`

`createFoM(projName, DesignParamsObj, "SAI2610")` (with `DesignParamsObj` from
`SAI-260316.smat`, lists reduced to the test case) reads the new
`*_perform_SAI2610.nc`, completes post-processing, and its `nSpRatio` equals the C
counters. `createFoM.m` needed one change: the optional `performTag` argument for the
new file name (default `"SAI"`, so existing calls are unchanged). Two prerequisites
outside this change: `sim/SAI-2610/out/Grid1_landmask.out` (not written by the C
code; copy it from `sim/SAI-260316/out/`), and a design grid with more than one case
(a 1 x 1 x 1 x 1 grid fails when the `.mat` checkpoint is indexed with 4 subscripts;
this predates the change).

### 4c. 6/500/50/1 (server)

`bash sim/SAI-2610/smoke_test.sh 6 500 50 1` on the machine holding the specular-point
files. Results to be added after the run. Acceptance: H&V ratio within 0.02 of
`passByOrbit(:,2,1)` = 0.9312, 0.8917, 0.9336, 0.4326 (ORBCOMM, UFO, MUOS, GPS).

## Reproducing

```
make sai_check
./sai_check --selftest sim/SAI-2610/Inp_SAI.txt ./ant_pat/ cdf.csv
./sai_check --alias UFO=MUOS --out DIR sim/SAI-2610/Inp_SAI.txt ./ant_pat/ <payload.geomFiles in order>
```

MATLAB, from `ASR/code/payload` (or a copy of it, to keep the workspace untouched):

```
single_ant_model; export_socrates_inputs          % regenerate the inputs
single_ant_model; SAI_REF_OUT = "DIR"; export_sai_reference
```

## Findings on the task specification

1. The base commit 47d2ac3 is `origin/master`; the local `master` was behind it.
2. The `.nc` files carry no units attributes. Values confirm metres and radians;
   `lookAngElD` runs 90 to 180 deg (1st to 99th percentile 90 to 144 deg), not 91 to 153.
3. In Test 2, "treat C^2 < N_D^2 as fail" cannot occur: Test 1 implies
   eta_bar N_c - kappa > 4 N_D, hence C > N_D. Kept as a guard.
4. The GNSS quantile also applies `unique(X, 'last')` before `gammaQuantile`'s
   `unique(F, 'first')`; the specification names only the second. Both are implemented
   (no effect on the current CDFs, which are strictly increasing).
5. `%.10e` does not round-trip a double; `%.17g` is used. N_c is not an exact integer in
   floating point (0.0009 x 25e3 x 2).
6. rho1 rho2 = N_D^2 to 1e-12 is not attainable with the MATLAB form (Section 1 note).
7. A per-point CSV of the pooled sample (about 150 M rows) is impractical; `sai_check`
   and `export_sai_reference` write binary per-point files.
8. `payload.passByOrbit` is the optimizer's worse-port marginal on one receiver and four
   bands, so it is not the same quantity as the joint H&V ratio, has no value for GE and
   GENB, and differs slightly from the evaluator (GPS 0.4068 vs 0.4053).
9. The performance file is written into the shared specular-point directory, so a new
   project alone does not protect the old `*_perform_SAI.nc`, and `checkPerformNc` would
   have skipped the run. Hence the run tag and `createFoM.m`'s `performTag`.
10. `createFoM.m`'s Hybrid branch looked for `_perform.nc`, which the C code never wrote
    (fixed with the tag).
11. Running `single_ant_model.m` writes `evaluation_summary.m` and
    `manuscript/images/reflectivity_CDFs.*`; `run_sai_workflow.m` also re-optimizes the
    payload. The `WF.EXPORT_SOCRATES` hook was therefore not added.
12. The `Inp_SAI.txt` example header shows 2026-10-20 22:15; the payload is 2026-10-21 14:07.
13. A project also needs `orbit/42input/<project>/`; `out/` is created by SOCRATES.
14. The per-SoOp Rx gain and pattern entries of `Inp_Design.txt` are no longer used (UFO
    pointed at CROSSDIPOLE_370, and the files were not in `ant_pat/`, so the old loader
    would have stopped). They are still parsed for the file format.
15. List values in `Inp_Design.txt` need a trailing `! comment` (the parser keeps the
    newline in the last token otherwise).
16. Not changed, for awareness: `griddata` is about 34 GB of virtual memory at
    `MAX_NSP` = 7000 and is zeroed on every slice; `freeDesign` is never called; the
    `SOCRATES` binary is tracked in git, so building in a checkout modifies it.
