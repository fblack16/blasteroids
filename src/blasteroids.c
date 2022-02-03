#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <allegro5/allegro.h>

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480

void error(char* message) {
	fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

int main(int argc, char** argv) {

	// Initialize allegro
	if (!al_init()) {
		error("Could not initialize allegro!");
		exit(1);
	}

	// Initialize keyboard
	if (!al_install_keyboard()) {
		error("Could not initialize keyboard!");
		exit(1);
	}

	// Create window
	ALLEGRO_DISPLAY* display_ptr = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if (!display_ptr) {
		error("Could not create window!");
		exit(1);
	}

	// Create event queue
	ALLEGRO_EVENT_QUEUE* event_queue_ptr = al_create_event_queue();
	if (!event_queue_ptr) {
		error("Could not create event queue!");
		exit(1);
	}

	// Register keyboard as event source with the event queue
	al_register_event_source(event_queue_ptr, al_get_keyboard_event_source());

	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;
	unsigned char alpha = 0;

	ALLEGRO_KEYBOARD_STATE keyboard_state;

	while (true) {
		al_get_keyboard_state(&keyboard_state);
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_ESCAPE)) {
			break;
		}
		al_clear_to_color(al_map_rgba(red, green, blue, alpha));
		al_flip_display();
	}

	// Destroy event queue
	al_destroy_event_queue(event_queue_ptr);

	// Destroy window
	al_destroy_display(display_ptr);

	// Uninstall allegro
	al_uninstall_system();
}
