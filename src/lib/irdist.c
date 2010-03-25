#include "lib/irdist.h"
#include "analog.h"

uint16_t irdist_cal_m;
uint16_t irdist_cal_c;

void irdist_set_calibration(uint16_t m, uint16_t c) {
    irdist_cal_m = m;
    irdist_cal_c = c;
}

uint16_t irdist_read(int port) {
    uint16_t v = analog_read(port);
    return irdist_cal_m/v - irdist_cal_c;
}
