#include "equipmentList_common.hh"
#include "globals.h"
#include "vme.h"
#include "maroc.h"

int debug;
int FEBID;
int outreg;           /* Output register */
int addr_conn_out;    /* */
int addr_conn_in;     /* */
int mar_mask[829];    /* */
int32_t BHandle;      /* */

using namespace std;

int main(int argc, char **argv)
{
  FEBID=0;
  debug=0;
  if (argc>1) {
    stringstream ss;
    ss << argv[1];
    ss >> FEBID;
  }

  if (argc>2) {
    stringstream ss;
    ss << argv[2];
    ss >> debug;
  }
  std::cout << "FEBID: " << FEBID << std::endl;
  std::cout << "debug: " << debug << std::endl;

  outreg = 0x33*0x10000;
  addr_conn_out = outreg+0xe000;
  addr_conn_in = outreg+0xf000;

  InitVme();
  LoadMaroc();

  fprintf(stdout,"\n    DONE\n\n");
  return 0;
}

