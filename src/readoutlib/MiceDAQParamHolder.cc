#include "MiceDAQParamHolder.hh"
#include "equipmentList_common.hh"

MiceDAQParamHolder::MiceDAQParamHolder() : dumy_param_(BAD_PARAM) {}

MiceDAQParamHolder::~MiceDAQParamHolder() {}

void MiceDAQParamHolder::setParam(const char* pName, int pValue)  {}

int& MiceDAQParamHolder::getParam(const char* pName)  { return dumy_param_;}

int MiceDAQParamHolder::operator[](const char* pName) {return getParam(pName);}

