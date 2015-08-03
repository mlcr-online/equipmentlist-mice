#! /bin/bash

#cd /dateSite/EMCal-KL /
source daqtest_def.sh

#I/O register
testEq V977  21020000

# KL fADCs
testEq V1724 210E0000
testEq V1724 210F0000
testEq V1724 21050000
testEq V1724 21060000
testEq V1724 21070000
testEq V1724 21080000

# TOF2 fADCs
# In case the HV of the PMTs is ON
export ZSThresholdV1724=200

# In case the HV of the PMTs is OFF
#export ZSThresholdV1724=30

testEq V1724 21090000
testEq V1724 210A0000
testEq V1724 210B0000
testEq V1724 210C0000
testEq V1724 210D0000

# CKOV fADC
# In case the HV of the PMTs is ON
export ZSThresholdV1731=100

# In case the HV of the PMTs is OFF
#export ZSThresholdV1731=3

testEq V1731 21110000

endOfTests




