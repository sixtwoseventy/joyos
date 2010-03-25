#include <joyos.h>
#include <happylib.h>

/**
 * This program attempts to calculate constants for a sharp IR
 * linearisation function.
 *
 * The program will take a number of readings a different know
 * distances then do a line fit and return the calibration
 * constants kM and kC. These constants can be passed to
 * ir_dist_set_calibration in a user program.
 *
 */

uint8_t irdist_fit(uint16_t *xd, uint16_t *yd,
        uint16_t n, uint16_t *km, uint16_t *kc) {
    uint16_t i;
    float a=0,b=0,c=0,d=0;
    float det,den,num;

    // calculate a=sum(x_i^2), b=sum(x_i), c=sum(x_i/y_i) d=sum(1/y_i)
    for (i=0;i<n;i++) {
        a += xd[i]*xd[i];
        b += xd[i];
        c += xd[i]/(float)(yd[i]);
        d += 1/(float)(yd[i]);
    }

    // solve for kM and kC
    det = (n*a - b*b);
    den = (n*c - b*d);
    num = (a*d - b*c);
    *km = (uint16_t)(det/den);
    *kc = (uint16_t)(num/den);
    return 0;
}

int usetup() {
    return 0;
}

uint16_t xd[36];
uint16_t yd[36];
int umain() {
    uint16_t i,n = 36;
    uint16_t port=8;
    uint16_t km,kc;
    //happylib_init();
    // start
    printf("\nIRDistCal       Press Go");
    go_click();
    // get number of samples to read
    while (!go_press()) {
        printf("\nUse frob to # ofsamples: %2d",n);
        switch (frob_read_range(0,2)) {
            case 0: n= 9; break;
            case 1: n=18; break;
            case 2: n=36; break;
        }
        pause (40);
    }

    // wait for go release
    while (go_press());

    // fill distance array
    for (i=0;i<n;i++) {
        xd[i] = 10 + 2*i*(36/n);
    }
    // get port number
    while (!go_press()) {
        port = frob_read_range(8,23);
        printf("\nUse frob to     select port: %2d",port);
        pause (40);
    }

    // wait for go release
    while (go_press());

    // read samples
    for (i=0;i<n;i++) {
        while (!go_press()) {
            yd[i] = analog_read(port);
            printf("\nSample @ %2dcm =%4d",xd[i],yd[i]);
        }

        // wait for go release
        while (go_press());
    }

    // calculate & print
    irdist_fit(xd,yd,n,&km,&kc);
    printf("\nOK: M: %5d    C: %d, press Go",km,kc);
    go_click();
    // save
    /*
       Disabled until confdb is working
       printf("\nGo to Save calibStop to quit");
       while (1) {
       if (go_press()) {
       go_click();
       confdb_save_integer(CONF_HLIB_IRDIST_M,km);
       confdb_save_integer(CONF_HLIB_IRDIST_C,kc);
       break;
       }
       if (stop_press()) {
       stop_press();
       break;
       }
       }
     */

    printf("\ncalibration done");

    // do nothing forever
    while (1);

    return 0;
}
