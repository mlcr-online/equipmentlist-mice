#include "V1731DataProcessor.hh"

int V1731DataProcessor::Process(MDdataContainer* aPartEventPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1731 argument.
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1731) )  return CastError;
  MDpartEventV1731* xV1731Evnt = static_cast<MDpartEventV1731*>(aPartEventPtr);
//   xV1731Evnt->Dump();

  string xDetector = "emr";
  if ( !xV1731Evnt->IsValid() )
    return GenericError;

  unsigned int xEvCounter = xV1731Evnt->GetEventCounter(); // starts from 1.
  if (xEvCounter > (*_spill).size()) {
    _spill->resize(xEvCounter);
  }
  // Put static data into the Json
//   int xLdc, xGeo, xEquip, xPartEv;
//   pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
//   pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
//   pBoardDoc["time_stamp"]          = this->GetTimeStamp();
//   pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
//   pBoardDoc["part_event_number"] = xPartEv = this->GetPartEventNumber();
//   pBoardDoc["geo"]          = xGeo = xV1731Evnt->GetGeo();
//   pBoardDoc["trigger_time_tag"]    = xV1731Evnt->GetTriggerTimeTag();

  MAUS::V1731 xV1731hit;
  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1731_NCHANNELS; xCh++) {
    if (xV1731Evnt->GetLength(xCh) ) {
      unsigned int xSamples = ( xV1731Evnt->GetLength(xCh) )*V1731_SAMPLES_PER_WORD;
      for (unsigned int j = 0; j < xSamples; j++) {
        int sample = xV1731Evnt->GetSampleData(xCh,  // Channel ID
                                               j);  // Sample ID
        _data.push_back(sample);
      }
      this->set_pedestal();

      xV1731hit.SetDetector(xDetector);
      xV1731hit.SetLdcId(this->GetLdcId());
      xV1731hit.SetPartEventNumber(xV1731Evnt->GetEventCount());
      xV1731hit.SetPhysEventNumber(this->GetPhysEventNumber());
      xV1731hit.SetTimeStamp(this->GetTimeStamp());
      xV1731hit.SetGeo(xV1731Evnt->GetGeo());
      xV1731hit.SetChargeMm( this->get_charge(ceaMinMax) );
      xV1731hit.SetChargePm( this->get_charge(ceaPedMin) );
      int dummy_var = 0;
      xV1731hit.SetPulseArea(this->get_neg_signal_area(dummy_var));
      xV1731hit.SetMaxPos(this->get_max_position());
      xV1731hit.SetArrivalTime(this->get_arrival_time());
      xV1731hit.SetPositionMin(this->get_min_position());
      xV1731hit.SetPedestal(this->get_pedestal());
      xV1731hit.SetSampleArray(_data);
      xV1731hit.SetChannel(xCh);
//       xV1731hit.print();
      (*_spill)[xEvCounter-1].push_back(xV1731hit);
    }
//       xfAdcHit["charge_mm"]    = charge_mm;
//       xfAdcHit["charge_pm"]    = this->get_charge(ceaPedMin);
//       int dummy_var = 0;
//       xfAdcHit["pulse_area"]   = this->get_neg_signal_area(dummy_var);
//       xfAdcHit["max_pos"]      = this->get_max_position();
//       xfAdcHit["arrival_time"] = this->get_arrival_time();
//       xfAdcHit["position_min"] = this->get_min_position();
//       xfAdcHit["pedestal"]     = this->get_pedestal();
//       xfAdcHit["samples"]      = this ->get_samples();
//       DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
//       if (xKey) {
//         xDetector = xKey->detector();
//         xfAdcHit["channel_key"]   = xKey->str();
//         xfAdcHit["detector"]      = xDetector;
//       } else {
//         xfAdcHit["detector"] = xDetector = "unknown";
//       }
//       xfAdcHit["channel"]        = xCh;
// 
//       ( *_docSpill )[xDetector][ xPartEv ][_equipment].append(xfAdcHit);

    _data.resize(0);
  }

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int fADCDataProcessor::get_area() {
  int area = 0;
  for ( unsigned int ich = 0; ich < _data.size(); ich++)
    area += abs(_data[ich] - _pedestal);

  return area;
}

void fADCDataProcessor::set_pedestal() {
  double area = 0;
  unsigned int pedBins = 10;
  if (_data.size() < pedBins)
    _pedestal = 0;

  if (_data.size() > pedBins) {
    for (unsigned int i = 0; i < pedBins; i++) {
       area += _data[i];
    }
  }
  _pedestal = static_cast<int>(area/pedBins);
}

int fADCDataProcessor::chargeMinMax() {
  int min, max, d;
  d = -99;
  max = INT_MIN;
  min = INT_MAX;
  for ( unsigned int i = 0; i < _data.size(); ++i ) {
    d = _data[ i ];
    min = ( min > d )? d : min;
    max = ( max > d )? max : d;
  }

  return max - min;
}

int fADCDataProcessor::get_neg_signal_area(int&pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator min;
  int area = 0;
  if (_data.size()) {
    min = min_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), min);

    if (pos > 15) { // was: pos > 10
      it = min -  10;
      while (it < min + 30 && it < _data.end()) {
        // area += abs(*it - _pedestal);
        area += _pedestal- *it;
//         std::cout << "val: " << *it << "  integr. area: " << area << std::endl;
        it++;
      }
    }
    return area;
  }

  return 0;
}


int fADCDataProcessor::get_signal_area(int& pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator max;
  int area = 0;
  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);

    if (pos > 10) it = max - 10;

    while (it < max+20) {
      area += abs(*it - _pedestal);
      it++;
    }

    return area;
  }

  return 0;
}

int fADCDataProcessor::get_arrival_time() {
  int arr_time = 0;
  for ( unsigned int i = 0; i < _data.size(); ++i ) {
    arr_time = i;
    if ( abs(_pedestal - _data[i]) > 2 ) {
      break;
    }
  }
    return arr_time;
}

int fADCDataProcessor::get_pedestal_area(int& pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator max;
  int pedareaminus = 0;
  int pedareaplus = 0;

  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);

    if (pos > 30 && pos < 97) {
      it = max - 30;
      while (it < max-10) {
        pedareaminus += *it - _pedestal;
        it++;
      }

      it = max + 20;
      while (it < max+30) {
        pedareaplus += *it - _pedestal;
        it++;
      }
    }

    return pedareaminus + pedareaplus;
  }

  return 0;
}

int fADCDataProcessor::get_max_position() {
  int pos = 0;
  vector<int>::iterator max;
  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);

    return pos;
  }

  return 0;
}

int fADCDataProcessor::get_min_position() {
  int pos = 0;
  vector<int>::iterator min;
  if (_data.size()) {
    min = min_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), min);

    return pos;
  }

  return 0;
}

int fADCDataProcessor::chargePedMax() {
  int max = 0;
  if (_data.size()) {
    max = *max_element(_data.begin(), _data.end());
    return max - _pedestal;
  }

  return 0;
}

int fADCDataProcessor::chargePedMin() {
  int min = 0;
  if (_data.size()) {
    min = *min_element(_data.begin(), _data.end());
    return _pedestal - min;
  }

  return 0;
}

int fADCDataProcessor::get_charge(int Algorithm) {
  int charge;
  switch ( Algorithm ) {
  case ceaMinMax:
    charge = chargeMinMax();
    break;
  case ceaPedMax:
    charge = chargePedMax();
    break;
  case ceaPedMin:
    charge = chargePedMin();
    break;
  default:
    charge = -99;
    break;
  }

  return charge;
}



