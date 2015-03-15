#include "MiceDAQException.hh"

MiceDAQException::MiceDAQException(const std::string & ErrorDescr,
                             const std::string & ErrorLocation,
                             int Svr)
: errorDescription_(ErrorDescr), errorLocation_(ErrorLocation), severity_(Svr) {}

