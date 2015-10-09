#! /bin/bash

cd /dateSite/Central-Trigger/
source daqtest_def.sh

# Trigger
testEq V1495 11020000

#I/O register
testEq V977  11150000

# scaler
testEq V830  11030000

# TDCs
testEq V1290 11060000
testEq V1290 11070000
testEq V1290 11080000
testEq V1290 11090000

# TOF fADCs
# In case the HV of the PMTs is ON
export ZSThresholdV1724=2000

# In case the HV of the PMTs is OFF
#export ZSThresholdV1724=30

testEq V1724 110A0000
testEq V1724 110B0000
testEq V1724 110C0000
testEq V1724 110D0000
testEq V1724 110E0000
testEq V1724 110F0000
testEq V1724 11100000
testEq V1724 11110000
testEq V1724 11120000

endOfTests

cd -


