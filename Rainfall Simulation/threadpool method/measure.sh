#! /bin/bash

GREEN="\e[1;32m"
RED="\e[0;31m"
NC="\e[m"

echo -e "${GREEN}=== Running dimension 4096 with sequential version ===${NC}"
./rainfall_seq 1 50 0.5 4096 measurement_4096x4096.in > ms_base
echo -e "${GREEN}=== Checking correctness(seq) ===${NC}"
./check.py 4096 measurement_4096x4096.out ms_base

for cnt in 1 2 4 8; do
    echo -e "${GREEN}=== Running dimension 4096 with parallel version(pt, ${cnt}) ===${NC}"
    ./rainfall_pt $cnt 50 0.5 4096 measurement_4096x4096.in > ms_pt_$cnt
    echo -e "${GREEN}=== Checking correctness(pt, $cnt) ===${NC}"
    ./check.py 4096 measurement_4096x4096.out ms_pt_$cnt
done

for cnt in 1 2 4 8; do
    echo -e "${GREEN}=== Running dimension 4096 with parallel version(th, ${cnt}) ===${NC}"
    ./rainfall_th $cnt 50 0.5 4096 measurement_4096x4096.in > ms_th_$cnt
    echo -e "${GREEN}=== Checking correctness(th, $cnt) ===${NC}"
    ./check.py 4096 measurement_4096x4096.out ms_tp_$cnt
done

