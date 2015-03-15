#include "EventBuildManager.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQException.hh"
#include "MDexception.h"

using namespace std;

EventBuildManager::EventBuildManager()
: spill_count_(-1), run_count_(0),
 _onStartOfRunDo(NULL), _onEndOfRunDo(NULL),
 _onStartOfSpillDo(NULL), _onEndOfSpillDo(NULL) {}

EventBuildManager::~EventBuildManager() {}

void EventBuildManager::SetDataPtr(MDE_Pointer *dataPtr) {
  binary_file_.SetDataPtr(dataPtr);
}

void EventBuildManager::Go() {

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();

  while (1) {

    if (spill_count_==-1) {
      if(_onStartOfRunDo)
        _onStartOfRunDo(run_count_);

      spill_count_ = 0;
    }

    stringstream ss;
    ss << "../data_tmp/" << spill_count_ << "_miceacq22.data";

    bool file_OK = binary_file_.Open(ss.str(),1);
    if ( !file_OK ) {
      if (spill_count_!=0) {
        if(_onEndOfRunDo)
          _onEndOfRunDo(run_count_);

        *(messanger->getStream()) << "EventBuildManager: end of run " << run_count_ << "\n...\n...\n...";
        messanger->sendMessage(MDE_INFO);
         spill_count_ = -1;
        run_count_ ++;
      }
      break;
    }

    try {

      if(_onStartOfSpillDo)
        _onStartOfSpillDo(spill_count_);

      binary_file_.Read();
      remove(ss.str().c_str());

      if(_onEndOfSpillDo)
        _onEndOfSpillDo(spill_count_);

    } catch (MiceDAQException e) {
      *(messanger->getStream()) << "EventBuildManager: DAQ exception"    << endl
                                << e.GetLocation()    << endl << e.GetDescription() << endl;
      messanger->sendMessage(MDE_ERROR);

      spill_count_++;
      continue;
    } catch ( MDexception & e) {
      *(messanger->getStream()) << "EventBuildManager: Unpacking exception" << endl
                                << e.GetDescription() << endl;
      messanger->sendMessage(MDE_ERROR);

      spill_count_++;
      continue;
    } catch(std::exception & e) {
      *(messanger->getStream()) << "EventBuildManager: Standard exception" << endl
                                << e.what() << endl;
      messanger->sendMessage(MDE_ERROR);
      spill_count_++;
      continue;
    }

    spill_count_++;
  }
}



