#!/bin/bash

: ${CURRENT_LAB:=0}

mkdir build 
cd build 
cmake ..
make -j 16 -l ${MAX_LOAD_AVERAGE:-32}

echo -e current project "\x1b[32m${CI_PROJECT_NAME}\x1b[0m"
echo -e pipeline id "\x1b[32m${CI_PIPELINE_ID}\x1b[0m"
echo -e job id "\x1b[32m${CI_JOB_ID}\x1b[0m"

if [ $CURRENT_LAB -eq 0 ]
then
    exit 0
fi

echo -e "\x1b[1;32mStarting test Lab${CURRENT_LAB}\x1b[0m"
test/lab${CURRENT_LAB}_test --gtest_output="json:report.json" --gtest_color="yes" || fail=1

if [ ${fail:-0} -eq 1 ]
then
    exit 1
fi
 
echo -e "\x1b[1;32mLab${CURRENT_LAB} test passed\x1b[0m"

if [ ${SEND_RESULT:-0} -eq 1 -a -e report.json ]
then
    curl -s -F "project=${CI_PROJECT_NAME}" -F "pipeline=${CI_PIPELINE_ID}" \
     -F "job=${CI_JOB_ID}" -F "lab=${CURRENT_LAB}" -F "file=@report.json" \
     172.6.31.11:9876/collect/ || { echo -e "\x1b[1;31mFailed to send result\x1b[0m"; exit 1; }
    echo  -e "\x1b[1;32mLabfinished sending result\x1b[0m"
fi