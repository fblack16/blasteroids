#ifndef BLACK_BLASTEROIDS_ASTEROID_H
#define BLACK_BLASTEROIDS_ASTEROID_H

#include "points.h"
#include "circle.h"
#include "allegro5/color.h"

float const ASTEROID_SCALE_FACTOR = 20.0f;
char const ASTEROID_COLOR[] = "white";
float const ASTEROID_LINE_THICKNESS = 2.0f;
#define ASTEROID_MAX 10

#define NUMBER_OF_ASTEROID_VERTICES 4
Point2D const ASTEROID_VERTICES[NUMBER_OF_ASTEROID_VERTICES] = {
    {.x = -0.5f * ASTEROID_SCALE_FACTOR, .y = 0.0f * ASTEROID_SCALE_FACTOR},
    {.x = 0.0f * ASTEROID_SCALE_FACTOR, .y = 0.5f * ASTEROID_SCALE_FACTOR},
    {.x = 0.5f * ASTEROID_SCALE_FACTOR, .y = 0.0f * ASTEROID_SCALE_FACTOR},
    {.x = 0.0f * ASTEROID_SCALE_FACTOR, .y = -0.5f * ASTEROID_SCALE_FACTOR},
};

struct Asteroid {
    Point2D coordinates;
    float heading;
    float rotation_angle;
    float angle_velocity;
    ALLEGRO_COLOR color;
    Point2D const* vertices;
    Circle hitbox;
};
typedef struct Asteroid Asteroid;

struct AsteroidContainer {
    bool is_in_use;
    Asteroid asteroid;
};
typedef struct AsteroidContainer AsteroidContainer;

Asteroid initialize_asteroid();
void draw_asteroid(Asteroid const* const asteroid);
void draw_asteroid_container(AsteroidContainer const* const asteroid_container);
void draw_asteroid_containers(AsteroidContainer const asteroid_containers[]);
void update_asteroid(Asteroid* const asteroid_container, float frame_delta);
void update_asteroid_container(AsteroidContainer* const asteroid_container, float frame_delta);
void update_asteroid_containers(AsteroidContainer asteroid_containers[], float frame_delta);
AsteroidContainer initialize_asteroid_container(Asteroid const* const Asteroid);
void fill_asteroid_container(AsteroidContainer* const asteroid_container, Asteroid const* const asteroid);

#endif
