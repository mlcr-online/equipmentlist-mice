#include "equipmentList_common.hh"
#include "MiceDAQMessanger.hh"
#include "DAQManager.hh"
#include "EventBuildManager.hh"
#include "MDprocessor.h"
#include "MDfragmentDBB.h"
#include "MDpartEventV1731.h"

#include "MDprocessManager.h"
#include "DBBDataProcessor.hh"
#include "V1731DataProcessor.hh"

#include "DBBSpillData.hh"
#include "EMRDaq.hh"
#include "DAQData.hh"
#include "Spill.hh"

#include "TFile.h"
#include "TTree.h"

using namespace std;

MAUS::Spill                *spill;
MAUS::DAQData              *data;
// MAUS::EMRDaq               *emr;
MAUS::DBBArray             *dbb;
MAUS::V1731PartEventArray  *v1731spill;

DBBDataProcessor           *dbbProc;
V1731DataProcessor         *v1731Proc;

TFile *dataFile;
TTree *dataTree;

DAQManager        *myDAQ;
EventBuildManager *eventBuilder;
MiceDAQMessanger *messanger;

void start_of_run(int i) {
  *(messanger->getStream()) << "-----> \nThis is Start of Run " << i << endl;
  messanger->sendMessage(MDE_DEBUGGING);

  stringstream ss_rf;
  ss_rf << "../data_tmp/EMRdata_" << i << ".root";
//   cout << ss_rf.str() << endl;
  dataFile   = new TFile(ss_rf.str().c_str(),"RECREATE","EMR Cosmoc data");
  dataTree   = new TTree("EMRCosmicData", "EMR Cosmic test Data");

//   spill = new MAUS::Spill();
  data       = new MAUS::DAQData;
//   emr        = new MAUS::EMRDaq;
  dbb        = new MAUS::DBBArray;
  v1731spill = new MAUS::V1731PartEventArray;

  dbbProc->set_spill(dbb);
  v1731Proc->set_spill(v1731spill);

  dataTree->Branch("Spill", &data);
}

void end_of_run(int i) {
  *(messanger->getStream()) << "-----> \nThis is End of Run " << i;
  messanger->sendMessage(MDE_DEBUGGING);

  dataTree->Write();
  dataFile->Close();
}

void start_of_spill(int i) {
  *(messanger->getStream()) << "-----> \nThis is Start of Spill " << i << endl;
  messanger->sendMessage(MDE_DEBUGGING);
//   dbb->resize(0);
//   v1731spill->resize(0);

}

void end_of_spill(int i) {
  *(messanger->getStream()) << "-----> \nThis is End of Spill " << i << endl;
  messanger->sendMessage(MDE_DEBUGGING);

  dbb->resize(0);
  v1731spill->resize(0);

  myDAQ->Process();

//   if (dbb->size()==2) {
//     (*dbb)[0].print();
//     (*dbb)[1].print();
//   }

  MAUS::EMRDaq emr;
  emr.SetDBBArray(*dbb);
  emr.SetV1731PartEventArray(*v1731spill);

  data->SetEMRDaq(emr);
  dataTree->Fill();

}

int main(int argc, char** argv) {

  int run_count(0);
  if (argc==2) {
    stringstream ss;
    ss << argv[1];
    ss >> run_count;
  }
  MDprocessManager proc_manager;
  messanger = MiceDAQMessanger::Instance();
  messanger->setVerbosity(1);

  myDAQ = new DAQManager();
  eventBuilder = new EventBuildManager();

  dbbProc = new DBBDataProcessor();
  dbbProc->SetProcessManager(&proc_manager);
//   dbbProc->set_debug_mode(true);

  v1731Proc = new V1731DataProcessor();
  v1731Proc->SetProcessManager(&proc_manager);

//     MDequipMap::Dump();
  myDAQ->SetFragmentProc("DBB",    dbbProc);
  myDAQ->SetPartEventProc("V1731", v1731Proc);

  myDAQ->DumpProcessors();

  myDAQ->MemBankInit(1024*1024);
  eventBuilder->SetDataPtr(myDAQ->getDataPtr());
  eventBuilder->SetRunCount(run_count);

  eventBuilder->SetOnStartOfRunDo(&start_of_run);
  eventBuilder->SetOnEndOfRunDo(&end_of_run);
//   eventBuilder->SetOnStartOfSpillDo(&start_of_spill);
  eventBuilder->SetOnEndOfSpillDo(&end_of_spill);

  eventBuilder->Go();

  delete myDAQ;
  delete eventBuilder;
  delete dbbProc;
  delete v1731Proc;

  return 1;
}



