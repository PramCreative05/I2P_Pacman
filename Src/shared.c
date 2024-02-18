// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
// #include "scene_menu.h"

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_SAMPLE* themeMusic = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND = NULL;
ALLEGRO_SAMPLE* GHOST_DEATH_SOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_BEAN = NULL;
ALLEGRO_SAMPLE* overMusic = NULL;
ALLEGRO_FONT* menuFont = NULL;
ALLEGRO_FONT* warnFont = NULL;
ALLEGRO_FONT* openFont = NULL;
int fontSize = 30;
int warnSize = 15;
float music_volume = 0.5;
float effect_volume = 0.5;
bool gameDone = false;

/*
	A way to accelerate is load assets once.
	And delete them at the end.
	This method does provide you better management of memory.
	
*/
void shared_init(void) {
	openFont = load_font("Assets/OpenSans-Regular.ttf", fontSize);
	menuFont = load_font("Assets/Minecraft.ttf", fontSize);
	warnFont = load_font("Assets/Minecraft.ttf", warnSize);
	themeMusic = load_audio("Assets/Music/original_theme.ogg");
	PACMAN_MOVESOUND = load_audio("Assets/Music/pacman-chomp.ogg");
	PACMAN_DEATH_SOUND = load_audio("Assets/Music/pacman_death.ogg");
	GHOST_DEATH_SOUND = load_audio("Assets/Music/pacman_eatghost.ogg");
	PACMAN_BEAN =  load_audio("Assets/Music/pacman_intermission.ogg");
	overMusic = load_audio("Assets/Music/fail.ogg");
}

void shared_destroy(void) {
	al_destroy_font(openFont);
	al_destroy_font(menuFont);
	al_destroy_font(warnFont);
	al_destroy_sample(themeMusic);
	al_destroy_sample(PACMAN_MOVESOUND);
	al_destroy_sample(PACMAN_DEATH_SOUND);
	al_destroy_sample(GHOST_DEATH_SOUND);
	al_destroy_sample(PACMAN_BEAN);
	al_destroy_sample(overMusic);
}