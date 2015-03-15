#! /bin/bash

#cd /dateSite/EMCal-KL /
source daqtest_def.sh

#I/O register
testEq V977  21020000

# KL fADCs
testEq V1724 21030000
testEq V1724 21040000
testEq V1724 21050000
testEq V1724 21060000
testEq V1724 21070000
testEq V1724 21080000

# TOF2 fADCs
testEq V1724 21090000
testEq V1724 210A0000
testEq V1724 210B0000
testEq V1724 210C0000
testEq V1724 210D0000

endOfTests




