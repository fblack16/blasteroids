#include "../include/blast.h"
#include "../include/display.h"
#include "allegro5/allegro_color.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/transformations.h>
#include <math.h>

Blast initialize_blast(Point2D const coordinates, float const heading) {
  Blast blast = {.coordinates = coordinates,
                 .heading = heading,
                 .vertices = BLAST_VERTICES,
                 .color = al_color_name(BLAST_COLOR),
                 .hitbox = {
                     .center = coordinates,
                     .radius = 1.0f * BLAST_SCALE_FACTOR,
                 }};
  return blast;
}

BlastContainer initialize_blast_container(Point2D const blast_position,
                                          float const heading) {
  BlastContainer blast_container = {
      .is_in_use = true,
      .blast = initialize_blast(blast_position, heading),
  };

  return blast_container;
}

void fill_blast_container(BlastContainer *const blast_container,
                          Blast const *const blast) {
  if (blast_container->is_in_use) {
    return;
  }
  blast_container->blast = *blast;
}

void draw_blast(Blast const *const blast) {
  ALLEGRO_TRANSFORM transform;
  al_rotate_transform(&transform, blast->heading);
  al_translate_transform(&transform, blast->coordinates.x + DISPLAY_OFFSET_X,
                         blast->coordinates.y + DISPLAY_OFFSET_Y);
  al_use_transform(&transform);

  for (int i = 0; i < NUMBER_OF_BLAST_VERTICES - 1; ++i) {
    al_draw_line(blast->vertices[i].x, blast->vertices[i].y,
                 blast->vertices[i + 1].x, blast->vertices[i + 1].y,
                 blast->color, BLAST_LINE_THICKNESS);
  }

  al_draw_circle(blast->hitbox.center.x, blast->hitbox.center.y,
                 blast->hitbox.radius, al_color_name("green"), 2.0f);
}

void draw_blast_container(BlastContainer const *const blast_container) {
  if (blast_container->is_in_use) {
    draw_blast(&blast_container->blast);
  }
}

void draw_blast_containers(BlastContainer const blast_containers[]) {
  for (int i = 0; i < BLAST_MAX; ++i) {
    draw_blast_container(&blast_containers[i]);
  }
}

void update_blast(Blast *const blast, float const frame_delta) {
  blast->coordinates.x += (float)cos(blast->heading) * (1.0f + frame_delta);
  blast->coordinates.y += (float)sin(blast->heading) * (1.0f + frame_delta);

  blast->hitbox.center.x = blast->coordinates.x;
  blast->hitbox.center.y = blast->coordinates.y;
}

void update_blast_container(BlastContainer *const blast_container,
                            float const frame_delta) {
  if (blast_container->is_in_use) {
    update_blast(&blast_container->blast, frame_delta);
    bool blast_out_of_x_bounds =
        blast_container->blast.coordinates.x < 0 ||
        blast_container->blast.coordinates.x > DISPLAY_WIDTH;
    bool blast_out_of_y_bounds =
        blast_container->blast.coordinates.y < 0 ||
        blast_container->blast.coordinates.y > DISPLAY_HEIGHT;
    // If the blast leaves the screen, mark the container as not being in use to
    // prevent unnecessary drawing and computations.
    if (blast_out_of_x_bounds || blast_out_of_y_bounds) {
      blast_container->is_in_use = false;
    }
  }
}

void update_blast_containers(BlastContainer blast_containers[],
                             float const frame_delta) {
  for (int i = 0; i < BLAST_MAX; ++i) {
    update_blast_container(&blast_containers[i], frame_delta);
  }
}
