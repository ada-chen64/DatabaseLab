#!/bin/bash

: ${CURRENT_LAB:=1}
: ${SEND_RESULT:=0}

mkdir build 
cd build 
cmake ..
make -j 16 -l 32

echo -e current project "\x1b[32m${CI_PROJECT_NAME}\x1b[0m"
echo -e pipeline id "\x1b[32m${CI_PIPELINE_ID}\x1b[0m"

echo -e "\x1b[1;32mStarting testing Lab${CURRENT_LAB}\x1b[0m"
bin/thdb_init
test/lab${CURRENT_LAB}_test --gtest_output="json" --gtest_brief=1 || fail=1
bin/thdb_clear

if [ $SEND_RESULT -eq 1 -a -e report.json ]
then
    echo finish send result here
fi

if [ $fail -eq 1 ]
then
    exit 1
fi
 
echo ALL PASSED
