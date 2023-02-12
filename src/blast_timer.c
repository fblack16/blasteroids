#include "../include/blast_timer.h"
#include <allegro5/events.h>
#include <stdlib.h>

BLAST_TIMER* create_blast_timer() {
    BLAST_TIMER* blast_timer = malloc(sizeof(BLAST_TIMER));
    if (blast_timer) {
        al_init_user_event_source(&blast_timer->event_source);
    }
    return blast_timer;
}

void destroy_blast_timer(BLAST_TIMER* blast_timer) {
    if (blast_timer) {
        al_destroy_user_event_source(&blast_timer->event_source);
        free(blast_timer);
    }
}
