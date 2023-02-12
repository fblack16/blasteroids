#ifndef BLACK_BLASTEROIDS_SPACESHIP_H
#define BLACK_BLASTEROIDS_SPACESHIP_H

#include "points.h"
#include "circle.h"
#include "control.h"
#include "allegro5/color.h"

float const SPACESHIP_SCALE_FACTOR = 20.0f;
char const SPACESHIP_COLOR[] = "hotpink";
float const SPACESHIP_LINE_THICKNESS = 2.0f;

#define NUMBER_OF_SPACESHIP_VERTICES 3
Point2D const SPACESHIP_VERTICES[NUMBER_OF_SPACESHIP_VERTICES] = {
    {.x = -0.5f * SPACESHIP_SCALE_FACTOR, .y = -0.5f * SPACESHIP_SCALE_FACTOR},
    {.x = 0.5f * SPACESHIP_SCALE_FACTOR, .y = 0.0f * SPACESHIP_SCALE_FACTOR},
    {.x = -0.5f * SPACESHIP_SCALE_FACTOR, .y = 0.5f * SPACESHIP_SCALE_FACTOR},
};

struct Spaceship {
    Point2D coordinates;
    float heading;
    ALLEGRO_COLOR color;
    Point2D const* vertices;
    Circle hitbox;
};
typedef struct Spaceship Spaceship;

Spaceship initialize_spaceship();
void draw_spaceship(Spaceship const* const spaceship);
void update_spaceship(Spaceship* const spaceship, KeyState const key_states[], float const frame_delta);

#endif
