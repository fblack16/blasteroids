#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <allegro5/allegro.h>

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define FPS 30

struct key {
	unsigned int is_pressed: 1;
	unsigned int needs_processing: 1;
};
typedef struct key Key;

void error(char* message);
void initialize_allegro();
void initialize_keyboard();
ALLEGRO_DISPLAY* initialize_display();
ALLEGRO_EVENT_QUEUE* initialize_event_queue();

int main(int argc, char** argv) {

	initialize_allegro();
	initialize_keyboard();
	ALLEGRO_DISPLAY* display_ptr = initialize_display();

	ALLEGRO_EVENT_QUEUE* event_queue_ptr = initialize_event_queue();
	ALLEGRO_TIMER* timer_ptr = al_create_timer(ALLEGRO_BPM_TO_SECS(FPS));
	// Register keyboard as event source with the event queue
	al_register_event_source(event_queue_ptr, al_get_keyboard_event_source());
	al_register_event_source(event_queue_ptr, al_get_timer_event_source(timer_ptr));

	// Keypress array
	Key keys[ALLEGRO_KEY_MAX];
	Key key = {.is_pressed = 0, .needs_processing = 0};
	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
		keys[i] = key;
	}

	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;
	unsigned char alpha = 0;

	ALLEGRO_EVENT event;

	bool running = true;
	bool needs_redraw = true;
	al_start_timer(timer_ptr);
	while (running) {
		al_wait_for_event(event_queue_ptr, &event);

		double start_time = al_get_time();
		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				if ((keys[ALLEGRO_KEY_R].is_pressed || keys[ALLEGRO_KEY_R].needs_processing) && red < 255) red++;
				if ((keys[ALLEGRO_KEY_G].is_pressed || keys[ALLEGRO_KEY_G].needs_processing) && green < 255) green++;
				if ((keys[ALLEGRO_KEY_B].is_pressed || keys[ALLEGRO_KEY_B].needs_processing) && blue < 255) blue++;
				if (keys[ALLEGRO_KEY_ESCAPE].is_pressed || keys[ALLEGRO_KEY_ESCAPE].needs_processing) running = false;

				for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
					keys[i].needs_processing = 0;
				}
				needs_redraw = true;
				break;

			case ALLEGRO_EVENT_KEY_DOWN:
				keys[event.keyboard.keycode].is_pressed = 1;
				keys[event.keyboard.keycode].needs_processing = 1;
				break;

			case ALLEGRO_EVENT_KEY_UP:
				keys[event.keyboard.keycode].is_pressed = 0;
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
		if (needs_redraw && al_event_queue_is_empty(event_queue_ptr)) {
			al_clear_to_color(al_map_rgba(red, green, blue, alpha));
			al_flip_display();
			needs_redraw = false;
		}
		double end_time = al_get_time();
		double frame_time = end_time - start_time;
		if (frame_time > ALLEGRO_BPM_TO_SECS(FPS)) printf("Frameskip must occur!");
		printf("Frame time: %.4f\n", frame_time);
	}

	// Destroy event queue
	al_destroy_event_queue(event_queue_ptr);

	// Destroy window
	al_destroy_display(display_ptr);

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
