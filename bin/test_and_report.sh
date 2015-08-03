export host=`hostname -s`
./daqtest_${host}.sh >${host}testlog_short.txt

mail -a ${host}testlog.txt -s "DAQ readout test from "${host} yordan.karadzhov@cern.ch < ${host}testlog_short.txt

rm ${host}testlog_short.txt ${host}testlog.txt
