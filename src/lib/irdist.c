#ifndef SIMULATE

#include "lib/irdist.h"
#include "analog.h"

#else

#include <joyos.h>

#endif

#ifndef SIMULATE

uint16_t irdist_cal_m;
uint16_t irdist_cal_c;

#endif

void irdist_set_calibration(uint16_t m, uint16_t c) {

	#ifndef SIMULATE

    irdist_cal_m = m;
    irdist_cal_c = c;

	#else

	printf("Skipping IR Distance calibration...\n");

	#endif

}

uint16_t irdist_read(int port) {

	#ifndef SIMULATE

    uint16_t v = analog_read(port);
    return irdist_cal_m/v - irdist_cal_c;

	#else

	uint16_t in;
	char pbuf[6];

	sprintf(pbuf, "d %u\n", port);

	acquire(&socket_lock);
	write(sockfd, pbuf, strlen(pbuf));
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);
	sscanf(socket_buffer, "%u", &in);
	release(&socket_lock);
  
	return in;

	#endif

}
