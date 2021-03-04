#!/bin/bash

: ${CURRENT_LAB:=0}
: ${SEND_RESULT:=0}

mkdir build 
cd build 
cmake ..
make -j 16 -l 32

echo -e current project "\x1b[32m${CI_PROJECT_NAME}\x1b[0m"
echo -e pipeline id "\x1b[32m${CI_PIPELINE_ID}\x1b[0m"

if [ $CURRENT_LAB -eq 0 ]
then
    exit 0
fi

echo -e "\x1b[1;32mStarting test Lab${CURRENT_LAB}\x1b[0m"
test/lab${CURRENT_LAB}_test --gtest_output="json" --gtest_color="yes" || fail=1

if [ $SEND_RESULT -eq 1 -a -e report.json ]
then
    echo finish send result here
fi

if [ ${fail:-0} -eq 1 ]
then
    exit 1
fi
 
echo -e "\x1b[1;32mLab${CURRENT_LAB} test passed\x1b[0m"
