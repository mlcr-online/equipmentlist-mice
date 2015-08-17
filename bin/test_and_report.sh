export host=`hostname -s`
./daqtest_${host}.sh >${host}testlog_short.txt

mail -a ${host}testlog.txt -s "DAQ readout test from "${host} Yordan.Karadzhov@cern.ch \
                                                              E.Overton@sheffield.ac.uk \
                                                              Francois.Drielsma@gmail.com \
                                                              MICE-OM@stfc.ac.uk \
                                                              S.B.Boyd@warwick.ac.uk < ${host}testlog_short.txt

rm ${host}testlog_short.txt ${host}testlog.txt
