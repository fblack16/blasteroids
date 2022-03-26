#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

struct display_coordinates {
	float x;
	float y;
};
typedef struct display_coordinates DisplayCoordinates;

const int DISPLAY_WIDTH = 640;
const int DISPLAY_HEIGHT = 480;
const int DISPLAY_OFFSET_X = (DISPLAY_WIDTH / 2);
const int DISPLAY_OFFSET_Y = (DISPLAY_HEIGHT / 2);
const int FPS = 30;

const float SPACESHIP_SCALE_FACTOR = 20.0f;
const char SPACESHIP_COLOR[] = "hotpink";
#define NUMBER_OF_SPACESHIP_VERTICES 3
const DisplayCoordinates SPACESHIP_VERTICES[NUMBER_OF_SPACESHIP_VERTICES] = {
	{ .x = -0.5f * SPACESHIP_SCALE_FACTOR, .y = -0.5f * SPACESHIP_SCALE_FACTOR},
	{ .x = 0.5f * SPACESHIP_SCALE_FACTOR, .y = 0.0f * SPACESHIP_SCALE_FACTOR},
	{ .x = -0.5f * SPACESHIP_SCALE_FACTOR, .y = 0.5f * SPACESHIP_SCALE_FACTOR},
};
const float SPACESHIP_LINE_THICKNESS = 2.0f;

const float ASTEROID_SCALE_FACTOR = 20.0f;
const char ASTEROID_COLOR[] = "white";
#define NUMBER_OF_ASTEROID_VERTICES 4
const DisplayCoordinates ASTEROID_VERTICES[NUMBER_OF_ASTEROID_VERTICES] = {
	{ .x = -0.5f * ASTEROID_SCALE_FACTOR, .y = 0.0f * ASTEROID_SCALE_FACTOR},
	{ .x = 0.0f * ASTEROID_SCALE_FACTOR, .y = 0.5f * ASTEROID_SCALE_FACTOR},
	{ .x = 0.5f * ASTEROID_SCALE_FACTOR, .y = 0.0f * ASTEROID_SCALE_FACTOR},
	{ .x = 0.0f * ASTEROID_SCALE_FACTOR, .y = -0.5f * ASTEROID_SCALE_FACTOR},
};
const float ASTEROID_LINE_THICKNESS = 2.0f;
#define ASTEROID_MAX 10

const float BLAST_SCALE_FACTOR = 20.0f;
const char BLAST_COLOR[] = "red";
#define NUMBER_OF_BLAST_VERTICES 2
const DisplayCoordinates BLAST_VERTICES[NUMBER_OF_BLAST_VERTICES] = {
	{ .x = 0.0f * BLAST_SCALE_FACTOR, .y = 0.0f * BLAST_SCALE_FACTOR},
	{ .x = 1.0f * BLAST_SCALE_FACTOR, .y = 0.0f * BLAST_SCALE_FACTOR},
};
const float BLAST_LINE_THICKNESS = 2.0f;
const float BLAST_VELOCITY = 2.0f;
#define BLAST_MAX 10


const int FORWARD = ALLEGRO_KEY_W;
const int BACK = ALLEGRO_KEY_S;
const int LEFT = ALLEGRO_KEY_A;
const int RIGHT = ALLEGRO_KEY_D;
const int FIRE = ALLEGRO_KEY_SPACE;

struct keystate {
	unsigned int is_pressed: 1;
	unsigned int needs_processing: 1;
};
typedef struct keystate KeyState;

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
	WorldCoordinates center;
	float radius;
};
typedef struct circle Circle;

struct spaceship {
	WorldCoordinates world_coordinates;
	float heading;
	ALLEGRO_COLOR color;
	const DisplayCoordinates* vertices;
	Circle hitbox;
};
typedef struct spaceship Spaceship;

struct asteroid {
	WorldCoordinates world_coordinates;
	float heading;
	float rotation_angle;
	float angle_velocity;
	ALLEGRO_COLOR color;
	const DisplayCoordinates* vertices;
	Circle hitbox;
};
typedef struct asteroid Asteroid;

struct asteroid_container {
	bool is_in_use;
	Asteroid asteroid;
};
typedef struct asteroid_container AsteroidContainer;

struct blast {
	WorldCoordinates world_coordinates;
	float heading;
	ALLEGRO_COLOR color;
	const DisplayCoordinates* vertices;
	Circle hitbox;
};
typedef struct blast Blast;

struct blast_container {
	bool is_in_use;
	Blast blast;
};
typedef struct blast_container BlastContainer;

struct blast_timer {
	ALLEGRO_EVENT_SOURCE event_source;
};
typedef struct blast_timer BLAST_TIMER;

enum {
	EVENT_BLAST_TIMER = 1024,
};

struct game_state {
	ALLEGRO_DISPLAY* display_ptr;
	ALLEGRO_EVENT_QUEUE* event_queue_ptr;
	ALLEGRO_TIMER* timer_ptr;
	KeyState key_states[ALLEGRO_KEY_MAX];
	Spaceship spaceship;
	AsteroidContainer asteroid_containers[ASTEROID_MAX];
	BlastContainer blast_containers[BLAST_MAX];
	bool is_blast_on_cooldown;
	bool is_asteroid_creation_on_cooldown;
};
typedef struct game_state GameState;

void error(const char* message);
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
Spaceship initialize_spaceship();
void draw_spaceship(Spaceship* spaceship);
void update_spaceship(Spaceship* spaceship, KeyState key_states[], float frame_delta);
void map_world_coordinates_to_screen(WorldCoordinates* world_coordinates, float angle);
void map_display_coordinates_to_screen(DisplayCoordinates* display_coordinates, float angle);
WorldCoordinates map_display_coordinates_to_world_coordinates(const DisplayCoordinates* display_coordinates);
DisplayCoordinates map_world_coordinates_to_display_coordinates(const WorldCoordinates* world_coordinates);
MathematicalCoordinates map_display_coordinates_to_mathematical_coordinates(DisplayCoordinates* display_coordinates);
DisplayCoordinates map_mathematical_coordinates_to_display_coordinates(MathematicalCoordinates* mathematical_coordinates);
MathematicalCoordinates map_world_coordinates_to_mathematical_coordinates(WorldCoordinates* world_coordinates);
WorldCoordinates map_mathematical_coordinates_to_world_coordinates(MathematicalCoordinates* mathematical_coordinates);
Asteroid initialize_asteroid();
void draw_asteroid(AsteroidContainer* asteroid_container);
void draw_asteroids(AsteroidContainer asteroid_containers[]);
void update_asteroid(AsteroidContainer* asteroid_container, float frame_delta);
void update_asteroids(AsteroidContainer asteroid_containers[], float frame_delta);
bool circles_collide_world_coordinates(Circle* first, Circle* second);
bool circles_collide_display_coordinates(Circle* first, Circle* second);
WorldCoordinates subtract_world_coordinates(WorldCoordinates* first, WorldCoordinates* second);
float absolute_value_of_world_coordinates(WorldCoordinates* world_coordinates);
DisplayCoordinates subtract_display_coordinates(DisplayCoordinates* first, DisplayCoordinates* second);
float absolute_value_of_display_coordinates(DisplayCoordinates* display_coordinates);
AsteroidContainer initialize_asteroid_container(Asteroid* asteroid);
void fill_asteroid_container(AsteroidContainer* asteroid_container, Asteroid* asteroid);
void draw_line_in_display_coordinates(const DisplayCoordinates* start, const DisplayCoordinates* end, ALLEGRO_COLOR color, float thickness);
void draw_line_in_world_coordinates(const WorldCoordinates* start, const WorldCoordinates* end, ALLEGRO_COLOR color, float thickness);
void draw_circle_in_display_coordinates(const DisplayCoordinates* center, float radius, ALLEGRO_COLOR color, float thickness);
BlastContainer initialize_blast_container(WorldCoordinates* world_coordinates, float heading);
void fill_blast_container(BlastContainer* blast_container, Blast* blast);
void draw_blast(BlastContainer* blast);
void draw_blasts(BlastContainer blast_containers[]);
void update_blast(BlastContainer* blast, float frame_delta);
void update_blasts(BlastContainer blast_containers[], float frame_delta);
void fire_blast(GameState* game_state);
void check_collision_between_blasts_and_asteroids(GameState* game_state);
void create_new_asteroids(GameState* game_state);

BLAST_TIMER* create_blast_timer();
void destroy_blast_timer(BLAST_TIMER* blast_timer);

int main(int argc, char** argv) {

	// seed random number generator
	srand(0);

	initialize_allegro();
	initialize_allegro_primitives();
	initialize_keyboard();

	GameState game_state = initialize_game_state();
	al_register_event_source(game_state.event_queue_ptr, al_get_keyboard_event_source());
	al_register_event_source(game_state.event_queue_ptr, al_get_timer_event_source(game_state.timer_ptr));

	ALLEGRO_EVENT event;

	ALLEGRO_TIMER* blast_timer = al_create_timer(0.5f);
	al_register_event_source(game_state.event_queue_ptr, al_get_timer_event_source(blast_timer));

	ALLEGRO_TIMER* asteroid_creation_timer = al_create_timer(2.0f);
	al_register_event_source(game_state.event_queue_ptr, al_get_timer_event_source(asteroid_creation_timer));

	bool running = true;
	bool needs_redraw = true;
	double frame_delta = 0.0;
	al_start_timer(game_state.timer_ptr);
	al_start_timer(blast_timer);
	al_start_timer(asteroid_creation_timer);

	while (running) {
		al_wait_for_event(game_state.event_queue_ptr, &event);

		double start_time = al_get_time();
		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				if (event.timer.source == game_state.timer_ptr) {
					if (is_pressed_or_needs_processing(game_state.key_states, ALLEGRO_KEY_ESCAPE)) running = false;

					update_spaceship(&game_state.spaceship, game_state.key_states, (float)frame_delta);
					update_asteroids(game_state.asteroid_containers, (float)frame_delta);
					//fire_blast(&game_state);
					update_blasts(game_state.blast_containers, (float)frame_delta);
					fire_blast(&game_state);
					check_collision_between_blasts_and_asteroids(&game_state);
					create_new_asteroids(&game_state);
					unset_needs_processing(game_state.key_states);

					needs_redraw = true;
					frame_delta = 0.0; // reset frame delta
				}
				if (event.timer.source == blast_timer) {
					game_state.is_blast_on_cooldown = false;
				}
				if (event.timer.source == asteroid_creation_timer) {
					game_state.is_asteroid_creation_on_cooldown = false;
				}
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
			al_clear_to_color(al_color_name("black"));
			draw_spaceship(&game_state.spaceship);
			draw_asteroids(game_state.asteroid_containers);
			draw_blasts(game_state.blast_containers);
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

void error(const char* message) {
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
		.is_blast_on_cooldown = false,
		.is_asteroid_creation_on_cooldown = false,
	};

	KeyState key_state = {.is_pressed = 0, .needs_processing = 0};
	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i) {
		game_state.key_states[i] = key_state;
	}

	for (int i = 0; i < ASTEROID_MAX; ++i) {
		Asteroid asteroid = initialize_asteroid();
		game_state.asteroid_containers[i] = initialize_asteroid_container(&asteroid);
	}

	BlastContainer blast_container = { .is_in_use = false };
	for (int i = 0; i < BLAST_MAX; ++i) {
		game_state.blast_containers[i] = blast_container;
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

void draw_line_in_display_coordinates(const DisplayCoordinates* start, const DisplayCoordinates* end, ALLEGRO_COLOR color, float thickness) {
	al_draw_line(start->x, start->y, end->x, end->y, color, thickness);
}

void draw_line_in_world_coordinates(const WorldCoordinates* start, const WorldCoordinates* end, ALLEGRO_COLOR color, float thickness) {
	const DisplayCoordinates start_as_display_coordinates = map_world_coordinates_to_display_coordinates(start);
	const DisplayCoordinates end_as_display_coordinates = map_world_coordinates_to_display_coordinates(end);

	draw_line_in_display_coordinates(&start_as_display_coordinates, &end_as_display_coordinates, color, thickness);
}

Spaceship initialize_spaceship() {
	Spaceship spaceship = {
		.world_coordinates = { .x = 0.0f, .y = 0.0f },
		.heading = 0.0f,
		.color = al_color_name(SPACESHIP_COLOR),
		.vertices = SPACESHIP_VERTICES,
		.hitbox = {
			.center = { 
				.x = 0.0f, 
				.y = 0.0f,
			},
			.radius = 1.0f * SPACESHIP_SCALE_FACTOR,
		}
	};

	return spaceship;
}

void draw_spaceship(Spaceship* spaceship) {
	DisplayCoordinates display_coordinates = map_world_coordinates_to_display_coordinates(&spaceship->world_coordinates);
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, -spaceship->heading); // need to take the negative of heading since drawing is in display coordinates
	al_translate_transform(&transform, display_coordinates.x, display_coordinates.y);
	al_use_transform(&transform);

	draw_line_in_display_coordinates(&spaceship->vertices[NUMBER_OF_SPACESHIP_VERTICES-1], &spaceship->vertices[0], spaceship->color, SPACESHIP_LINE_THICKNESS);
	for (int i = 0; i < NUMBER_OF_SPACESHIP_VERTICES - 1; ++i) {
		draw_line_in_display_coordinates(&spaceship->vertices[i], &spaceship->vertices[i+1], spaceship->color, SPACESHIP_LINE_THICKNESS);
	}

	al_identity_transform(&transform);
	al_use_transform(&transform);
	DisplayCoordinates hitbox_coordinates = map_world_coordinates_to_display_coordinates(&spaceship->hitbox.center);
	draw_circle_in_display_coordinates(&hitbox_coordinates, spaceship->hitbox.radius, al_color_name("red"), 2.0f);
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
		spaceship->heading = (spaceship->heading + 0.1f * (1.0f + frame_delta));
	}
	if (is_pressed_or_needs_processing(key_states, RIGHT)) {
		spaceship->heading = (spaceship->heading - 0.1f * (1.0f + frame_delta));
	}

	map_world_coordinates_to_screen(&spaceship->world_coordinates, spaceship->heading);

	spaceship->hitbox.center.x = spaceship->world_coordinates.x;
	spaceship->hitbox.center.y = spaceship->world_coordinates.y;
}

void map_world_coordinates_to_screen(WorldCoordinates* world_coordinates, float angle) {
	if (world_coordinates->x < -(float)DISPLAY_OFFSET_X) world_coordinates->x = (float)DISPLAY_OFFSET_X;
	if (world_coordinates->x > (float)DISPLAY_OFFSET_X) world_coordinates->x = -(float)DISPLAY_OFFSET_X;
	if (world_coordinates->y < -(float)DISPLAY_OFFSET_Y) world_coordinates->y = (float)DISPLAY_OFFSET_Y;
	if (world_coordinates->y > (float)DISPLAY_OFFSET_Y) world_coordinates->y = -(float)DISPLAY_OFFSET_Y;
}

void map_display_coordinates_to_screen(DisplayCoordinates* display_coordinates, float angle) {
	if (display_coordinates->x < 0.0f) display_coordinates->x = (float)DISPLAY_WIDTH;
	if (display_coordinates->x > (float)DISPLAY_WIDTH) display_coordinates->x = 0.0f;
	if (display_coordinates->y < 0.0f) display_coordinates->y = (float)DISPLAY_HEIGHT;
	if (display_coordinates->y > (float)DISPLAY_HEIGHT) display_coordinates->y = 0.0f;
}

WorldCoordinates map_display_coordinates_to_world_coordinates(const DisplayCoordinates* display_coordinates) {
	WorldCoordinates world_coordinates = {
		.x = display_coordinates->x - (float)DISPLAY_OFFSET_X,
		.y = -display_coordinates->y - (float)DISPLAY_OFFSET_Y,
	};
	return world_coordinates;
}

DisplayCoordinates map_world_coordinates_to_display_coordinates(const WorldCoordinates* world_coordinates) {
	DisplayCoordinates display_coordinates = {
		.x = world_coordinates->x + (float)DISPLAY_OFFSET_X,
		.y = -world_coordinates->y + (float)DISPLAY_OFFSET_Y,
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
		.x = world_coordinates->x + (float)DISPLAY_OFFSET_X,
		.y = world_coordinates->y + (float)DISPLAY_OFFSET_Y,
	};
	return mathematical_coordinates;
}

WorldCoordinates map_mathematical_coordinates_to_world_coordinates(MathematicalCoordinates* mathematical_coordinates) {
	WorldCoordinates world_coordinates = {
		.x = mathematical_coordinates->x - (float)DISPLAY_OFFSET_X,
		.y = mathematical_coordinates->y - (float)DISPLAY_OFFSET_Y,
	};
	return world_coordinates;
}

Asteroid initialize_asteroid() {
	// Should be an enum
	int direction = rand() % 4; // 0, 1, 2, 3
	WorldCoordinates world_coordinates = { .x = 0.0f, .y = 0.0f };
	switch (direction) {
		case 0:
			world_coordinates.x = 10.0f - DISPLAY_OFFSET_X;
			world_coordinates.y = (float)(rand() % DISPLAY_HEIGHT - DISPLAY_OFFSET_Y);
		case 1:
			world_coordinates.x = DISPLAY_OFFSET_X - 10.0f;
			world_coordinates.y = (float)(rand() % DISPLAY_HEIGHT - DISPLAY_OFFSET_Y);
		case 2:
			world_coordinates.x = (float)(rand() % DISPLAY_WIDTH - DISPLAY_OFFSET_X);
			world_coordinates.y = 10.0f - DISPLAY_OFFSET_Y;
		case 3:
			world_coordinates.x = (float)(rand() % DISPLAY_WIDTH - DISPLAY_OFFSET_X);
			world_coordinates.y = DISPLAY_OFFSET_Y - 10.0f;
	}

	Asteroid asteroid = {
		.world_coordinates = world_coordinates,
		.heading = (float)(rand() % 36),
		.rotation_angle = 0.0f,
		.angle_velocity = 0.1f,
		.color = al_color_name(ASTEROID_COLOR),
		.vertices = ASTEROID_VERTICES,
		.hitbox = {
			.center = world_coordinates,
			.radius = 1.0f * ASTEROID_SCALE_FACTOR,
		},
	};

	return asteroid;
}

void draw_asteroid(AsteroidContainer* asteroid_container) {
	DisplayCoordinates display_coordinates = map_world_coordinates_to_display_coordinates(&asteroid_container->asteroid.world_coordinates);
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, asteroid_container->asteroid.rotation_angle);
	al_translate_transform(&transform, display_coordinates.x, display_coordinates.y);
	al_use_transform(&transform);

	draw_line_in_display_coordinates(&asteroid_container->asteroid.vertices[NUMBER_OF_ASTEROID_VERTICES-1], &asteroid_container->asteroid.vertices[0], asteroid_container->asteroid.color, ASTEROID_LINE_THICKNESS);
	for (int i = 0; i < NUMBER_OF_ASTEROID_VERTICES - 1; ++i) {
		draw_line_in_display_coordinates(&asteroid_container->asteroid.vertices[i], &asteroid_container->asteroid.vertices[i+1], asteroid_container->asteroid.color, ASTEROID_LINE_THICKNESS);
	}

	al_identity_transform(&transform);
	al_use_transform(&transform);
	DisplayCoordinates hitbox_coordinates = map_world_coordinates_to_display_coordinates(&asteroid_container->asteroid.hitbox.center);
	draw_circle_in_display_coordinates(&hitbox_coordinates, asteroid_container->asteroid.hitbox.radius, al_color_name("green"), 2.0f);
}

void draw_asteroids(AsteroidContainer asteroid_containers[]) {
	for (int i = 0; i < ASTEROID_MAX; ++i) {
		if (asteroid_containers[i].is_in_use) {
			draw_asteroid(asteroid_containers + i);
		}
	}
}

void update_asteroid(AsteroidContainer* asteroid_container, float frame_delta) {
	asteroid_container->asteroid.world_coordinates.x += (float)cos(asteroid_container->asteroid.heading) * (1.0f + frame_delta);
	asteroid_container->asteroid.world_coordinates.y += (float)sin(asteroid_container->asteroid.heading) * (1.0f + frame_delta);
	asteroid_container->asteroid.rotation_angle += asteroid_container->asteroid.angle_velocity;

	map_world_coordinates_to_screen(&asteroid_container->asteroid.world_coordinates, asteroid_container->asteroid.heading);

	asteroid_container->asteroid.hitbox.center.x = asteroid_container->asteroid.world_coordinates.x;
	asteroid_container->asteroid.hitbox.center.y = asteroid_container->asteroid.world_coordinates.y;
}

void update_asteroids(AsteroidContainer asteroid_containers[], float frame_delta) {
	for (int i = 0; i < ASTEROID_MAX; ++i) {
		if (asteroid_containers[i].is_in_use) {
			update_asteroid(&asteroid_containers[i], frame_delta);
		}
	}
}

bool circles_collide_world_coordinates(Circle* first, Circle* second) {
	WorldCoordinates difference_of_centers = subtract_world_coordinates(&first->center, &second->center);
	float distance_of_centers = absolute_value_of_world_coordinates(&difference_of_centers);
	float sum_of_radii = first->radius + second->radius;
	//printf("Sum of radii is %f\n", sum_of_radii);
	//printf("Distance of centers is %f\n", distance_of_centers);
	return distance_of_centers < sum_of_radii;
}

bool circles_collide_display_coordinates(Circle* first, Circle* second) {
	DisplayCoordinates first_center_in_display_coordinates = map_world_coordinates_to_display_coordinates(&first->center);
	DisplayCoordinates second_center_in_display_coordinates = map_world_coordinates_to_display_coordinates(&second->center);

	DisplayCoordinates difference_of_centers = subtract_display_coordinates(&first_center_in_display_coordinates, &second_center_in_display_coordinates);
	float distance_of_centers = absolute_value_of_display_coordinates(&difference_of_centers);
	float sum_of_radii = first->radius + second->radius;
	//printf("Sum of radii is %f\n", sum_of_radii);
	//printf("Distance of centers is %f\n", distance_of_centers);
	return distance_of_centers < sum_of_radii;
}

DisplayCoordinates subtract_display_coordinates(DisplayCoordinates* first, DisplayCoordinates* second) {
	DisplayCoordinates difference = {
		.x = first->x - second->x,
		.y = first->y - second->y,
	};
	return difference;
}

WorldCoordinates subtract_world_coordinates(WorldCoordinates* first, WorldCoordinates* second) {
	WorldCoordinates difference = {
		.x = first->x - second->x,
		.y = first->y - second->y,
	};
	return difference;
}

float absolute_value_of_world_coordinates(WorldCoordinates* world_coordinates) {
	return sqrt(world_coordinates->x * world_coordinates->x + world_coordinates->y * world_coordinates->y);
}

float absolute_value_of_display_coordinates(DisplayCoordinates* display_coordinates) {
	return sqrt(display_coordinates->x * display_coordinates->x + display_coordinates->y * display_coordinates->y);
}

AsteroidContainer initialize_asteroid_container(Asteroid* asteroid) {
	AsteroidContainer asteroid_container = {
		.is_in_use = true,
		.asteroid = *asteroid,
	};
	return asteroid_container;
}

void fill_asteroid_container(AsteroidContainer* asteroid_container, Asteroid* asteroid) {
	if (asteroid_container->is_in_use) return;
	asteroid_container->asteroid = *asteroid;
}

void draw_circle_in_display_coordinates(const DisplayCoordinates* center, float radius, ALLEGRO_COLOR color, float thickness) {
	al_draw_circle(center->x, center->y, radius, color, thickness);
}

BlastContainer initialize_blast(WorldCoordinates* world_coordinates, float heading) {
	BlastContainer blast_container = {
		.is_in_use = true,
		.blast = {
			.world_coordinates = *world_coordinates,
			.heading = heading,
			.color = al_color_name(BLAST_COLOR),
			.vertices = BLAST_VERTICES,
			.hitbox = {
				.center = *world_coordinates,
				.radius = 1.0f * BLAST_SCALE_FACTOR,
			},
		},
	};
	return blast_container;
}

void draw_blast(BlastContainer* blast_container) {
	DisplayCoordinates display_coordinates = map_world_coordinates_to_display_coordinates(&blast_container->blast.world_coordinates);
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, -blast_container->blast.heading); // need to use negative of heading, since y axis is inverted in display coordinates
	al_translate_transform(&transform, display_coordinates.x, display_coordinates.y);
	al_use_transform(&transform);

	for (int i = 0; i < NUMBER_OF_BLAST_VERTICES - 1; ++i) {
		draw_line_in_display_coordinates(&blast_container->blast.vertices[i], &blast_container->blast.vertices[i+1], blast_container->blast.color, BLAST_LINE_THICKNESS);
	}

	al_identity_transform(&transform);
	al_use_transform(&transform);
	DisplayCoordinates hitbox_coordinates = map_world_coordinates_to_display_coordinates(&blast_container->blast.hitbox.center);
	draw_circle_in_display_coordinates(&hitbox_coordinates, blast_container->blast.hitbox.radius, al_color_name("green"), 2.0f);
}

void draw_blasts(BlastContainer blast_containers[]) {
	for (int i = 0; i < BLAST_MAX; ++i) {
		if (blast_containers[i].is_in_use) {
			draw_blast(&blast_containers[i]);
		}
	}
}

void update_blast(BlastContainer* blast_container, float frame_delta) {
	blast_container->blast.world_coordinates.x += (float)cos(blast_container->blast.heading) * (1.0f + frame_delta) * BLAST_VELOCITY;
	blast_container->blast.world_coordinates.y += (float)sin(blast_container->blast.heading) * (1.0f + frame_delta) * BLAST_VELOCITY;

	blast_container->blast.hitbox.center.x = blast_container->blast.world_coordinates.x;
	blast_container->blast.hitbox.center.y = blast_container->blast.world_coordinates.y;

	if (blast_container->blast.world_coordinates.x > DISPLAY_OFFSET_X || blast_container->blast.world_coordinates.x < -DISPLAY_OFFSET_X) {
		blast_container->is_in_use = false;
	}
	if (blast_container->blast.world_coordinates.y > DISPLAY_OFFSET_Y || blast_container->blast.world_coordinates.y < -DISPLAY_OFFSET_Y) {
		blast_container->is_in_use = false;
	}
}

void update_blasts(BlastContainer blast_containers[], float frame_delta) {
	for (int i = 0; i < BLAST_MAX; ++i) {
		if (blast_containers[i].is_in_use) {
			update_blast(&blast_containers[i], frame_delta);
		}
	}
}

void fire_blast(GameState* game_state) {
	if (is_pressed_or_needs_processing(game_state->key_states, FIRE)) {
		BlastContainer blast_container = initialize_blast(&game_state->spaceship.world_coordinates, game_state->spaceship.heading);
		for (int i = 0; i < BLAST_MAX; ++i) {
			if (!game_state->blast_containers[i].is_in_use && !game_state->is_blast_on_cooldown) {
				game_state->blast_containers[i] = blast_container;
				game_state->is_blast_on_cooldown = true;
			}
		}
	}
}

BLAST_TIMER* create_blast_timer() {
	BLAST_TIMER* blast_timer = malloc(sizeof(BLAST_TIMER));

	if (blast_timer) {
		al_init_user_event_source(&blast_timer->event_source);
	}

	return blast_timer;
}

void destroy_blast_timer(BLAST_TIMER* blast_timer) {
	al_destroy_user_event_source(&blast_timer->event_source);
	free(blast_timer);
}

void check_collision_between_blasts_and_asteroids(GameState* game_state) {
	for (int i = 0; i < BLAST_MAX; ++i) {
		if (game_state->blast_containers[i].is_in_use) {
			for (int j = 0; j < ASTEROID_MAX; ++j) {
				if (game_state->asteroid_containers[j].is_in_use) {
					bool is_collision = circles_collide_world_coordinates(&game_state->blast_containers[i].blast.hitbox, &game_state->asteroid_containers[j].asteroid.hitbox);
					if (is_collision) {
						//printf("Collision happened!\n");
						game_state->blast_containers[i].is_in_use = false;
						game_state->asteroid_containers[j].is_in_use = false;
					}
				}
			}
		}
	}
}

void create_new_asteroids(GameState* game_state) {
	if (!game_state->is_asteroid_creation_on_cooldown) {
		for (int i = 0; i < ASTEROID_MAX; ++i) {
			if (!game_state->asteroid_containers[i].is_in_use) {
				Asteroid asteroid = initialize_asteroid();
				game_state->asteroid_containers[i] = initialize_asteroid_container(&asteroid);
				game_state->is_asteroid_creation_on_cooldown = true;
			}
		}
	}
}
