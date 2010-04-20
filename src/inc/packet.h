#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>
#ifndef SIMULATE
#include <stdio.h>
#endif

#ifndef SIMULATE

#define PAYLOAD_SIZE 30

#define POSITION 0x00 //Updated position of this bot, other bot, or mouse
#define START 0x01 //Start of the round
#define STOP 0x02 //End of the round
#define STRING 0x03 //String from bot to board
#define SYNC 0x04 //Not sure how to use yet.  Established IDs of all bots/vision system on board
#define GOAL 0x05 //Sets the target position of this robot
#define LIGHT 0x06 //Instructs robots to turn their lights on or off
#define REPLY_STRING 0x07 //String from board to bot
#define STATUS 0x08 // Status packet from mouse

#endif

typedef struct {
    uint8_t id;
    signed x : 12;
    signed y : 12;
    signed theta : 12;
    unsigned confidence : 12;
#ifndef SIMULATE
} __attribute__ ((aligned (1))) __attribute__ ((packed)) board_coord;
#else
} board_coord;
#endif

#ifndef SIMULATE

typedef struct{
    uint8_t id;
    int16_t value;
} __attribute__ ((aligned (1))) __attribute__ ((packed)) light_command;

typedef struct{
    uint8_t id;
    uint8_t caught;
} __attribute__ ((aligned (1))) __attribute__ ((packed)) status_update;

typedef struct {
    uint8_t type;
    uint8_t address;
    union {
        uint8_t array[PAYLOAD_SIZE];
        board_coord coords[4];
        light_command lights[4];
        status_update status;
    } payload;
} __attribute__ ((packed)) packet_buffer;

#endif

#endif
