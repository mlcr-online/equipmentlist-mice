#! /bin/bash

HNS=`hostname -s`
LOG_FILE=$HNS"testlog.txt"
ERR=0

if [ "$(/sbin/pidof readout)" ] 
then
  echo "Data readout is in progres..."
  echo "exit"
  echo "Data readout is in progres...">$LOG_FILE
  echo "exit" >>$LOG_FILE
  exit
fi

echo
echo "**************************************************"
echo "Start of the test:" `date`
echo "host:" $HNS
echo "Start of the test:" `date` >$LOG_FILE
echo "host:" $HNS >>$LOG_FILE
echo "**************************************************"

function testEq {
err=0
echo    $HNS " --- Testing" $1 "(BA:" $2")" >>$LOG_FILE
echo -n $HNS " --- Testing" $1 "(BA:" $2")  /"
for i in `seq 0 9`;
do
    ./equipmentTest $1 $2 >>$LOG_FILE
    if [ $? -eq 0 ]; then
        echo -n "x"
        ERR=$(($ERR+1))
        err=$(($err+1))        
    else
        echo -n "-"
    fi
done

if [ $err -eq 0 ]; then
  echo "/  All tests OK"
else
  echo "/ " $err "tests FAILED"
fi
}

function endOfTests {
echo "**************************************************"
echo "End of the test:" `date`
echo -n "host:" $HNS

echo "End of the test:" `date` >>$LOG_FILE
echo -n "host:" $HNS >>$LOG_FILE

if [ $ERR -eq 0 ]; then
  echo "  -->  All tests OK"
  echo "  -->  All tests OK" >>$LOG_FILE
else
  echo "  --> " $ERR "tests FAILED!"
  echo "  --> " $ERR "tests FAILED!" >>$LOG_FILE
fi

echo "**************************************************"
}

