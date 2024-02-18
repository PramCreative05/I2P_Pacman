#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_over.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"


//v TODO-HACKATHON 2-0: Create one ghost
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
#define LIFE 3
//v TODO-GC-ghost: create a least FOUR ghost!
/* global variables*/
extern int temp_score;
extern int64_t tick_elapsed;

extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
int lifes = LIFE;
bool game_over = false;

int64_t startStamp = 0;
int64_t endStamp = 0;
int64_t latestStamp = 0;
int64_t contStamp = 0;
int64_t deathStamp = 0;


/* Internal variables*/
static ALLEGRO_SAMPLE_ID gameBGM;

static ALLEGRO_TIMER* power_up_timer;
static const int power_up_duration = 10;

static ALLEGRO_BITMAP* stopSign = NULL;
static ALLEGRO_BITMAP* breakSign = NULL;
static ALLEGRO_BITMAP* heartSign = NULL;
static int stopW ;
static int stopH ;
static int breakW;
static int breakH;
static int heartW;
static int heartH;

static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;

bool debug_mode = false;
bool cheat_mode = false;
bool ctrl_state = false;
bool stop = false;
bool trans = false;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(int btn, int x, int y, int dz);
static void render_init_screen(void);
static void draw_hitboxes(void);

static void init(void) {
	game_over = false;
	lifes = LIFE;
	game_main_Score = 0;

	// create map
	// basic_map = create_map(NULL);
	//v TODO-GC-read_txt: Create map from .txt file so that you can design your own map!!
	basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}

	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacman\n");
	}

	stopSign = load_bitmap("Assets/stop.png");
	stopW = al_get_bitmap_width(stopSign);
	stopH = al_get_bitmap_height(stopSign);

	breakSign =  load_bitmap("Assets/breakW.png");
	breakW = al_get_bitmap_width(breakSign);
	breakH = al_get_bitmap_height(breakSign);

	heartSign = load_bitmap("Assets/heart.png");
	heartW = al_get_bitmap_width(heartSign);
	heartH = al_get_bitmap_height(heartSign);
	
	
	// allocate ghost memory
	//v TODO-HACKATHON 2-1: Allocate dynamic memory for ghosts array.
	
	ghosts = (Ghost**) malloc(sizeof(Ghost*) * GHOST_NUM); //Address Holder for All Ghosts
	
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	}
	else {
		//v TODO-HACKATHON 2-2: create a ghost.
		// Try to look the definition of ghost_create in ghost.h and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  //0 1 2 3 B P I C
			if (!ghosts[i])
				game_abort("error creating ghost\n");
			
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	stop_bgm(gameBGM);
	gameBGM = play_bgm(themeMusic, music_volume);
	//Start Timestamp
	startStamp = al_get_timer_count(game_tick_timer);
	power_up_timer = al_create_timer(1.0f); // 1 tick per second
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
// Use in scene update
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x;
	int Grid_y = pman->objData.Coord.y;

	// Outbound Handler
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	
	//v TODO-HACKATHON 1-3: check which item you are going to eat and use `pacman_eatItem` to deal with it.
	
	switch (basic_map->map[Grid_y][Grid_x]) {
		case '.':
			pacman_eatItem(pman, basic_map->map[Grid_y][Grid_x]);
			game_main_Score += 1;
			basic_map->map[Grid_y][Grid_x] = ' ';
			break;
		case 'P':
			//v TODO-GC-PB: ease power bean
			
			// stop and reset power_up_timer value
			if(pman->powerUp) {
				al_stop_timer(power_up_timer);
				al_set_timer_count(power_up_timer, 0);
			}

			pacman_eatItem(pman, basic_map->map[Grid_y][Grid_x]);

			// start the timer
			al_start_timer(power_up_timer);
			basic_map->map[Grid_y][Grid_x] = ' ';
			break;
		case 'B':
			if (!pman->trans) {
				al_rest(1.0);
				pacman_die();
				game_over = true;
				lifes--;
			}
			break;
		default:
			break;
	}
	
	

	//v TODO-HACKATHON 1-4: erase the item you eat from map
	// Be careful, don't erase the wall block.
	// '#' -> wall
	// '.' -> beans
	// 'B' -> room of ghost
	// 'P' -> Power Bean 
	
	
}
static void status_update(void) {
	//v TODO-GC-PB: check powerUp duration
	
	if (pman->powerUp)
	{
		// Check the value of power_up_timer
		// If runs out of time reset all relevant variables and ghost's status
		// hint: ghost_toggle_FLEE
		
		if(al_get_timer_count(power_up_timer) < power_up_duration) {
			for(int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], true);
			}
		}
		else {
			for(int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], false);
			}
			pman->powerUp = false;
			al_stop_timer(power_up_timer);
			al_set_timer_count(power_up_timer, 0);
		}
	}
	


	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN){
			continue;
		}
		else if (ghosts[i]->status == FREEDOM)
		{
			//v TODO-GC-game_over: use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect if pacman and ghosts collide with each other.
			// And perform corresponding operations.

			// [NOTE] You should have some if-else branch here if you want to implement power bean mode.
			// Uncomment Following Code
			
			RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
			RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);

			if(!cheat_mode && RecAreaOverlap(&pmanHB, &ghostHB))
			{
				game_log("collide with ghost\n");
				al_rest(1.0);
				pacman_die();
				lifes--;
				game_over = true;
				break; // animation shouldn't be trigger twice.
			}
		}
		else if (ghosts[i]->status == FLEE)
		{
			//v TODO-GC-PB: if ghost is collided by pacman, it should go back to the cage immediately and come out after a period.
			RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
			RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);

			if(!cheat_mode && RecAreaOverlap(&pmanHB, &ghostHB))
			{
				ghost_collided(ghosts[i]);
				game_main_Score += 100;
				
			}
		}
	}
}

static void update(void) {

	if (game_over && lifes < LIFE && lifes > 0) {
		// Reset
		if (al_get_timer_count(pman->death_anim_counter) == 0) {
			deathStamp = al_get_timer_count(game_tick_timer);
			//game_log("Death: %lld", deathStamp);
		}
		
		al_start_timer(pman->death_anim_counter);
		// Reset Ghost
        for(int i = 0; i < GHOST_NUM; i++) ghost_reset(ghosts[i]);
		if(al_get_timer_count(pman->death_anim_counter) > 12) {
			al_stop_timer(pman->death_anim_counter);
			al_set_timer_count(pman->death_anim_counter, 0);
			
			// Reset Pacman
			pacman_reset(pman);

			contStamp = al_get_timer_count(game_tick_timer);
			game_log("Cont: %lld", contStamp);

			game_over = false;
		}
	}
	
	if (lifes == 0) {
		//v TODO-GC-game_over: start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
		// hint: refer al_get_timer_started(...), al_get_timer_count(...), al_stop_timer(...), al_rest(...)
		
			// start the timer if it hasn't been started.
			// check timer counter.
			// stop the timer if counter reach desired time.
		game_over = true;
		if (al_get_timer_count(pman->death_anim_counter) == 0) endStamp = al_get_timer_count(game_tick_timer);
		al_start_timer(pman->death_anim_counter);
		if(al_get_timer_count(pman->death_anim_counter) > 12) {
			al_stop_timer(pman->death_anim_counter);
			temp_score = game_main_Score;
			//ticking every 32 frames; tick = count * (1.0f / 64/ 2)
			//tick_elapsed = endStamp - startStamp - ( (contStamp - deathStamp) * (LIFE - lifes)) - ((LIFE - lifes) * (1.0 /0.0078125));

			game_log("Time: %lld ~ %.2lf ", tick_elapsed, ((tick_elapsed) * (0.0078125)));
			stop_bgm(gameBGM);
			game_change_scene(scene_over_create()); // Game Over
		}
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	//v TODO-GC-scoring: Draw scoreboard, something your may need is sprinf();
	if (game_over) tick_elapsed = latestStamp;
	char scoreNow[20];
	sprintf(scoreNow, "Score: %d", game_main_Score);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		100, SCREEN_H - 50,
		ALLEGRO_ALIGN_CENTER,
		scoreNow
	);

	//v TODO-Adv: Ghost Stop

	if (stop) {
		al_draw_scaled_bitmap(
			stopSign,
			0, 0,
			stopW, stopH,
			200, 740,
			stopW * 0.02, stopH * 0.02,
			0
		);
	}

	if (trans) {
		al_draw_scaled_bitmap(
			breakSign,
			0, 0,
			breakW, breakH,
			280, 740,
			breakW * 0.08, breakH * 0.08,
			0
		);
	}

	// Render Heatlh
	int space = 10;
	for (int i = 0; i < lifes; i++) {
		al_draw_scaled_bitmap(
			heartSign,
			0, 0,
			300, heartH,
			30 + ( (300 * 0.15) * i + space), SCREEN_H - 110,
			300 * 0.15, heartH * 0.15,
			0
		);
	}

	// Cheat Notif
	if (cheat_mode) {
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 0),
			200, SCREEN_H - 100,
			ALLEGRO_ALIGN_LEFT,
			"CHEAT: ON!"
		);
	}

	// Render Time
	if (lifes < LIFE) {
		int64_t halt = contStamp - deathStamp;
		latestStamp = al_get_timer_count(game_tick_timer) - (startStamp) - (halt * (LIFE - lifes)) - ((LIFE - lifes) * (1.0 /0.0078125));
	}
	else {
		latestStamp = al_get_timer_count(game_tick_timer) - (startStamp);
	}
		
	if (!game_over) {
		char time[50] = "";
		//Each ghost collide rest 1.0 and rest = 2
		sprintf(time, "Time: %.2lf", (latestStamp * 0.0078125) + 2);
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 0),
			SCREEN_W - 180, SCREEN_H - 50,
			ALLEGRO_ALIGN_LEFT,
			time
		);
	}
	// Render Main Object
	draw_map(basic_map);
	pacman_draw(pman);

    if (!game_over) for (int i = 0; i < GHOST_NUM; i++) ghost_draw(ghosts[i]);

	if (game_over && !lifes) {
		game_log("Game Over");
		return;
	}
	
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}
}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	//v TODO-GC-memory: free map array, Pacman and ghosts
	for(int i = 0; i < GHOST_NUM; i++) ghost_destroy(ghosts[i]);
	free(ghosts);
	pacman_destroy(pman);
	delete_map(basic_map);
	al_destroy_bitmap(stopSign);
	al_destroy_bitmap(breakSign);
	al_destroy_bitmap(heartSign);
	game_log("main game destroyed");
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
		//v TODO-HACKATHON 1-1: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
		
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			// TODO-Adv: Cross Wall
			if (ctrl_state && cheat_mode) {
				stop = (!stop) ? true : false;
				for(int i = 0; i < GHOST_NUM; i++) ghosts[i]->stop = stop;
				game_log("Stop Ghost");
				ctrl_state = false;
			}
			else {
				pacman_NextMove(pman, DOWN);
			}
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode) {
				printf("cheat mode on\n");
			}
			else {
				for(int i = 0; i < GHOST_NUM; i++) ghosts[i]->stop = false;
				stop = false;
				pman->trans = false;
				trans = false;
				printf("cheat mode off\n");
			}	
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;
		case ALLEGRO_KEY_K:
			//v TODO-Adv: Go back to cage
			if (cheat_mode) {
				for (int i = 0; i < GHOST_NUM; i++) {
					ghost_home(ghosts[i]);
				}
			}
			break;
		case ALLEGRO_KEY_LCTRL:
			ctrl_state = (!ctrl_state) ? true : false;
			break;
		case ALLEGRO_KEY_L:
			// TODO-Adv: Cross Wall
			if (ctrl_state && cheat_mode) {
				trans = (!trans) ? true : false;
				pman->trans = trans;

				game_log("Invisible");
				ctrl_state = !ctrl_state;
			}
			break;
		default:
			break;
	}

}

static void on_mouse_down(int btn, int x, int y, int dz) {

	// nothing here

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);
}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

int64_t get_power_up_duration() {
    return (int64_t)power_up_duration;
}

int64_t get_power_up_timer_tick() {
	return (int64_t)al_get_timer_count(power_up_timer);
}

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
