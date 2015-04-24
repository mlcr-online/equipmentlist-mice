# miceacq14
scp ../bin/readout                              miceacq14:/dateSite/Central-Trigger/
scp ../bin/daqtest_def.sh                       miceacq14:/dateSite/Central-Trigger/
scp ../bin/daqtest_miceacq14.sh                 miceacq14:/dateSite/Central-Trigger/
scp ../bin/equipmentTest                        miceacq14:/dateSite/Central-Trigger/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq14:$DATE_SITE_CONFIG

#miceacq15
scp ../bin/readout                              miceacq15:/dateSite/EMCal-KL/
scp ../bin/daqtest_def.sh                       miceacq15:/dateSite/EMCal-KL/
scp ../bin/daqtest_miceacq15.sh                 miceacq15:/dateSite/EMCal-KL/
scp ../bin/equipmentTest                        miceacq15:/dateSite/EMCal-KL/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq15:$DATE_SITE_CONFIG

#miceacq17
scp ../bin/readout                              miceacq16:/dateSite/Tracker1/
scp ../bin/daqtest_def.sh                       miceacq16:/dateSite/Tracker1/
#scp ../bin/daqtest_miceacq15.sh                 miceacq16:/dateSite/Tracker1/
scp ../bin/equipmentTest                        miceacq16:/dateSite/Tracker1/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq16:$DATE_SITE_CONFIG

#miceacq20
scp ../bin/readout                              miceacq20:/dateSite/EMCal-EMR/
scp ../bin/daqtest_def.sh                       miceacq20:/dateSite/EMCal-EMR/
scp ../bin/daqtest_miceacq20.sh                 miceacq20:/dateSite/EMCal-EMR/
scp ../bin/equipmentTest                        miceacq20:/dateSite/EMCal-EMR/
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceacq20:$DATE_SITE_CONFIG

#miceraid4
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceraid4:$DATE_SITE_CONFIG

#miceraid5
scp ../src/epicsinterface/DAQPVClientsMap.txt   miceraid5:$DATE_SITE_CONFIG
