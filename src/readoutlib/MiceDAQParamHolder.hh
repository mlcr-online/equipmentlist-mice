#ifndef MICEDAQPARAMHOLDER
#define MICEDAQPARAMHOLDER 1

#include "MDEMacros.hh"
#include "equipmentList_common.hh"
#include "MiceDAQException.hh"

class MiceDAQParamHolder {
 private:
  struct SetParamHelper {
    MiceDAQParamHolder* obj_;
    SetParamHelper(MiceDAQParamHolder* o): obj_(o) {}

    const SetParamHelper& operator() (const char* name, int value) const {
      obj_->setParam(name, value);

      return *this;
    }
  };

  int dumy_param_;

 public:
  MiceDAQParamHolder();
  virtual ~MiceDAQParamHolder();

  SetParamHelper setParams(const char* name, int value) {
    this->setParam(name, value);
    return SetParamHelper(this);
  }

  virtual void setParam(const char* pName, int pValue);
  virtual int& getParam(const char* pName);

  int operator[](const char* pName); // {return getParam(pName);}
};

#endif



