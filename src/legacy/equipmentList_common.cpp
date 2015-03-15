#include "equipmentList_common.h"


/***************************/

long32 getBA( char  * BA ) {
  long32 res;
  sscanf( BA, "%x", &res);
  return res; 
}


int decodeBinStr( char * BStr ) {
  int res = 0;
  int i, len = strlen( BStr );
  for( i = 0; i < len; i++ ){
    if( BStr[ i ] == '1' ) res += 1 << i;
  }

  //  int i, len = strlen( BStr )-1; //The length of the string includes the string end character, so decrease it by 1
  //  for( i = len; i > -1; --i ){
  //    if( BStr[ i ] == '1' ) res = 1 << ( len - i );
  //  }
  return res;
}

int decodeChar( char * Ch ) {
  switch ( Ch[ 0 ] ){
    case 'Y' :
    case 'y' :
    case '1' :
      return 1;
    case 'N' :
    case 'n' :
    case '0' :
      return 0;
    default :
      return -1;
  };
}

int decodeBaseAddress( char * BA ) {
  int res = 0;
  sscanf( BA, "%x", &res );
  return res;
}

int bitCount( int Number ) {
  int N = Number;
  int i = 0;
  while (N) {
    ++i;
    N &= (N - 1);
  }
  return i;
}

long32 channelPattern( char * channelConfig ) {
  //TODO:Add code here
  char chPat[32];
  long32 Pat;
  int cnt = 0;
  //Get the channel config string
  cnt = sscanf( channelConfig, "%s", chPat );
  //check if it is set to "all" channels
  if( strcmp( chPat, "all" ) == 0 ){
    Pat = 0xFFFFFFFF;
  } else 
  //..or to "none" of the channels
  if( strcmp( chPat, "none" ) == 0 ){
    Pat = 0x0;
  } else {
    //if none of the above is true, try to interpret the string as HEX integer
    cnt = sscanf( chPat, "%x", &Pat );
  }
  return Pat;
//  return 0xFFFFFFFF;
}
/*
int getHighestBitNumber( long32 Word )
{
  int X0, X1, X2, X3, X4, X5, X6;
  X0 = Word;
  X1 = X0 || (X0 >> 1);
  X2 = X1 || (X1 >> 2);
  X3 = X2 || (X2 >> 4);
  X4 = X3 || (X3 >> 8);
  X5 = X4 || (X4 >> 16);
  //  X6 = X5 | (X5 >> 32);
  return X5;
}
*/

