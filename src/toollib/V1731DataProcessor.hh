#ifndef _V1731DATAPROCESSOR_
#define _V1731DATAPROCESSOR_

#include <climits>
#include <algorithm>

#include "MDprocessor.h"
#include "MDpartEventV1731.h"

#include "DAQChannelMap.hh"
#include "EMRDaq.hh"
#include "V1731.hh"

////////////////////////////////////////////////////////////////////////////////
// do not increment inside following macro! (e.g.: MAX( ++a, ++b );
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

class fADCDataProcessor : public MDprocessor {
 public:

  fADCDataProcessor() {}
  ~fADCDataProcessor() {}

  /** Return the position of the maximum.
  * This function returns the number of the sample having maximum amplitude.
  */
  int get_max_position();

  /** Return the position of the minimum.
   * This function returns the number of the sample having minimum amplitude.
   */

  int get_min_position();

  int get_arrival_time();
  /** Return the area of the signal.
  * This function returns the area of the signal. The pedestal is subtracted.
  * The integration is done in window starting 10 samples before the maximum
  * and ending 20 samples after the maximum.
  * \param[out] pos The number of the sample having maximum amplitude.
  */
  int get_signal_area(int &pos);

  int get_neg_signal_area(int &pos);

  int get_pedestal_area(int &pos);

  /** Return the measured value for the given sample
  * \param[in] i The number of the sample.
  */
  int get_point(int i) const { return _data[i]; }

  /// Return the vector of samples
  vector<int> get_data() const { return _data; }

  /** Return the area of the signal.
  * This function returns the area of the signal. The pedestal is subtracted.
  * The integration is done using the whole acquisition window.
  */

  int get_area();

  /// Return the data member _pedestal
  int get_pedestal() const { return _pedestal; }

  /** Return the charge of the signal.
  * This function returns the charge of the signal calculated by the specified
  * algorithm.
  * \param[in] Algorithm identifier of the algorithm.
  */
  int get_charge(int Algorithm = ceaPedMax);

  enum chargeEstimationAlgorithm {
    ceaMinMax, /// Simplest algorithm
    ceaFractionDescriminatorThreshold, /// not implemented
    ceaPedMax,
    ceaPedMin ,
  };

  /**Return the charge of the Cherenkov Singal (negative pulse)
  Uses same Algorithm.  
  */
  int get_charge_ckov(int Algorithm = ceaPedMin);

 protected:

  void set_pedestal();
  int chargeMinMax();
  int chargePedMax();
  int chargePedMin();
  /// vector of samples (measurements) */
  vector<int> _data;

  /// Pedestal level of the signal */
  int _pedestal;
};


#define V1731_SAMPLES_PER_WORD   4
class V1731DataProcessor : public fADCDataProcessor {
 public:
  V1731DataProcessor() {_equipment="V1731";}
  virtual ~V1731DataProcessor() {}

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

  std::vector< std::vector<MAUS::V1731> >* get_spill() {return _spill;}
  void set_spill(std::vector< std::vector<MAUS::V1731> > *s) {_spill = s;}
  private:

  DAQChannelMap* _chMap;
  std::string _equipment;
  std::vector< std::vector<MAUS::V1731> >*_spill;
};



#endif


