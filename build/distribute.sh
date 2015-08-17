echo `hostname -s`":"${PWD}>ORIGIN

# miceacq14
scp ../bin/readout                              miceacq14:/dateSite/Central-Trigger/
scp ../bin/daqtest_def.sh                       miceacq14:/dateSite/Central-Trigger/
scp ../bin/daqtest_miceacq14.sh                 miceacq14:/dateSite/Central-Trigger/
scp ../bin/test_and_report.sh                   miceacq14:/dateSite/Central-Trigger/                  
scp ../bin/equipmentTest                        miceacq14:/dateSite/Central-Trigger/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq14:$DATE_SITE_CONFIG
scp ORIGIN                                      miceacq14:/dateSite/Central-Trigger/

#miceacq15
scp ../bin/readout                              miceacq15:/dateSite/EMCal-KL/
scp ../bin/daqtest_def.sh                       miceacq15:/dateSite/EMCal-KL/
scp ../bin/daqtest_miceacq15.sh                 miceacq15:/dateSite/EMCal-KL/
scp ../bin/test_and_report.sh                   miceacq15:/dateSite/EMCal-KL/
scp ../bin/equipmentTest                        miceacq15:/dateSite/EMCal-KL/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq15:$DATE_SITE_CONFIG
scp ORIGIN                                      miceacq15:/dateSite/EMCal-KL/

#miceacq16
scp ../bin/readout                              miceacq16:/dateSite/Tracker1/
scp ../bin/daqtest_def.sh                       miceacq16:/dateSite/Tracker1/
scp ../bin/daqtest_miceacq16.sh                 miceacq16:/dateSite/Tracker1/
scp ../bin/test_and_report.sh                   miceacq16:/dateSite/Tracker1/
scp ../bin/equipmentTest                        miceacq16:/dateSite/Tracker1/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq16:$DATE_SITE_CONFIG
scp ORIGIN                                      miceacq16:/dateSite/Tracker1/

#miceacq17
scp ../bin/readout                              miceacq17:/dateSite/Tracker2/
scp ../bin/daqtest_def.sh                       miceacq17:/dateSite/Tracker2/
scp ../bin/daqtest_miceacq17.sh                 miceacq17:/dateSite/Tracker2/
scp ../bin/test_and_report.sh                   miceacq17:/dateSite/Tracker2/
scp ../bin/equipmentTest                        miceacq17:/dateSite/Tracker2/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq17:$DATE_SITE_CONFIG
scp ORIGIN                                      miceacq17:/dateSite/Tracker2/

#miceacq20
scp ../bin/readout                              miceacq20:/dateSite/EMCal-EMR/
scp ../bin/daqtest_def.sh                       miceacq20:/dateSite/EMCal-EMR/
scp ../bin/daqtest_miceacq20.sh                 miceacq20:/dateSite/EMCal-EMR/
scp ../bin/test_and_report.sh                   miceacq20:/dateSite/EMCal-EMR/
scp ../bin/equipmentTest                        miceacq20:/dateSite/EMCal-EMR/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq20:$DATE_SITE_CONFIG
scp ORIGIN                                      miceacq20:/dateSite/EMCal-EMR/

#miceraid4
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceraid4:$DATE_SITE_CONFIG

#miceraid5
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceraid5:$DATE_SITE_CONFIG
