#ifndef _DBBCHAINDATAPROCESSOR_
#define _DBBCHAINDATAPROCESSOR_

#include "MDprocessor.h"
#include "MDfragmentDBB.h"
#include "MDfragmentDBBChain.h"
#include "MDpartEventV1731.h"

#include "DAQChannelMap.hh"
#include "DBBSpillData.hh"
#include "EMRDaq.hh"

#define DBB_TRIGGER_CHANNEL 4

class DBBChainDataProcessor : public MDprocessor {
 public:
  DBBChainDataProcessor() {_equipment="DBBChain";_debug_mode=false;}
  virtual ~DBBChainDataProcessor() {}

 /** Unpack a single event part to JSON.
  *
  * This function unpacks a single particleevent,
  * into a JSON sub-tree.
  *
  * \param[in,out] dc Pointer to the event to process.
  * recorded by equipment DBB (EMR board)
  * Will be casted to MDfragmentDBB.
  */
  virtual int Process(MDdataContainer* dc);

 /**
  * This function sets the DAQ map.
  * \param[in] map The DAQ channel map.
  */
  void set_DAQ_map(DAQChannelMap* map) {_chMap = map;}

  std::string get_equipment_name() {return _equipment;}

  std::vector<MAUS::DBBSpillData>* get_spill() {return _spill;}
  void set_spill(std::vector<MAUS::DBBSpillData> *s) {_spill = s;}
  void set_debug_mode(bool mode) {_debug_mode = mode;}

  private:

  DAQChannelMap* _chMap;
  std::string _equipment;
  MAUS::DBBArray *_spill;
  bool _debug_mode;
};

#endif


