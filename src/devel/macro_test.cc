// To run this test do :
// gcc -E macro_test.cc

#include "../readoutlib/MDEMacros.hh"

#define WarriorParams(FIRST, NEXT) \
  FIRST(intelligence) \
  NEXT(wisdom) \
  NEXT(charisma) \
  NEXT(strength)

//SET_ALL_PARAMS_TO_ZERO(WarriorParams);

//IMPLEMENT_PARAMS(WarriorParams, MyClass)
//IMPLEMENT_COPY_CONSTRUCTOR(WarriorParams, MyClass)
//IMPLEMENT_FUNCTIONS(WarriorParams, MyClass)
//IMPLEMENT_GET_INFO(WarriorParams, MyClass)
IMPLEMENT_GET_INFO_SHORT(WarriorParams, MyClass)
