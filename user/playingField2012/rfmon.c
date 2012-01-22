#include<joyos.h>

int usetup() {
    return 0;
}

void uround_start() {

}

void uround_end() {

}

int umain() {
    while(1) {
        copy_objects();
        for (int i = 0; i < 2; i++) {
            printf("ROBOT:%d;%d;%d;%d;%d;%d;\n",
                    i,
                    game.coords[i].id, 
                    game.coords[i].x,
                    game.coords[i].y,
                    game.coords[i].theta,
                    game.coords[i].score);
        }
        for (int i = 0; i < 6; i++) {
            printf("TERR:%d;%d;%d;%d;\n",
                    i,
                    game.territories[i].owner,
                    game.territories[i].remaining,
                    game.territories[i].rate_limit);
        }
        pause(20);
    }
    return 0;
}
