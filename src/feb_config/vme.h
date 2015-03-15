#ifndef vme_h
#define vme_h

void InitVme();
unsigned short write_vme(int data, int address);
unsigned short read_vme(int *data, int address);
void clear_bit(int *dataptr, int mode_bit);
void set_bit(int *dataptr, int mode_bit);
void clock_bus_pulse();
void regin_up();
void regin_down();
void clkreg_clk();
void send_on_bus_mus(int febid, int address, int data);

#endif

