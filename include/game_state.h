#ifndef BLACK_BLASTEROIDS_GAME_STATE_H
#define BLACK_BLASTEROIDS_GAME_STATE_H

#include <allegro5/display.h>
#include <allegro5/keycodes.h>
#include <allegro5/timer.h>
#include "spaceship.h"
#include "asteroid.h"
#include "blast.h"
#include "control.h"

struct GameState {
    ALLEGRO_DISPLAY* display_ptr;
    ALLEGRO_EVENT_QUEUE* event_queue_ptr;
    ALLEGRO_TIMER* timer_ptr;
    KeyState key_states[ALLEGRO_KEY_MAX];
    Spaceship spaceship;
    AsteroidContainer asteroid_containers[ASTEROID_MAX];
    BlastContainer blast_containers[BLAST_MAX];
    bool is_blast_on_cooldown;
    bool is_asteroid_creation_on_cooldown;
};
typedef struct GameState GameState;

GameState initialize_game_state();
void destroy_game_state(GameState* game_state_ptr);
void fire_blast(GameState* game_state);
void check_collision_between_blasts_and_asteroids(GameState* game_state);
void create_new_asteroids(GameState* game_state);

#endif
