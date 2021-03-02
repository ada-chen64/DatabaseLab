#!/bin/bash

: ${LAB_UNTIL:=1}

mkdir build 
cd build 
cmake ..
make


for (( lab = 1; lab <= ${LAB_UNTIL}; lab++))
do
    echo "Testing Lab${lab}"
    ./tests --gtest_filter=Lab${lab}*
done
 
echo ALL PASSED