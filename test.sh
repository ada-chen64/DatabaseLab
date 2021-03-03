#!/bin/bash

: ${LAB_UNTIL:=1}
: ${SEND_RESULT:=0}

mkdir build 
cd build 
cmake ..
make

echo current project ${CI_PROJECT_NAME}
echo pipeline id ${CI_PIPELINE_ID}

for (( lab = 1; lab <= ${LAB_UNTIL}; lab++))
do
    echo "Testing Lab${lab}"
    bin/thdb_init
    test/lab${lab}_test --gtest_output="json" --gtest_brief=1 || fail=1
    bin/thdb_clear
done

if [ $SEND_RESULT -eq 1 -a -e report.json ]
then
    echo finish send result here
fi

if [ $fail -eq 1]
then
    exit 1
fi
 
echo ALL PASSED