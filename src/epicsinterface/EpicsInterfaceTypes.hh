#ifndef EPICS_INTERFACE_TYPES
#define EPICS_INTERFACE_TYPES 1

#include "epicsTypes.h"

typedef signed char             i8;    // Signed 8-bit integer
typedef signed short            i16;   // Signed 16-bit integer
typedef signed long             i32;   // Signed 32-bit integer

typedef unsigned char           u8;    // Unsigned 8-bit integer
typedef char                    str40[MAX_STRING_SIZE];  // 
typedef const char              c_str40[MAX_STRING_SIZE];  // 


typedef unsigned short          u16;   // Unsigned 16-bit integer
typedef unsigned long           u32;   // Unsigned 32-bit integer


#endif



