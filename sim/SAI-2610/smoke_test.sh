#!/bin/bash
# Smoke test of the SAI observation model on one constellation
# (doc/SAI_model_validation.md, check 4). Run from the SOCRATES root on the
# machine that holds the specular-point files of the constellation:
#
#   bash sim/SAI-2610/smoke_test.sh [nRx alt inc nPlane]     (default 6 500 50 1)
#
# Builds SOCRATES and sai_check, runs the self-test, runs the performance
# analyzer ("perf") for that one constellation from a temporary copy of the
# SAI-2610 project, and prints the per-SoOp H&V pass ratio
# nSpValid/(nSpValid + nSpInvalid) with the per-port counters.
# The performance file is written next to the specular-point files as
# <cons>_perform_<run tag>.nc, the name the production run uses, so the
# production run skips this constellation afterwards.
set -euo pipefail

NRX=${1:-6}; ALT=${2:-500}; INC=${3:-50}; NPL=${4:-1}
PROJ=SAI-2610
TEST=${PROJ}-smoke
CONS=${NRX}_${ALT}_${INC}_${NPL}
ROOT=$(awk '/Large File Storage Root Directory/ {print $1}' sim/$PROJ/Inp_Design.txt)
TAG=$(awk '/Run tag/ {print $1}' sim/$PROJ/Inp_Design.txt)
NP=${NP:-1}

for i in $(seq 1 "$NRX"); do
    f=$ROOT$CONS/${CONS}_Rx$i.nc
    [ -f "$f" ] || { echo "missing $f"; exit 1; }
done

make SOCRATES sai_check
./sai_check --selftest sim/$PROJ/Inp_Design.txt ./ant_pat/

# temporary project: SAI-2610 with the design lists reduced to this constellation
rm -rf "sim/$TEST" "orbit/42input/$TEST"
cp -R "sim/$PROJ" "sim/$TEST"
cp -R "orbit/42input/$PROJ" "orbit/42input/$TEST"
trap 'rm -rf "sim/$TEST" "orbit/42input/$TEST"' EXIT
awk -v a="$NRX" -v b="$ALT" -v c="$INC" -v d="$NPL" '
    val != "" { printf "%-25s ! smoke test\n", val; val = ""; next }
    /! Number of # Receiver Variable/     { print "1                         ! Number of # Receiver Variable"; val = a; next }
    /! Number of Altitude Variable/       { print "1                         ! Number of Altitude Variable"; val = b; next }
    /! Number of Inclination Variable/    { print "1                         ! Number of Inclination Variable"; val = c; next }
    /! Number of # Orbital Plane Variable/ { print "1                         ! Number of # Orbital Plane Variable"; val = d; next }
    { print }' "sim/$PROJ/Inp_Design.txt" > "sim/$TEST/Inp_Design.txt"

mpirun -np "$NP" ./SOCRATES "$TEST" perf

F=$ROOT$CONS/${CONS}_perform_$TAG.nc
get() { ncdump -v "$1" "$F" | sed -n "s/^ *$1 = \([0-9-]*\) ;.*/\1/p"; }
echo
echo "Performance file: $F"
ncdump -h "$F" | grep -E ':sai(Source|RunTag)'
printf "%-8s %10s %10s %8s %8s %8s %8s %6s %8s\n" SoOp valid invalid "H&V" "pass X" "pass Y" LHCP "k>-1" offgrid
for s in ORBCOMM UFO MUOS GPS GE GENB; do
    v=$(get nSpValid_$s); i=$(get nSpInvalid_$s)
    x=$(get nSpPassX_$s); y=$(get nSpPassY_$s); l=$(get nSpPassLcp_$s)
    k=$(get nSpKappa_$s); o=$(get nSpOffGrid_$s)
    awk -v s="$s" -v v="$v" -v i="$i" -v x="$x" -v y="$y" -v l="$l" -v k="$k" -v o="$o" 'BEGIN {
        n = v + i; printf "%-8s %10d %10d %8.4f %8.4f %8.4f %8.4f %6d %8d\n", s, v, i, v/n, x/n, y/n, l/n, k, o }'
done
if [ "$CONS" = "6_500_50_1" ]; then
    echo "payload.passByOrbit(:,2,1) (50 deg, 500 km; ORBCOMM UFO MUOS GPS): 0.9312 0.8917 0.9336 0.4326; tolerance 0.02"
fi
