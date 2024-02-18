#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

#define MAX_ROWS 50
#define MAX_COLS 4
#define MAX_NAME_LENGTH 50



/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;

static ALLEGRO_SAMPLE_ID menuBGM;
static int gameTitleW ;
static int gameTitleH ;

static char data[MAX_ROWS][MAX_COLS][MAX_NAME_LENGTH];
static int totRow;

bool viewScore = false;
bool defaultMode = true;

// [HACKATHON 3]
// TARGET : use a clickable button to enter setting scene.
// For `Button` struct(object) is defined in `scene_menu_object.h` and `scene_menu_object.c`
// STRONGLY recommend you trace both of them first. 

//v TODO-HACKATHON 3-1: Declare variable for button
// Uncomment and fill the code below
static Button btnSettings;
static Button btnLeader;

static Button btnJohn;
static Button btnPacman;

static void init() {

	//v TODO-HACKATHON 3-2: Create button to settings
	// Uncomment and fill the code below
	btnSettings = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png");
	btnLeader = button_create(670, 20, 50, 50, "Assets/rankRaw.png", "Assets/rankRaw(1).png");
	btnJohn = button_create((SCREEN_W / 2) - 100, SCREEN_H /2, 90, 90, "Assets/btnJohn(1).png", "Assets/btnJohn.png");
	btnPacman = button_create((SCREEN_W / 2) + 10, SCREEN_H /2, 90, 90, "Assets/p_c.png", "Assets/p_c(1).png");

	gameTitle = load_bitmap("Assets/title.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);
	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);

	parseCSV("scoreBoard.csv", data, &totRow);
}


static void draw() {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	const float scale = 0.6;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;

	//draw title image
	al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h - 250,
		gameTitleW * scale, gameTitleH * scale,
		0
	);

	// Select Character
	al_draw_text(
		menuFont,
		al_map_rgb(250, 250, 250),
		SCREEN_W / 2,
		SCREEN_H / 2 - 50,
		ALLEGRO_ALIGN_CENTER,
		"SELECT MODE:"
	);

	drawButton(btnJohn);
	drawButton(btnPacman);

	char chosen[10] = "";
	(defaultMode) ? strcpy(chosen, "PACMAN") : strcpy(chosen, "JOHN");

	al_draw_text(
		menuFont,
		al_map_rgb(250, 250, 0),
		SCREEN_W / 2,
		SCREEN_H / 2 + 120,
		ALLEGRO_ALIGN_CENTER,
		chosen
	);


	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS <ENTER>"
	);

	if (viewScore) {
		al_draw_filled_rectangle(
			offset_w, 
			SCREEN_H / 2 - 150, 
			gameTitleW * 0.777, 
			SCREEN_H / 2 + 220, 
			al_map_rgb(200, 200, 200)
		);

		// For Loop Render
		al_draw_text(
			menuFont,
			al_map_rgb(0, 0, 0),
			offset_w + 20,
			SCREEN_H / 2 - 130,
			ALLEGRO_ALIGN_LEFT,
			"NO"
		);

		al_draw_text(
			menuFont,
			al_map_rgb(0, 0, 0),
			offset_w + 80,
			SCREEN_H / 2 - 130,
			ALLEGRO_ALIGN_LEFT,
			"NAME"
		);

		al_draw_text(
			menuFont,
			al_map_rgb(0, 0, 0),
			offset_w + 270,
			SCREEN_H / 2 - 130,
			ALLEGRO_ALIGN_LEFT,
			"PTS"
		);

		al_draw_text(
			menuFont,
			al_map_rgb(0, 0, 0),
			offset_w + 360,
			SCREEN_H / 2 - 130,
			ALLEGRO_ALIGN_LEFT,
			"TIME"
		);

		for(int i = totRow - 1, n = 1; i > totRow - 6; i--, n++) {
			char name[11] = "";
			char rank[2] = "";
			char pts[5] = "";
			char time[6] = "";

			sprintf(rank, "%d", n);
			strcpy(name, data[i][1]);
			strcpy(pts, data[i][2]);
			strncpy(time, data[i][3], 5);
			int space = 55;
			
			al_draw_text(
				menuFont,
				al_map_rgb(0, 0, 0),
				offset_w + 25,
				(SCREEN_H / 2 - 130) + (space * n),
				ALLEGRO_ALIGN_LEFT,
				rank
			);

			al_draw_text(
				menuFont,
				al_map_rgb(0, 0, 0),
				offset_w + 80,
				(SCREEN_H / 2 - 130) + (space * n),
				ALLEGRO_ALIGN_LEFT,
				name
			);

			al_draw_text(
				menuFont,
				al_map_rgb(0, 0, 0),
				offset_w + 270,
				(SCREEN_H / 2 - 130) + (space * n),
				ALLEGRO_ALIGN_LEFT,
				pts
			);

			al_draw_text(
				menuFont,
				al_map_rgb(0, 0, 0),
				offset_w + 360,
				(SCREEN_H / 2 - 130) + (space * n),
				ALLEGRO_ALIGN_LEFT,
				time
			);
		}
		
	}

	//v TODO-HACKATHON 3-3: Draw button
	// Uncomment and fill the code below
	drawButton(btnSettings);
	drawButton(btnLeader);


}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//v	TODO-HACKATHON 3-7: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
	btnSettings.hovered = pnt_in_rect(mouse_x, mouse_y, btnSettings.body);
	btnLeader.hovered = pnt_in_rect(mouse_x, mouse_y, btnLeader.body);
	btnJohn.hovered = pnt_in_rect(mouse_x, mouse_y, btnJohn.body);
	btnPacman.hovered = pnt_in_rect(mouse_x, mouse_y, btnPacman.body);
}


//v	TODO-HACKATHON 3-8: When btnSettings clicked, switch to settings scene
//  `game_change_scene` is defined in `game.h`.
//  You can check line 121 `scene_menu.c` to see how to use this function.
//  And find the corresponding function for `scene_setting`
// 	Utilize the member defined in struct `Button`.
//  The logic here is 
//  `if clicked the mouse` && `mouse position is in the area of button`
// 	      `Enter the setting scene`
//	Uncomment and fill the code below

static void on_mouse_down() {
	if (btnSettings.hovered){
		game_change_scene(scene_settings_create());
		printf("clicked\n");
	}

	if (btnLeader.hovered){
		viewScore = (!viewScore) ? true : false;
		game_log("Pop Up");
	}

	if (btnJohn.hovered){
		//viewScore = (!viewScore) ? true : false;
		defaultMode = false;
		game_log("John Clicked");
	}

	if (btnPacman.hovered){
		//viewScore = (!viewScore) ? true : false;
		defaultMode = true;
		game_log("Pac Clicked");
	}
		
}


static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(gameTitle);
	//v	TODO-HACKATHON 3-10: Destroy button images
	//	Uncomment and fill the code below
	
	al_destroy_bitmap(btnSettings.default_img);
	al_destroy_bitmap(btnSettings.hovered_img);

	al_destroy_bitmap(btnLeader.default_img);
	al_destroy_bitmap(btnLeader.hovered_img);

	al_destroy_bitmap(btnJohn.default_img);
	al_destroy_bitmap(btnJohn.hovered_img);

	al_destroy_bitmap(btnPacman.default_img);
	al_destroy_bitmap(btnPacman.hovered_img);

	game_log("Menu Destroyed");
	
}

static void on_key_down(int keycode) {

	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_main_create());
			break;
		default:
			break;
	}
}

// TODO-IF: Add more event callback functions such as update, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_menu_create(void) {

	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Menu";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	//v TODO-HACKATHON 3-9: Register on_mouse_down.
	// Uncomment the code below.

	scene.on_mouse_down = &on_mouse_down;
	

	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Menu scene created");
	return scene;
}