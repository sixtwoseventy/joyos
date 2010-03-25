#ifndef GEARTRAIN_H
#define GEARTRAIN_H

#include <math.h>

/**
 * \file geartrain.h
 * \brief Geartrain calculations and conversions
 *
 * This file defines a number of useful geartrain/distance/rotaion calculation
 * and conversion routines.
 *
 * To use these functions the user must define the following values before
 * including geartrain.h
 *
 *  - ENCODER_TO_WHEEL_RATIO - the ratio between the encoder and the wheel
 *  - WHEEL_CIRCUMFERENCE - circumference of the wheel in centimetres
 *  - WHEEL_TRACK - distance between the wheels in centimetres
 *
 *  For the example robot:
 *
 *  \code
 *
 *  #define ENCODER_TO_WHEEL_RATIO 3
 *  #define WHEEL_CIRCUMFERENCE 10.0
 *  #define WHEEL_TRACK 20.0
 *
 *  #include <lib/geartrain.h>
 *
 *  \endcode
 *
 */

// Ratio between encoder and wheel (3 in awesome bot)
#ifndef ENCODER_TO_WHEEL_RATIO
#error "ENCODER_TO_WHEEL_RATION not defined before #include <lib/geartrain.h>"
#endif

// Wheel circumference
#ifndef WHEEL_CIRCUMFERENCE
#error "WHEEL_CIRCUMFERENCE not defined before #include <lib/geartrain.h>"
#endif

// Wheel Track (distance between wheels)
#ifndef WHEEL_TRACK
#error "WHEEL_TRACK not defined before #include <lib/geartrain.h>"
#endif

// Standard LEGO Encoder count (number of ticks per revolution of the encoder)
#define TICKS_PER_ENCODER_REV 6

// The number of ticks an encoder reads for every 1 revolution of the wheel
#define TICKS_PER_WHEEL_REV (ENCODER_TO_WHEEL_RATIO*TICKS_PER_ENCODER_REV)

// The number of ticks an encoder reads for every centimeter of wheel travel
#define TICKS_PER_CM ((float)(TICKS_PER_WHEEL_REV)/(float)(WHEEL_CIRCUMFERENCE))

/// Convert from centimeters to encoder ticks
#define CM_TO_TICKS(cm)  (uint32_t)((cm)*TICKS_PER_CM)

/// Convert from degrees to cm of wheel rotation [rotation in place]
#define DEG_TO_CM_IN_PLACE(deg)  ((M_PI*WHEEL_TRACK)*(deg)/360.0)

/// Convert from degrees to encoder ticks [rotation in place]
#define DEG_TO_TICKS_IN_PLACE(deg) (CM_TO_TICKS(DEG_TO_CM_IN_PLACE(deg)))

/// Convert from degrees to cm of wheel rotation [rotation around one wheel]
#define DEG_TO_CM_CORNER(deg)  ((M_PI*2.0*WHEEL_TRACK)*(deg)/360.0)

/// Convert from degrees to encoder ticks [rotation around one wheel]
#define DEG_TO_TICKS_CORNER(deg) (CM_TO_TICKS(DEG_TO_CM_CORNER(deg)))

#endif
