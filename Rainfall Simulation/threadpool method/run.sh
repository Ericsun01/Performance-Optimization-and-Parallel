#! /bin/bash

GREEN="\e[1;32m"
RED="\e[0;31m"
NC="\e[m"

suffix=$1
mode=$2
cnt=$3

if [[ $cnt == "" ]] ; then
    cnt = 1
fi

if [[ $mode != "pt" ]] ; then
    echo -e "${GREEN}=== Running dimension 4 ===${NC}"
    ./rainfall_seq 1 10 0.25 4 sample_4x4.in         > out4$suffix

    echo -e "${GREEN}=== Running dimension 16 ===${NC}"
    ./rainfall_seq 1 20 0.5 16 sample_16x16.in       > out16$suffix

    echo -e "${GREEN}=== Running dimension 32 ===${NC}"
    ./rainfall_seq 1 20 0.5 32 sample_32x32.in       > out32$suffix

    echo -e "${GREEN}=== Running dimension 128 ===${NC}"
    ./rainfall_seq 1 30 0.25 128 sample_128x128.in   > out128$suffix

    echo -e "${GREEN}=== Running dimension 512 ===${NC}"
    ./rainfall_seq 1 30 0.75 512 sample_512x512.in   > out512$suffix

    echo -e "${GREEN}=== Running dimension 2048 ===${NC}"
    ./rainfall_seq 1 35 0.5 2048 sample_2048x2048.in > out2048$suffix

    #./rainfall_seq 1 50 0.5 4096 measurement_4096x4096.in > out4096
else
    echo -e "${GREEN}=== Running dimension 4(pt) ===${NC}"
    ./rainfall_pt $cnt 10 0.25 4 sample_4x4.in         > out4$suffix

    echo -e "${GREEN}=== Running dimension 16(pt) ===${NC}"
    ./rainfall_pt $cnt 20 0.5 16 sample_16x16.in       > out16$suffix

    echo -e "${GREEN}=== Running dimension 32(pt) ===${NC}"
    ./rainfall_pt $cnt 20 0.5 32 sample_32x32.in       > out32$suffix

    echo -e "${GREEN}=== Running dimension 128(pt) ===${NC}"
    ./rainfall_pt $cnt 30 0.25 128 sample_128x128.in   > out128$suffix

    echo -e "${GREEN}=== Running dimension 512(pt) ===${NC}"
    ./rainfall_pt $cnt 30 0.75 512 sample_512x512.in   > out512$suffix

    echo -e "${GREEN}=== Running dimension 2048(pt) ===${NC}"
    ./rainfall_pt $cnt 35 0.5 2048 sample_2048x2048.in > out2048$suffix

    #./rainfall_pt $cnt 50 0.5 4096 measurement_4096x4096.in > out4096
fi

for d in 4 16 32 128 512 2048; do
    echo -e "${GREEN}=== Checking dimension ${d} ===${NC}"
    ./check.py $d sample_${d}x${d}.out out${d}${suffix}
done

