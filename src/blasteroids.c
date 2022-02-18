#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <allegro5/allegro.h>

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define FPS 30

struct keystate {
	unsigned int is_pressed: 1;
	unsigned int needs_processing: 1;
};
typedef struct keystate KeyState;

struct game_state {
	ALLEGRO_DISPLAY* display_ptr;
	ALLEGRO_EVENT_QUEUE* event_queue_ptr;
	ALLEGRO_TIMER* timer_ptr;
	KeyState key_states[ALLEGRO_KEY_MAX];
};
typedef struct game_state GameState;

void error(char* message);
void initialize_allegro();
void initialize_keyboard();
ALLEGRO_DISPLAY* initialize_display();
ALLEGRO_EVENT_QUEUE* initialize_event_queue();
ALLEGRO_TIMER* initialize_timer();
GameState initialize_game_state();
void destroy_game_state(GameState* game_state_ptr);
bool is_pressed_or_needs_processing(KeyState key_states[], int keycode);
void unset_needs_processing(KeyState key_states[]);

int main(int argc, char** argv) {

	initialize_allegro();
	initialize_keyboard();

	GameState game_state = initialize_game_state();
	al_register_event_source(game_state.event_queue_ptr, al_get_keyboard_event_source());
	al_register_event_source(game_state.event_queue_ptr, al_get_timer_event_source(game_state.timer_ptr));

	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;
	unsigned char alpha = 0;

	ALLEGRO_EVENT event;

	bool running = true;
	bool needs_redraw = true;
	double frame_delta = 0.0;
	al_start_timer(game_state.timer_ptr);
	while (running) {
		al_wait_for_event(game_state.event_queue_ptr, &event);

		double start_time = al_get_time();
		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_R) && red < 255) red++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_G) && green < 255) green++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_B) && blue < 255) blue++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_ESCAPE)) running = false;
				unset_needs_processing(game_state.key_states);

				printf("Red is %i, green is %i, blue is %i\n", red, green, blue);

				needs_redraw = true;
				break;

			case ALLEGRO_EVENT_KEY_DOWN:
				game_state.key_states[event.keyboard.keycode].is_pressed = 1;
				game_state.key_states[event.keyboard.keycode].needs_processing = 1;
				break;

			case ALLEGRO_EVENT_KEY_UP:
				game_state.key_states[event.keyboard.keycode].is_pressed = 0;
				break;

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
		}
		
		// Idea to deal with frame skips: If all of the computation takes less than one tick of the
		// timer, this means that all the computation could be done within one frame, since the timer
		// ticks once every frame. If we need more than one tick = one frame, we should see that in the
		// difference between current_number of ticks and start_number_of_ticks.
		// Then, we need to multiply all game computations (positions of objects, speeds) by the delta,
		// and would need to adjust the timer queue.
		// Problem: the difference in timer ticks only reflects whole frames, but not fractional frames.
		if (needs_redraw && al_event_queue_is_empty(game_state.event_queue_ptr)) {
			al_clear_to_color(al_map_rgba(red, green, blue, alpha));
			al_flip_display();
			needs_redraw = false;
		}
		double end_time = al_get_time();
		double frame_time = end_time - start_time;
		if (frame_time > ALLEGRO_BPS_TO_SECS(FPS)) {
			printf("Frameskip must occur!\n");
			frame_delta = frame_time - ALLEGRO_BPM_TO_SECS(FPS);
			printf("Frame delta is %.4f\n", frame_delta);
		}
		printf("Frame time: %.4f\n", frame_time);
	}

	destroy_game_state(&game_state);

	// Uninstall allegro
	al_uninstall_system();
}

void error(char* message) {
	fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

void initialize_allegro() {
	if (!al_init()) {
		error("Could not initialize allegro!");
		exit(1);
	}
}

void initialize_keyboard() {
	if (!al_install_keyboard()) {
		error("Could not initialize keyboard!");
		exit(1);
	}
}

ALLEGRO_DISPLAY* initialize_display() {
	ALLEGRO_DISPLAY* display_ptr = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if (!display_ptr) {
		error("Could not create window!");
		exit(1);
	}
	return display_ptr;
}

ALLEGRO_EVENT_QUEUE* initialize_event_queue() {
	ALLEGRO_EVENT_QUEUE* event_queue_ptr = al_create_event_queue();
	if (!event_queue_ptr) {
		error("Could not create event queue!");
		exit(1);
	}
	return event_queue_ptr;
}

ALLEGRO_TIMER* initialize_timer() {
	ALLEGRO_TIMER* timer_ptr = al_create_timer(ALLEGRO_BPS_TO_SECS(FPS));
	if (!timer_ptr) {
		error("Could not create timer!");
		exit(1);
	}
	return timer_ptr;
}

GameState initialize_game_state() {
	GameState game_state = {
		.display_ptr = initialize_display(),
		.event_queue_ptr = initialize_event_queue(),
		.timer_ptr = initialize_timer(),
	};
	KeyState key_state = {.is_pressed = 0, .needs_processing = 0};
	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
		game_state.key_states[i] = key_state;
	}
	return game_state;
}

void destroy_game_state(GameState* game_state_ptr) {
	al_destroy_display(game_state_ptr->display_ptr);
	al_destroy_event_queue(game_state_ptr->event_queue_ptr);
	al_destroy_timer(game_state_ptr->timer_ptr);
}

bool is_pressed_or_needs_processing(KeyState key_states[], int keycode) {
	if (keycode < 0 || keycode >= ALLEGRO_KEY_MAX) {
		error("Keycode is out of range!");
		exit(1);
	}
	return (key_states[keycode].is_pressed || key_states[keycode].needs_processing);
}

void unset_needs_processing(KeyState key_states[]) {
	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
		key_states[i].needs_processing = 0;
	}
}
