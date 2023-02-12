#ifndef BLACK_BLASTEROIDS_BLAST_H
#define BLACK_BLASTEROIDS_BLAST_H

#include "points.h"
#include "circle.h"
#include "allegro5/color.h"

float const BLAST_SCALE_FACTOR = 20.0f;
char const BLAST_COLOR[] = "red";
float const BLAST_LINE_THICKNESS = 2.0f;
float const BLAST_VELOCITY = 2.0f;
#define BLAST_MAX 10

#define NUMBER_OF_BLAST_VERTICES 2
Point2D const BLAST_VERTICES[NUMBER_OF_BLAST_VERTICES] = {
    {.x = 0.0f * BLAST_SCALE_FACTOR, .y = 0.0f * BLAST_SCALE_FACTOR},
    {.x = 1.0f * BLAST_SCALE_FACTOR, .y = 0.0f * BLAST_SCALE_FACTOR},
};

struct Blast {
    Point2D coordinates;
    float heading;
    ALLEGRO_COLOR color;
    Point2D const* vertices;
    Circle hitbox;
};
typedef struct Blast Blast;

struct BlastContainer {
    bool is_in_use;
    Blast blast;
};
typedef struct BlastContainer BlastContainer;

Blast initialize_blast(Point2D const blast_position, float const heading);
BlastContainer initialize_blast_container(Point2D const blast_position, float const heading);
void fill_blast_container(BlastContainer* const blast_container, Blast const* const blast);
void draw_blast(Blast const* const blast);
void draw_blast_container(BlastContainer const* const blast_container);
void draw_blast_containers(BlastContainer const blast_containers[]);
void update_blast(Blast* const blast, float const frame_delta);
void update_blast_container(BlastContainer* const blast_container, float const frame_delta);
void update_blast_containers(BlastContainer blast_containers[], float const frame_delta);

#endif
