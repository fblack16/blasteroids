#ifndef BLACK_BLASTEROIDS_CONTROL_H
#define BLACK_BLASTEROIDS_CONTROL_H

#include <stdbool.h>
#include <allegro5/keycodes.h>

const int FORWARD = ALLEGRO_KEY_W;
const int BACK = ALLEGRO_KEY_S;
const int LEFT = ALLEGRO_KEY_A;
const int RIGHT = ALLEGRO_KEY_D;
const int FIRE = ALLEGRO_KEY_SPACE;

struct KeyState {
    bool is_pressed;
    bool needs_processing;
};
typedef struct KeyState KeyState;

bool is_pressed_or_needs_processing(KeyState const key_states[], int const keycode);
void unset_needs_processing(KeyState key_states[]);

#endif
