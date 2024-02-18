//v TODO-HACKATHON 3-9: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "scene_menu_object.h"
#include "scene_settings.h"
#include "utility.h"

extern float music_volume;

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */
static Button btnVol;
static int knobX = 150;

// TODO-IF: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.

// Slider properties
#define SLIDER_WIDTH 300
#define SLIDER_HEIGHT 20
#define SLIDER_COLOR al_map_rgb(250, 250, 250)

static void init() {
	btnVol = button_create(300, 200, SLIDER_WIDTH, SLIDER_HEIGHT, "Assets/bar.png", "Assets/bar.png");
}

static void draw(void ){
	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(
		menuFont,
		al_map_rgb(204, 0, 153),
		SCREEN_W / 2,
		100,
		ALLEGRO_ALIGN_CENTER,
		"SETTING"
	);


	al_draw_text(
		menuFont,
		al_map_rgb(204, 0, 153),
		50,
		200,
		ALLEGRO_ALIGN_LEFT,
		"Effect Volume: "
	);
	
	drawButton(btnVol);
	// Draw slider indicator
    al_draw_filled_rectangle(300, 200, 300 + knobX, 200 + SLIDER_HEIGHT, SLIDER_COLOR);
	
	char eVol[10];
	sprintf(eVol, "%.0f%%", effect_volume * 100);

	al_draw_text(
		menuFont,
		al_map_rgb(204, 0, 153),
		350 + SLIDER_WIDTH,
		200,
		ALLEGRO_ALIGN_LEFT,
		eVol
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);

}

static void on_mouse_move() {
	btnVol.hovered = pnt_in_rect(mouse_x, mouse_y, btnVol.body);
}

static void on_key_down(int keycode) {
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_menu_create());
			break;
		default:
			break;
	}
}

static void on_mouse_down() {
	// Get the axis
	if (btnVol.hovered) {
		// 300 - 600
		game_log("%d", mouse_x);
		knobX = mouse_x - 300;

		effect_volume = (float) knobX / 300.0;
		game_log("Updated eVol: %.2f", effect_volume);
	}
	// Update Volume
}

static void destroy() {
	al_destroy_bitmap(btnVol.default_img);
	al_destroy_bitmap(btnVol.hovered_img);
	game_log("Setting Destroyed");
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	scene.destroy = &destroy;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}

// TODO-Graphical_Widget:
// Just suggestions, you can create your own usage.
	// Selecting BGM:
	// 1. Declare global variables for storing the BGM. (see `shared.h`, `shared.c`)
	// 2. Load the BGM in `shared.c`.
	// 3. Create dropdown menu for selecting BGM in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and play the BGM if the corresponding option is selected.

	// Adjusting Volume:
	// 1. Declare global variables for storing the volume. (see `shared.h`, `shared.c`)
	// 2. Create a slider for adjusting volume in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. You may use checkbox to switch between mute and unmute.
	// 3. Adjust the volume and play when the button is pressed.

	// Selecting Map:
	// 1. Preload the map to `shared.c`.
	// 2. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. For player experience, you may also create another scene between menu scene and game scene for selecting map.
	// 3. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and draw the map if the corresponding option is selected.
		// 4.1. Suggestions: You may use `al_draw_bitmap` to draw the map for previewing. 
							// But the map is too large to be drawn in original size. 
							// You might want to modify the function to allow scaling.