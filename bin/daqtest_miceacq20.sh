#! /bin/bash

cd /dateSite/EMCal-EMR/
source daqtest_def.sh

#I/O register
testEq V977 e0020000

# VRBs
testEq VRB 11FF0000
testEq VRB 22FF0000
testEq VRB 33FF0000
testEq VRB 44FF0000
testEq VRB 55FF0000
testEq VRB 66FF0000
testEq VRB 77FF0000
testEq VRB 88FF0000

# VCBs
testEq VCB e00a0000
testEq VCB e00b0000
testEq VCB e00c0000

# EMR fADCs
# In case the HV of the PMTs is ON
export ZSThresholdV1731=1000

# In case the HV of the PMTs is OFF
#export ZSThresholdV1731=3

testEq V1731 e00e0000
testEq V1731 e00f0000
testEq V1731 e0100000
testEq V1731 e0110000
testEq V1731 e0120000
testEq V1731 e0130000

endOfTests

cd -


