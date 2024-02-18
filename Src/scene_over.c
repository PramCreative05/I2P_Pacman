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
#include "scene_over.h"
#include "utility.h"
#include "shared.h"

#define MAX_ROWS 50
#define MAX_COLS 4
#define MAX_NAME_LENGTH 50

extern int temp_score;
extern int64_t tick_elapsed;

/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;
static int gameTitleW;
static int gameTitleH;

static ALLEGRO_BITMAP* podium = NULL;
static int podiumH;
static int podiumW;

static ALLEGRO_SAMPLE_ID menuBGM;

const int maxNameLen = 10;
char playerName[11] = "Input";
bool entry = false;

char finalScore[50] = "";
char stringData[50] = "";

char data[MAX_ROWS][MAX_COLS][MAX_NAME_LENGTH];
int totalRows;


FILE* file = NULL;

// [HACKATHON 3]
// TARGET : use a clickable button to enter setting scene.
// For `Button` struct(object) is defined in `scene_menu_object.h` and `scene_menu_object.c`
// STRONGLY recommend you trace both of them first. 

//v TODO-HACKATHON 3-1: Declare variable for button
// Uncomment and fill the code below
static Button btnSettings;

static void init() {

	//v TODO-HACKATHON 3-2: Create button to settings
	// Uncomment and fill the code below
	btnSettings = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png");

	gameTitle = load_bitmap("Assets/over.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);

	podium = load_bitmap("Assets/podium.png");
	podiumW = al_get_bitmap_width(podium);
	podiumH = al_get_bitmap_height(podium);

	stop_bgm(menuBGM);
	menuBGM = play_bgm(overMusic, music_volume);
}


static void draw() {

	al_clear_to_color(al_map_rgb(43, 8, 12));

	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;

	const float offset_wp = (SCREEN_W >> 1) - 0.5 * 1 * podiumW;

	//draw title image
	al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h - 100,
		gameTitleW * scale, gameTitleH * scale,
		0
	);

	if (!entry) {
		// Get the Score
		
		sprintf(finalScore, "Score: %d Time: %.2lf", temp_score, (tick_elapsed * 0.0078125) + 2);
		al_draw_text(
			menuFont, 
			al_map_rgb(250, 250, 250), 
			SCREEN_W / 2, 
			SCREEN_H / 2 + 135, 
			ALLEGRO_ALIGN_CENTER, 
			finalScore
		);

		// Draw the textbox
		al_draw_filled_rectangle(
			SCREEN_W / 4, 
			SCREEN_H / 2 + 170, 
			SCREEN_W * 3 / 4, 
			SCREEN_H / 2 + 220, 
			al_map_rgb(200, 200, 200)
		);

		// Draw the player name
		al_draw_text(
			menuFont, 
			al_map_rgb(250, 0, 0), 
			SCREEN_W / 2, 
			SCREEN_H / 2 + 180, 
			ALLEGRO_ALIGN_CENTER, 
			playerName
		);

		// Draw the player name
		al_draw_text(
			warnFont, 
			al_map_rgb(250, 250, 0), 
			SCREEN_W / 2, 
			SCREEN_H / 2 + 230, 
			ALLEGRO_ALIGN_CENTER, 
			"Please input max 10: a ~ z and 0 ~ 9"
		);


		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			SCREEN_W/2,
			SCREEN_H - 90,
			ALLEGRO_ALIGN_CENTER,
			"PRESS <ENTER> TO SAVE"
		);
	}
	else {
		al_draw_scaled_bitmap(
			podium,
			0, 0,
			podiumW, podiumH,
			offset_wp, (SCREEN_H / 2) + 200,
			podiumW * 1, podiumH * 1,
			0
		);

		//Debugger
		// for(int i = totalRows - 1; i > totalRows - 4; i--){
		// 	game_log("%s", data[i][1]);
		// }

		// Draw Highest Score
		char pos[30];
		char pos1[30];
		char pos2[30];
		char score[10];
		char score1[10];
		char score2[10];
		char name[15];
		char name1[15];
		char name2[15];
		char space[3] = ": ";

		strcpy(name, data[totalRows - (1 + 0)][1]);
		strcpy(score, data[totalRows - (1 + 0)][2]);

		strcpy(pos, name);
		strcat(pos, space);
		strcat(pos, score);
		game_log("%s", pos);
		
		al_draw_text(
			menuFont, 
			al_map_rgb(250, 250, 250), 
			SCREEN_W / 2, 
			SCREEN_H / 2 + 165, 
			ALLEGRO_ALIGN_CENTER, 
			pos
		);

		strcpy(name1, data[totalRows - (1 + 1)][1]);
		strcpy(score1, data[totalRows - (1 + 1)][2]);

		strcpy(pos1, name1);
		strcat(pos1, space);
		strcat(pos1, score1);
		game_log("%s", pos1);
		
		al_draw_text(
			menuFont, 
			al_map_rgb(250, 250, 250), 
			SCREEN_W / 2 - 130, 
			SCREEN_H / 2 + 210, 
			ALLEGRO_ALIGN_CENTER, 
			pos1
		);

		strcpy(name2, data[totalRows - (1 + 2)][1]);
		strcpy(score2, data[totalRows - (1 + 2)][2]);

		strcpy(pos2, name2);
		strcat(pos2, space);
		strcat(pos2, score2);
		game_log("%s", pos2);
		
		al_draw_text(
			menuFont, 
			al_map_rgb(250, 250, 250), 
			SCREEN_W / 2 + 130, 
			SCREEN_H / 2 + 215, 
			ALLEGRO_ALIGN_CENTER, 
			pos2
		);

		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			SCREEN_W/2,
			SCREEN_H - 70,
			ALLEGRO_ALIGN_CENTER,
			"PRESS <ESC> TO MENU"
		);
	}
	
	//drawButton(btnSettings);
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//v	TODO-HACKATHON 3-7: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
	btnSettings.hovered = pnt_in_rect(mouse_x, mouse_y, btnSettings.body);
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
		
}


static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(gameTitle);
	al_destroy_bitmap(podium);
	
	al_destroy_bitmap(btnSettings.default_img);
	al_destroy_bitmap(btnSettings.hovered_img);

	game_log("Over Destroyed");
	
}

static void save_score(const char* stringData) {
	file = fopen("scoreBoard.csv", "a");
	if (file == NULL) 
		game_abort("Error open data");
	else {
		fprintf(file, "%s\n", stringData);
		fclose(file);
		game_log("Data Saved");
	}
}

static void update_header() {
	file = fopen("scoreBoard.csv", "r+");
	if (file == NULL) 
		game_abort("Error open data in Update Header");
	else {
		char line[50];
		if (fgets(line, sizeof(line), file) != NULL) {
			int ctr = 0;
			// sscanf return the object parsed
			if (sscanf(line, "Entries [Name,Score,Time]: %d", &ctr) == 1) {
				ctr++;
				//Set the pointer to the beginning to rewrite
				rewind(file);
				fprintf(file, "Entries [Name,Score,Time]: %d", ctr);
			}
			else {
				game_abort("Failed to parsed header");
			}

		}
		fclose(file);
		game_log("Header Updated");
	}
}



static void on_key_down(int keycode) {

	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			//game_change_scene(scene_main_create());

			if (!entry) {
				entry = true;
				
				char score[10] = "";
				snprintf(score, sizeof(score), "%d", temp_score);

				char time[10] = "";
				snprintf(time, sizeof(time), "%.2lf", (tick_elapsed * 0.0078125) + 2);

				snprintf(stringData, sizeof(stringData), "%s %s %s", playerName, score, time);
				game_log("Saving: %s", stringData);

				//Save Data, Alter the counter
				save_score(stringData);
				update_header();

				//Fetch Rank inject to local
				parseCSV("scoreBoard.csv", data, &totalRows);
				
				//Debugger
				for(int i = totalRows - 1; i > totalRows - 4; i--){
					game_log("%s", data[i][1]);
				}

				game_log("Total Played: %d", totalRows);
			}
			else {
				// Alternative Handler
				entry = false;
				strcpy(playerName, "Input");
				game_change_scene(scene_menu_create());
			}
			
			

			break;
		case ALLEGRO_KEY_BACKSPACE:
            // Remove the last character from the player name
            if (strlen(playerName) > 0) {
                playerName[strlen(playerName) - 1] = '\0';
            }
            break;
		case ALLEGRO_KEY_ESCAPE:
			if (entry) {
				entry = false;
				strcpy(playerName, "Input");
				game_change_scene(scene_menu_create());
			}
			break;
		default:
			// a = 0; 9 = 36
			if (keycode >= 0 && keycode <= 36 && strlen(playerName) < maxNameLen) {
                char character;

				if (keycode <= 26) character = 96 + (keycode);
				// 0 = 27; 9 = 36
				if (keycode > 26) character = 21 + (keycode);

                strncat(playerName, &character, 1);
            }
			break;
	}
}



// TODO-IF: Add more event callback functions such as update, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_over_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Over";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	//v TODO-HACKATHON 3-9: Register on_mouse_down.
	// Uncomment the code below.

	scene.on_mouse_down = &on_mouse_down;
	

	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Over scene created");
	return scene;
}