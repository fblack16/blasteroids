#ifndef BLACK_BLASTEROIDS_INITIALIZATION_H
#define BLACK_BLASTEROIDS_INITIALIZATION_H

#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/timer.h>

void initialize_allegro();
void initialize_allegro_primitives();
void initialize_keyboard();
ALLEGRO_DISPLAY* initialize_display();
ALLEGRO_EVENT_QUEUE* initialize_event_queue();
ALLEGRO_TIMER* initialize_timer();

#endif
