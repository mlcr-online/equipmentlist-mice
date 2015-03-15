#include "DBBChainDataProcessor.hh"
#include "DBBDataProcessor.hh"
#include "DBBSpillData.hh"

using namespace MAUS;
using namespace std;

int DBBChainDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentDBB argument.
//   cout << "This is DBBChainDataProcessor::Process" << endl;

  if ( typeid(*aFragPtr) != typeid(MDfragmentDBBChain) )
    return CastError;

  MDfragmentDBBChain* xDBBChainFragment = static_cast<MDfragmentDBBChain*>(aFragPtr);

  for (int i=0; i<DBBS_IN_CHAIN; i++) {
    MDfragmentDBB* xDBBFragment =  xDBBChainFragment->getFragmentDBB(i);

    if ( !xDBBFragment->IsValid() )
      return GenericError;

//     xDBBFragment->Dump();
    DBBSpillData xDBBSpill;
    xDBBSpill.SetLdcId(this->GetLdcId());
    xDBBSpill.SetTimeStamp(this->GetTimeStamp());
    DBBDataProcessor::fill_spill_data(xDBBFragment, &xDBBSpill);
    if (_debug_mode) xDBBSpill.print();
    _spill->push_back(xDBBSpill);
  }
  return OK;
}

