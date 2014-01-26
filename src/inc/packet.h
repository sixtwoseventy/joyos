#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>
#ifndef SIMULATE
#include <stdio.h>
#endif

#ifndef SIMULATE

#define PAYLOAD_SIZE 30

enum {
    POSITION, //Updated position of this bot, other bot, or mouse
    START, //Start of the round
    STOP, //End of the round
    STRING, //String from bot to board
    REPLY_STRING //String from board to bot
} packet_type;

#endif




typedef struct {
    unsigned d_blue_high_remaining : 6;
    unsigned d_blue_high_rate_limit : 6;
    unsigned d_red_high_remaining : 6;
    unsigned d_red_high_rate_limit : 6;
    unsigned d_blue_low_remaining : 6;
    unsigned d_blue_low_rate_limit : 6;
    unsigned d_red_low_remaining : 6;
    unsigned d_red_low_rate_limit : 6;
    
    unsigned t_blue_left : 4;
    unsigned t_blue_right : 4;
    unsigned t_orange_left : 4;
    unsigned t_orange_right  : 4;
   
#ifndef SIMULATE
} __attribute__ ((aligned (1))) __attribute__ ((packed))field_data;
#else
} field_data;
#endif


typedef struct {
    unsigned id : 8;
    signed x : 12;
    signed y : 12;
    signed theta : 12;
    unsigned score : 12;
#ifndef SIMULATE
} __attribute__ ((aligned (1))) __attribute__ ((packed)) board_coord;
#else
} board_coord;
#endif


typedef struct {
    board_coord coords[2];
    field_data sides[2];
#ifndef SIMULATE
} __attribute__ ((aligned (1))) __attribute__ ((packed)) game_data;
#else
} game_data;
#endif


#ifndef SIMULATE

typedef struct {
    uint8_t type;
    unsigned board : 2;
    unsigned seq_no : 6;
    union {
        uint8_t array[PAYLOAD_SIZE];
        game_data game;
    } payload;
} __attribute__ ((aligned (1))) __attribute__ ((packed)) packet_buffer;

#endif

#endif
