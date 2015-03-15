#include "DBBDataProcessor.hh"
#include "DBBSpillData.hh"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
using namespace MAUS;
using namespace std;

int DBBDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentDBB argument.

  //cout << "This is DBBDataProcessor::Process" << endl;

  if ( typeid(*aFragPtr) != typeid(MDfragmentDBB) )
    return CastError;

  MDfragmentDBB* xDBBFragment = static_cast<MDfragmentDBB*>(aFragPtr);
  if ( !xDBBFragment->IsValid() )
    return GenericError;
//   xDBBFragment->Dump();

  DBBSpillData xDBBSpill;
  xDBBSpill.SetLdcId(this->GetLdcId());
  xDBBSpill.SetTimeStamp(this->GetTimeStamp());
  fill_spill_data(xDBBFragment, &xDBBSpill);
  //  if (_debug_mode) xDBBSpill.print();

  _spill->push_back(xDBBSpill);
//  _spill->SetDBBSpill(xDBBSpill);
//  ( *_docSpill )[xDetector][_equipment].append(pBoardDoc);
//   _daqxDBBSpill.GetEMRDaq().SetDBBSpill(xDBBSpill);
  return OK;
}

void DBBDataProcessor::fill_spill_data(MDfragmentDBB *fragment, DBBSpillData *dbb_spill) {
  int xLdc=3, xGeo;
  string xDetector = "emr";

  dbb_spill->SetDetector(xDetector);
  xGeo = fragment->GetGeo();
  dbb_spill->SetDBBId(xGeo);
  dbb_spill->SetSpillNumber(fragment->GetSpillNumber());
  dbb_spill->SetSpillWidth(fragment->GetSpillWidth());
  dbb_spill->SetTriggerCount(fragment->GetTriggerCount());
  dbb_spill->SetHitCount(fragment->GetHitCount());

  DBBHitsArray hits;
  DBBHitsArray triggers;

  // Put static data into the Json
//   pBoardDoc["detector"]            = xDetector;
//   pBoardDoc["ldc_id"]              = xLdc = this->GetLdcId();
//   pBoardDoc["equip_type"]          = this->GetEquipmentType();
//   pBoardDoc["time_stamp"]          = this->GetTimeStamp();
//   pBoardDoc["spill_number"]        = fragment->GetSpillNumber();
//   pBoardDoc["spill_width"]         = fragment->GetSpillWidth();
//   pBoardDoc["geo"]          = xGeo = fragment->GetGeo();
//   pBoardDoc["trigger_count"]       = fragment->GetTriggerCount();
//   pBoardDoc["hit_count"]           = fragment->GetHitCount();

  // Loop over all the channels
  for (unsigned int xCh=0; xCh<DBB_NCHANNELS; xCh++) {
    // Get the number of leading edge hits.
    int xHitsL = fragment->GetNLeadingEdgeHits(xCh);
    // Get the number of trailing edge hits.
    int xHitsT = fragment->GetNTrailingEdgeHits(xCh);

    // By definition the number of the leading edge hits has to be equal
    // to the number of the trailing edge hits but some times this is not true.
    int xMaxHits = MAX(xHitsL, xHitsT);
/*    if (xMaxHits) {*/
    int xLT, xTT;  // Lead and Trail times
    for (int xHit=0; xHit<xMaxHits; xHit++) {
      DBBHit *hit= new DBBHit();
      hit->SetLDC(xLdc);
      hit->SetGEO(xGeo);
      hit->SetChannel(xCh);
//       hit->SetLTimes(fragment->GetLeadingTimes(xCh));
//       hit->SetTTimes(fragment->GetTrailingTimes(xCh));

      if (xHit < xHitsL) {
        xLT = fragment->GetHitMeasurement(xHit,  // Hit ID
                                             xCh,  // Channel ID
                                             'l');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xLT = 0;
      }

      if (xHit < xHitsT) {
        xTT = fragment->GetHitMeasurement(xHit,  // Hit ID
                                              xCh,  // Channel ID
                                              't');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xTT = 0;
      }
      hit->SetLTime(xLT);
      hit->SetTTime(xTT);
//       if (_debug_mode) hit->print();

      if (xCh == DBB_TRIGGER_CHANNEL)
        triggers.push_back(hit);
      else
        hits.push_back(hit);
    }
  }
  dbb_spill->SetDBBHitsArray(hits);
  dbb_spill->SetDBBTriggersArray(triggers);
}
