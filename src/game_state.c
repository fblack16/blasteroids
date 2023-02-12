#include "../include/game_state.h"
#include "../include/collision.h"
#include "../include/initialization.h"
#include <allegro5/keycodes.h>
#include <stdbool.h>

GameState initialize_game_state() {
  GameState game_state = {
      .display_ptr = initialize_display(),
      .event_queue_ptr = initialize_event_queue(),
      .timer_ptr = initialize_timer(),
      .spaceship = initialize_spaceship(),
      .is_blast_on_cooldown = false,
      .is_asteroid_creation_on_cooldown = false,
  };

  KeyState const key_state = {.is_pressed = false, .needs_processing = false};
  for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
    game_state.key_states[i] = key_state;
  }

  for (int i = 0; i < ASTEROID_MAX; ++i) {
    Asteroid asteroid = initialize_asteroid();
    game_state.asteroid_containers[i] =
        initialize_asteroid_container(&asteroid);
  }

  BlastContainer const blast_container = {.is_in_use = false};
  for (int i = 0; i < BLAST_MAX; ++i) {
    game_state.blast_containers[i] = blast_container;
  }

  return game_state;
}

void destroy_game_state(GameState *game_state_ptr) {
  al_destroy_display(game_state_ptr->display_ptr);
  al_destroy_event_queue(game_state_ptr->event_queue_ptr);
  al_destroy_timer(game_state_ptr->timer_ptr);
}

void fire_blast(GameState *game_state) {
  if (is_pressed_or_needs_processing(game_state->key_states, FIRE)) {
    for (int i = 0; i < BLAST_MAX; ++i) {
      if (!game_state->blast_containers[i].is_in_use &&
          !game_state->is_blast_on_cooldown) {
        game_state->blast_containers[i] = initialize_blast_container(
            game_state->spaceship.coordinates, game_state->spaceship.heading);
        game_state->is_blast_on_cooldown = true;
      }
    }
  }
}

void check_collision_between_blasts_and_asteroids(GameState *game_state) {
  for (int i = 0; i < BLAST_MAX; ++i) {
    if (game_state->blast_containers[i].is_in_use) {
      for (int j = 0; j < ASTEROID_MAX; ++j) {
        if (game_state->asteroid_containers[j].is_in_use) {
          bool is_collision = circles_collide(
              &game_state->blast_containers[i].blast.hitbox,
              &game_state->asteroid_containers[j].asteroid.hitbox);
          if (is_collision) {
            game_state->blast_containers[i].is_in_use = false;
            game_state->blast_containers[j].is_in_use = false;
          }
        }
      }
    }
  }
}

void create_new_asteroids(GameState *game_state) {
    if (!game_state->is_asteroid_creation_on_cooldown) {
        for (int i = 0; i < ASTEROID_MAX; ++i) {
            Asteroid asteroid = initialize_asteroid();
            game_state->asteroid_containers[i] = initialize_asteroid_container(&asteroid);
            game_state->is_asteroid_creation_on_cooldown = true;
        }
    }
}
