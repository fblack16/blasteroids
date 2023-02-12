#include "../include/spaceship.h"
#include "../include/display.h"
#include <allegro5/allegro_color.h>
#include <allegro5/transformations.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

Spaceship initialize_spaceship() {
    Spaceship spaceship = {
        .coordinates = {.x = 0.0f, .y = 0.0f},
        .heading = 0.0f,
        .color = al_color_name(SPACESHIP_COLOR),
        .vertices = SPACESHIP_VERTICES,
        .hitbox = {
            .center = {.x = 0.0f, .y = 0.0f},
            .radius = 1.0f * SPACESHIP_SCALE_FACTOR,
        }
    };
    return spaceship;
}

void draw_spaceship(const Spaceship *const spaceship) {
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_rotate_transform(&transform, spaceship->heading);
    al_translate_transform(&transform, spaceship->coordinates.x + (float)DISPLAY_OFFSET_X, spaceship->coordinates.y + (float)DISPLAY_OFFSET_Y);
    al_use_transform(&transform);

    al_draw_line(spaceship->vertices[NUMBER_OF_SPACESHIP_VERTICES-1].x, spaceship->vertices[NUMBER_OF_SPACESHIP_VERTICES-1].y, spaceship->vertices[0].x, spaceship->vertices[0].y, spaceship->color, SPACESHIP_LINE_THICKNESS);
    for (int i = 0; i < NUMBER_OF_SPACESHIP_VERTICES - 1; ++i) {
        al_draw_line(spaceship->vertices[i].x, spaceship->vertices[i].y, spaceship->vertices[i+1].x, spaceship->vertices[i+1].y, spaceship->color, SPACESHIP_LINE_THICKNESS);
    }

    al_draw_circle(spaceship->hitbox.center.x, spaceship->hitbox.center.y, spaceship->hitbox.radius, al_color_name("green"), 2.0f);
}

void update_spaceship(Spaceship *const spaceship, KeyState const* key_states, float const frame_delta) {
    if (is_pressed_or_needs_processing(key_states, FORWARD)) {
        spaceship->coordinates.x += (float)cos(spaceship->heading) * (1.0f + frame_delta);
        spaceship->coordinates.y += (float)sin(spaceship->heading) * (1.0f + frame_delta);
        spaceship->hitbox.center.x = spaceship->coordinates.x;
        spaceship->hitbox.center.y = spaceship->coordinates.y;
    }
    if (is_pressed_or_needs_processing(key_states, BACK)) {
        spaceship->coordinates.x -= (float)cos(spaceship->heading) * (1.0f + frame_delta);
        spaceship->coordinates.y -= (float)sin(spaceship->heading) * (1.0f + frame_delta);
        spaceship->hitbox.center.x = spaceship->coordinates.x;
        spaceship->hitbox.center.y = spaceship->coordinates.y;
    }
    if (is_pressed_or_needs_processing(key_states, LEFT)) {
        spaceship->heading += 0.1f * (1.0f + frame_delta);
    }
    if (is_pressed_or_needs_processing(key_states, RIGHT)) {
        spaceship->heading -= 0.1f * (1.0f + frame_delta);
    }
}
