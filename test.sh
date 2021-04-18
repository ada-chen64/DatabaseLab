#!/bin/bash

: ${CURRENT_LAB:=0}

echo -e current project "\x1b[32m${CI_PROJECT_NAME}\x1b[0m"
echo -e pipeline id "\x1b[32m${CI_PIPELINE_ID}\x1b[0m"
echo -e job id "\x1b[32m${CI_JOB_ID}\x1b[0m"
echo -e CI_COMMIT_TIMESTAMP "\x1b[32m${CI_COMMIT_TIMESTAMP}\x1b[0m"
echo -e CI_PIPELINE_CREATED_AT "\x1b[32m${CI_PIPELINE_CREATED_AT}\x1b[0m"

echo -e "\x1b[1;32mStart to compile\x1b[0m"
mkdir build && cd build
cmake .. -D Lab=$CURRENT_LAB || { echo  -e "\x1b[1;31mFailed to cmake\x1b[0m" ; exit 1; }
make -j 16 -l ${MAX_LOAD_AVERAGE:-32} ||  { echo  -e "\x1b[1;31mFailed to make\x1b[0m" ; exit 1; }


if [ $CURRENT_LAB -eq 0 ]
then
    exit 0
fi

echo -e "\x1b[1;32mStart to test Lab${CURRENT_LAB}\x1b[0m"
test/lab${CURRENT_LAB}_test --gtest_output="json:report.json" --gtest_color="yes" || fail=1

if [ ${SEND_RESULT:-0} -eq 1 -a -e report.json ]
then
    echo -e "\x1b[1;32mStart to send result\x1b[0m" 
    curl -s -F "project=${CI_PROJECT_NAME}" -F "pipeline=${CI_PIPELINE_ID}" \
     -F "job=${CI_JOB_ID}" -F "lab=${CURRENT_LAB}" -F "file=@report.json" \
     172.6.31.11:9876/collect/ || { echo -e "\x1b[1;31mFailed to send result, please contact TA\x1b[0m"; exit 1; }
    echo  -e "\x1b[1;32mResult has been sent\x1b[0m"
fi

if [ ${fail:-0} -eq 0 ]
then
    echo -e "\x1b[1;32mPassed lab${CURRENT_LAB} test\x1b[0m"
else
    exit 1
fi
