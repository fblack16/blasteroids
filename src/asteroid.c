#include "../include/asteroid.h"
#include "../include/display.h"
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/transformations.h>
#include <math.h>

Asteroid initialize_asteroid() {
  // TODO: Should be an enum.
  int direction = rand() % 4; // 0, 1, 2, 3
  Point2D coordinates = {.x = 0.0f, .y = 0.0f};
  switch (direction) {
  case 0:
    coordinates.x = 10.0f;
    coordinates.y = (float)(rand() % DISPLAY_HEIGHT);
    break;
  case 1:
    coordinates.x = DISPLAY_WIDTH - 10.0f;
    coordinates.y = (float)(rand() % DISPLAY_HEIGHT);
    break;
  case 2:
    coordinates.x = (float)(rand() % DISPLAY_WIDTH);
    coordinates.y = 10.0f;
    break;
  case 3:
    coordinates.x = (float)(rand() % DISPLAY_WIDTH);
    coordinates.y = DISPLAY_HEIGHT - 10.0f;
    break;
  default:
    break;
  }

  Asteroid asteroid = {
      .coordinates = coordinates,
      .heading = (float)(rand() % 36),
      .rotation_angle = 0.0f,
      .angle_velocity = 0.1f,
      .color = al_color_name(ASTEROID_COLOR),
      .vertices = ASTEROID_VERTICES,
      .hitbox =
          {
              .center = coordinates,
              .radius = 1.0f * ASTEROID_SCALE_FACTOR,
          },
  };

  return asteroid;
}

void draw_asteroid(Asteroid const *const asteroid) {
  ALLEGRO_TRANSFORM transform;
  al_identity_transform(&transform);
  al_rotate_transform(&transform, asteroid->rotation_angle);
  al_translate_transform(&transform, asteroid->coordinates.x,
                         asteroid->coordinates.y);
  al_use_transform(&transform);

  al_draw_line(asteroid->vertices[NUMBER_OF_ASTEROID_VERTICES - 1].x,
               asteroid->vertices[NUMBER_OF_ASTEROID_VERTICES - 1].y,
               asteroid->vertices[0].x, asteroid->vertices[0].y,
               asteroid->color, ASTEROID_LINE_THICKNESS);
  for (int i = 0; i < NUMBER_OF_ASTEROID_VERTICES - 1; ++i) {
    al_draw_line(asteroid->vertices[i].x, asteroid->vertices[i].y,
                 asteroid->vertices[i + 1].x, asteroid->vertices[i + 1].y,
                 asteroid->color, ASTEROID_LINE_THICKNESS);
  }

  al_draw_circle(asteroid->hitbox.center.x, asteroid->hitbox.center.y,
                 asteroid->hitbox.radius, al_color_name("green"), 2.0f);
}

void draw_asteroid_container(
    AsteroidContainer const *const asteroid_container) {
  ALLEGRO_TRANSFORM transform;
  draw_asteroid(&asteroid_container->asteroid);
}

void draw_asteroid_containers(AsteroidContainer const asteroid_containers[]) {
  for (int i = 0; i < ASTEROID_MAX; ++i) {
    if (asteroid_containers[i].is_in_use) {
      draw_asteroid_container(&asteroid_containers[i]);
    }
  }
}

void update_asteroid(Asteroid *const asteroid, float frame_delta) {
  asteroid->coordinates.x +=
      (float)cos(asteroid->heading) * (1.0f + frame_delta);
  asteroid->coordinates.y +=
      (float)sin(asteroid->heading) * (1.0f + frame_delta);
  asteroid->rotation_angle += asteroid->angle_velocity;

  asteroid->hitbox.center.x = asteroid->coordinates.x;
  asteroid->hitbox.center.y = asteroid->coordinates.y;
}

void update_asteroid_container(AsteroidContainer *const asteroid_container,
                               float frame_delta) {
  update_asteroid(&asteroid_container->asteroid, frame_delta);
}

void update_asteroid_containers(AsteroidContainer asteroid_containers[],
                                float frame_delta) {
  for (int i = 0; i < ASTEROID_MAX; ++i) {
    if (asteroid_containers[i].is_in_use) {
      update_asteroid_container(&asteroid_containers[i], frame_delta);
    }
  }
}

void fill_asteroid_container(AsteroidContainer *const asteroid_container, const Asteroid *const asteroid) {
    if (asteroid_container->is_in_use) {
        return;
    }
    asteroid_container->asteroid = *asteroid;
}
