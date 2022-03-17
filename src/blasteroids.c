#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define DISPLAY_OFFSET_X DISPLAY_WIDTH / 2
#define DISPLAY_OFFSET_Y DISPLAY_HEIGHT / 2
#define FPS 30

#define SPACESHIP_SCALE_FACTOR 20.0f
#define SPACESHIP_COLOR "hotpink"
#define SPACESHIP_VERTICES { \
	{ .x = -0.5f, .y = -0.5f }, \
	{ .x = 0.5f, .y = 0.0f }, \
	{ .x = -0.5f, .y = 0.5f }, \
}
#define SPACESHIP_LINE_THICKNESS 2.0f

#define ASTEROID_SCALE_FACTOR 20.0f
#define ASTEROID_COLOR "white"
#define ASTEROID_VERTICES { \
	{ .x = -0.5f, .y = 0.0f }, \
	{ .x = 0.0f, .y = 0.5f }, \
	{ .x = 0.5f, .y = 0.0f }, \
	{ .x = 0.0f, .y = -0.5f }, \
}
#define ASTEROID_LINE_THICKNESS 2.0f

#define FORWARD ALLEGRO_KEY_W
#define BACK ALLEGRO_KEY_S
#define LEFT ALLEGRO_KEY_A
#define RIGHT ALLEGRO_KEY_D

struct keystate {
	unsigned int is_pressed: 1;
	unsigned int needs_processing: 1;
};
typedef struct keystate KeyState;

struct point_2d {
	float x;
	float y;
};
typedef struct point_2d Point2D;
typedef Point2D Position;

struct display_coordinates {
	float x;
	float y;
};
typedef struct display_coordinates DisplayCoordinates;

struct world_coordinates {
	float x;
	float y;
};
typedef struct world_coordinates WorldCoordinates;

struct mathematical_coordinates {
	float x;
	float y;
};
typedef struct mathematical_coordinates MathematicalCoordinates;

struct circle {
	DisplayCoordinates center;
	float radius;
};
typedef struct circle Circle;

struct spaceship {
	WorldCoordinates world_coordinates;
	float heading;
	ALLEGRO_COLOR color;
	DisplayCoordinates vertices[3];
	Circle hitbox;
};
typedef struct spaceship Spaceship;

struct asteroid {
	WorldCoordinates world_coordinates;
	float angle_velocity;
	ALLEGRO_COLOR color;
	DisplayCoordinates vertices[5];
	Circle hitbox;
};
typedef struct asteroid Asteroid;

struct game_state {
	ALLEGRO_DISPLAY* display_ptr;
	ALLEGRO_EVENT_QUEUE* event_queue_ptr;
	ALLEGRO_TIMER* timer_ptr;
	KeyState key_states[ALLEGRO_KEY_MAX];
	Spaceship spaceship;
};
typedef struct game_state GameState;

void error(char* message);
void initialize_allegro();
void initialize_allegro_primitives();
void initialize_keyboard();
ALLEGRO_DISPLAY* initialize_display();
ALLEGRO_EVENT_QUEUE* initialize_event_queue();
ALLEGRO_TIMER* initialize_timer();
GameState initialize_game_state();
void destroy_game_state(GameState* game_state_ptr);
bool is_pressed_or_needs_processing(KeyState key_states[], int keycode);
void unset_needs_processing(KeyState key_states[]);
void add_point_2d(Position* self, Position* other);
void subtract_point_2d(Position* self, Position* other);
void draw_line(Position* start, Position* end, ALLEGRO_COLOR color, float thickness);
Spaceship initialize_spaceship();
void draw_spaceship(Spaceship* spaceship);
void update_spaceship(Spaceship* spaceship, KeyState key_states[], float frame_delta);
void map_world_coordinates_to_screen(WorldCoordinates* world_coordinates, float angle);
void map_display_coordinates_to_screen(DisplayCoordinates* display_coordinates, float angle);
WorldCoordinates map_display_coordinates_to_world_coordinates(DisplayCoordinates* display_coordinates);
DisplayCoordinates map_world_coordinates_to_display_coordinates(WorldCoordinates* world_coordinates);
MathematicalCoordinates map_display_coordinates_to_mathematical_coordinates(DisplayCoordinates* display_coordinates);
DisplayCoordinates map_mathematical_coordinates_to_display_coordinates(MathematicalCoordinates* mathematical_coordinates);
MathematicalCoordinates map_world_coordinates_to_mathematical_coordinates(WorldCoordinates* world_coordinates);
WorldCoordinates map_mathematical_coordinates_to_world_coordinates(MathematicalCoordinates* mathematical_coordinates);
Asteroid initialize_asteroid();
void draw_asteroid(Asteroid* asteroid);
void update_asteroid(Asteroid* asteroid, float frame_delta);

int main(int argc, char** argv) {

	initialize_allegro();
	initialize_allegro_primitives();
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
				// Update game objects
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_R) && red < 255) red++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_G) && green < 255) green++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_B) && blue < 255) blue++;
				if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_ESCAPE)) running = false;

				update_spaceship(&game_state.spaceship, game_state.key_states, (float)frame_delta);
				unset_needs_processing(game_state.key_states);

				//printf("Red is %i, green is %i, blue is %i\n", red, green, blue);

				needs_redraw = true;
				frame_delta = 0.0; // reset frame delta
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
			draw_spaceship(&game_state.spaceship);
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

	// Shut down primitives addon
	al_shutdown_primitives_addon();
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

void initialize_allegro_primitives() {
	if (!al_init_primitives_addon()) {
		error("Could not initialize allegro_primitives!");
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
		.spaceship = initialize_spaceship(),
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

void add_point_2d(Position* self, Position* other) {
	self->x += other->x;
	self->y += other->y;
}

void subtract_point_2d(Position* self, Position* other) {
	self->x -= other->x;
	self->y -= other->y;
}

void draw_line_in_display_coordinates(DisplayCoordinates* start, DisplayCoordinates* end, ALLEGRO_COLOR color, float thickness) {
	al_draw_line(start->x, start->y, end->x, end->y, color, thickness);
}

void draw_line_in_world_coordinates(WorldCoordinates* start, WorldCoordinates* end, ALLEGRO_COLOR color, float thickness) {
	al_draw_line(start->x, start->y, end->x, end->y, color, thickness);
}

Spaceship initialize_spaceship() {
	Spaceship spaceship = {
		.world_coordinates = { .x = (float)DISPLAY_OFFSET_X, .y = (float)DISPLAY_OFFSET_Y },
		.heading = 0.0f,
		.color = al_color_name(SPACESHIP_COLOR),
		.vertices = SPACESHIP_VERTICES,
		.hitbox = {
			{ .x = (float)DISPLAY_OFFSET_X, .y = (float)DISPLAY_OFFSET_Y },
			1.0f,
		}
	};

	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_scale_transform(&transform, SPACESHIP_SCALE_FACTOR, SPACESHIP_SCALE_FACTOR);
	al_transform_coordinates(&transform, &spaceship.vertices[0].x, &spaceship.vertices[0].y);
	al_transform_coordinates(&transform, &spaceship.vertices[1].x, &spaceship.vertices[1].y);
	al_transform_coordinates(&transform, &spaceship.vertices[2].x, &spaceship.vertices[2].y);

	return spaceship;
}

void draw_spaceship(Spaceship* spaceship) {
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, spaceship->heading);
	al_translate_transform(&transform, spaceship->world_coordinates.x, spaceship->world_coordinates.y);
	al_use_transform(&transform);
	draw_line_in_display_coordinates(&spaceship->vertices[0], &spaceship->vertices[1], spaceship->color, SPACESHIP_LINE_THICKNESS);
	draw_line_in_display_coordinates(&spaceship->vertices[1], &spaceship->vertices[2], spaceship->color, SPACESHIP_LINE_THICKNESS);
	draw_line_in_display_coordinates(&spaceship->vertices[2], &spaceship->vertices[0], spaceship->color, SPACESHIP_LINE_THICKNESS);
}

void update_spaceship(Spaceship* spaceship, KeyState key_states[], float frame_delta) {
	if (is_pressed_or_needs_processing(key_states, FORWARD)) {
		spaceship->world_coordinates.x += (float)cos(spaceship->heading) * (1.0f + frame_delta);
		spaceship->world_coordinates.y += (float)sin(spaceship->heading) * (1.0f + frame_delta);
	}
	if (is_pressed_or_needs_processing(key_states, BACK)) {
		spaceship->world_coordinates.x -= (float)cos(spaceship->heading) * (1.0f + frame_delta);
		spaceship->world_coordinates.y -= (float)sin(spaceship->heading) * (1.0f + frame_delta);
	}
	if (is_pressed_or_needs_processing(key_states, LEFT)) {
		spaceship->heading = (spaceship->heading - 0.1f * (1.0f + frame_delta));
	}
	if (is_pressed_or_needs_processing(key_states, RIGHT)) {
		spaceship->heading = (spaceship->heading + 0.1f * (1.0f + frame_delta));
	}

	spaceship->hitbox.center.x = spaceship->world_coordinates.x;
	spaceship->hitbox.center.y = spaceship->world_coordinates.y;

	printf("Heading is %f\n", spaceship->heading);

	map_world_coordinates_to_screen(&spaceship->world_coordinates, spaceship->heading);
}

void map_world_coordinates_to_screen(WorldCoordinates* world_coordinates, float angle) {
	if (world_coordinates->x < 0.0f) world_coordinates->x = (float)DISPLAY_WIDTH;
	if (world_coordinates->x > (float)DISPLAY_WIDTH) world_coordinates->x = 0.0f;
	if (world_coordinates->y < 0.0f) world_coordinates->y = (float)DISPLAY_HEIGHT;
	if (world_coordinates->y > (float)DISPLAY_HEIGHT) world_coordinates->y = 0.0f;
}

void map_display_coordinates_to_screen(DisplayCoordinates* display_coordinates, float angle) {
	WorldCoordinates world_coordinates = map_display_coordinates_to_world_coordinates(display_coordinates);
	map_world_coordinates_to_screen(&world_coordinates, angle);
	*display_coordinates = map_world_coordinates_to_display_coordinates(&world_coordinates);
}

WorldCoordinates map_display_coordinates_to_world_coordinates(DisplayCoordinates* display_coordinates) {
	WorldCoordinates world_coordinates = {
		.x = display_coordinates->x + (float)DISPLAY_OFFSET_X,
		.y = display_coordinates->y + (float)DISPLAY_OFFSET_Y,
	};
	return world_coordinates;
}

DisplayCoordinates map_world_coordinates_to_display_coordinates(WorldCoordinates* world_coordinates) {
	DisplayCoordinates display_coordinates = {
		.x = world_coordinates->x - (float)DISPLAY_OFFSET_X,
		.y = world_coordinates->y - (float)DISPLAY_OFFSET_Y,
	};
	return display_coordinates;
}

MathematicalCoordinates map_display_coordinates_to_mathematical_coordinates(DisplayCoordinates* display_coordinates) {
	MathematicalCoordinates mathematical_coordinates = {
		.x = display_coordinates->x,
		.y = (float)DISPLAY_HEIGHT - display_coordinates->y,
	};
	return mathematical_coordinates;
}

DisplayCoordinates map_mathematical_coordinates_to_display_coordinates(MathematicalCoordinates* mathematical_coordinates) {
	DisplayCoordinates display_coordinates = {
		.x = mathematical_coordinates->x,
		.y = (float)DISPLAY_HEIGHT - mathematical_coordinates->y,
	};
	return display_coordinates;
}

MathematicalCoordinates map_world_coordinates_to_mathematical_coordinates(WorldCoordinates* world_coordinates) {
	MathematicalCoordinates mathematical_coordinates = {
		.x = world_coordinates->x - (float)DISPLAY_OFFSET_X,
		.y = world_coordinates->y - (float)DISPLAY_OFFSET_Y,
	};
	return mathematical_coordinates;
}

WorldCoordinates map_mathematical_coordinates_to_world_coordinates(MathematicalCoordinates* mathematical_coordinates) {
	WorldCoordinates world_coordinates = {
		.x = mathematical_coordinates->x + (float)DISPLAY_OFFSET_X,
		.y = mathematical_coordinates->y + (float)DISPLAY_OFFSET_Y,
	};
	return world_coordinates;
}
