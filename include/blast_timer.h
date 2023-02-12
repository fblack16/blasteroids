#ifndef BLACK_BLASTEROIDS_BLAST_TIMER_H
#define BLACK_BLASTEROIDS_BLAST_TIMER_H

#include <allegro5/events.h>

struct blast_timer {
    ALLEGRO_EVENT_SOURCE event_source;
};
typedef struct blast_timer BLAST_TIMER;

// TODO: Extract to user event header
enum {
    EVENT_BLAST_TIMER = 1024,
};

BLAST_TIMER* create_blast_timer();
void destroy_blast_timer(BLAST_TIMER* blast_timer);

#endif
