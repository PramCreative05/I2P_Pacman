#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include "pacman_obj.h"
#include "map.h"
#include "scene_menu.h"
/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;		// where to put pacman at the beginning
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3;  // draw offset 
static const int draw_region = 30;							// pacman bitmap draw region
static ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
// [ NOTE - speed ]
// If you want to implement something regarding speed.
// You may have to modify the basic_speed here.
// But before you modify this, make sure you are 
// totally understand the meaning of speed and function
// `step()` in `scene_game.c`, also the relationship between
// `speed`, `GAME_TICK`, `GAME_TICK_CD`, `objData->moveCD`.
static const int basic_speed = 2;

/* Shared variables */
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern ALLEGRO_SAMPLE* PACMAN_BEAN;
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern bool game_over;
extern float effect_volume;
extern bool defaultMode;

/* Declare static function */
static bool pacman_movable(const Pacman* pacman, const Map* M, Directions targetDirec) {
	//v TODO-HACKATHON 1-2: Determine if the current direction is movable.
	// That is to say, your pacman shouldn't penetrate 'wall' and 'room'
	// , where room is reserved for ghost to set up.
	// 1) For the current direction `targetDirec`, use pre-implemented function
	// `is_wall_block` and `is_room_block` to check if the block is wall or room. (they are both defined in map.c)
	// 2) the coordinate data of pacman is stored in pacman->objData.Coord
	// it is a self-defined pair IntInt type. Trace the code and utilize it.

	int current_x = pacman->objData.Coord.x;
	int current_y = pacman->objData.Coord.y;

	int flag = 1;
	
	switch (targetDirec) {
		case UP:
			if (is_wall_block(M, current_x, current_y - 1) || is_room_block(M, current_x, current_y - 1)) flag = 0;
			if (pacman->trans) flag = 1;
			if (current_y - 1 >= M->row_num - 1 || current_y - 1 <= 0 || current_x >= M->col_num - 1 || current_x <= 0) flag = 0;
			break;
		case DOWN:
			if (is_wall_block(M, current_x, current_y + 1) || is_room_block(M, current_x, current_y + 1)) flag = 0;
			if (pacman->trans) flag = 1;
			if (current_y + 1 >= M->row_num - 1 || current_y + 1 <= 0 || current_x >= M->col_num - 1 || current_x <= 0) flag = 0;
			break;
		case LEFT:
			if (is_wall_block(M, current_x - 1, current_y) || is_room_block(M, current_x - 1, current_y)) flag = 0;
			if (pacman->trans) flag = 1;
			if (current_y >= M->row_num - 1 || current_y <= 0 || current_x - 1 >= M->col_num - 1 || current_x - 1 <= 0) flag = 0;
			break;
		case RIGHT:
			if (is_wall_block(M, current_x + 1, current_y) || is_room_block(M, current_x + 1, current_y)) flag = 0;
			if (pacman->trans) flag = 1;
			if (current_y >= M->row_num - 1 || current_y <= 0 || current_x + 1 >= M->col_num - 1 || current_x + 1 <= 0) flag = 0;
			break;
		default:
			// for none UP, DOWN, LEFT, RIGHT direction u should return false.
			return false;
	}

	return (flag) ? true : false;
}

Pacman* pacman_create() {
	// Allocate dynamic memory for pman pointer;
	Pacman* pman = (Pacman*)malloc(sizeof(Pacman));
	if (!pman)
		return NULL;
	pman->objData.Coord.x = 24;
	pman->objData.Coord.y = 24;
	pman->objData.Size.x = block_width;
	pman->objData.Size.y = block_height;

	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;
	pman->speed = basic_speed;

	pman->death_anim_counter = al_create_timer(1.0f / 8.0f);
	pman->powerUp = false;
	pman->trans = false;

	char chosen[10] = "";
	(defaultMode) ? strcpy(chosen, "PACMAN") : strcpy(chosen, "JOHN");
	game_log("CHOSEN: %s", chosen);

	/* load sprites */
	if (defaultMode) {
		pman->move_sprite = load_bitmap("Assets/pacman_move.png");
		pman->die_sprite = load_bitmap("Assets/pacman_die.png");
	}
	else {
		pman->move_sprite = load_bitmap("Assets/John.png");
		pman->die_sprite = load_bitmap("Assets/poof.png");
	}
	
	return pman;

}

void pacman_reset(Pacman* pman) {
	pman->objData.Coord.x = 24;
	pman->objData.Coord.y = 24;
	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;

}

void pacman_destroy(Pacman* pman) {	
	//v TODO-GC-memory: free pacman resource
	
	al_destroy_bitmap(pman->die_sprite);
	al_destroy_bitmap(pman->move_sprite);
	free(pman);
	
}


void pacman_draw(Pacman* pman) {
	//v TODO-GC-animation: Draw Pacman and animations
	// hint: use pman->objData.moveCD to determine which frame of the animation to draw
	RecArea drawArea = getDrawArea((object *)pman, GAME_TICK_CD);

	int offset = 0;
	int offdeath = 0;
	int64_t timerCount = 0;
	if (!game_over) {
		//v TODO-GC-animation: We have two frames for each direction. You can use the value of pman->objData.moveCD to determine which frame of the animation to draw.
		// For example, if the value if odd, draw 1st frame. Otherwise, draw 2nd frame.
		// But this frame rate may be a little bit too high. We can use % 32 and draw 1st frame if value is 0~15, and 2nd frame if value is 16~31.
		
		if((pman->objData.moveCD >> 4) & 1 == 0){
			offset = 0;
		}
		else if((pman->objData.moveCD >> 4) & 1 == 1){
			offset = (defaultMode) ? 16 : 460;
		}
		
		/*
		NOTE: since modulo operation is expensive in clock cycle perspective (reference: https://stackoverflow.com/questions/27977834/why-is-modulus-operator-slow)
			, you can use & (bitwise and) operator to determine a value is odd or even.
			e.g. If (val & 1 == 1) is true then `val` is odd. If (val & 1 == 0) is false then `val` is even.
			e.g. Similarly, if ((val>>4) & 1 == 0) is true then `val % 32` is 0~15, if ((val>>4) & 1 == 1) is true then `val % 32` is 16~31. 
		*/
		
		switch(pman->objData.facing) {
			case LEFT:
				if (defaultMode) {
					al_draw_scaled_bitmap(pman->move_sprite, 32 + offset, 0,
						16, 16,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				else {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 600 * 2,
						460, 600,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				break;
			case RIGHT:
				if (defaultMode) {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 0,
						16, 16,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				else {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 600 * 1,
						460, 600,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				break;
			case UP:
				if (defaultMode) {
					al_draw_scaled_bitmap(pman->move_sprite, 64 + offset, 0,
						16, 16,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				else {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 600 * 3,
						460, 600,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				break;
			case DOWN:
				if (defaultMode) {
					al_draw_scaled_bitmap(pman->move_sprite, 96 + offset, 0,
						16, 16,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				else {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 0,
						460, 600,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				break;
			default:
				if (defaultMode) {
					al_draw_scaled_bitmap(pman->move_sprite, 0 + offset, 0,
						16, 16,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				else {
					al_draw_scaled_bitmap(pman->move_sprite, 0, 600 * 1,
						460, 600,
						drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
						draw_region, draw_region, 0
					);
				}
				break;
		}
	}
	else {
		//v TODO-GC-animation: Draw die animation(pman->die_sprite)
		// hint: instead of using pman->objData.moveCD, use pman->death_anim_counter to create animation.
		// refer al_get_timer_count and al_draw_scaled_bitmap. Suggestion frame rate: 8fps.

		// timerCount = al_get_timer_count(pman->death_anim_counter);
		timerCount = al_get_timer_count(pman->death_anim_counter);
		game_log("timer: %ld", timerCount);

		int spacing;
		spacing = (defaultMode) ? 16 : 323;
		offdeath = timerCount * spacing;

		if (defaultMode) {
			al_draw_scaled_bitmap(pman->die_sprite, offdeath, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
		else {
			al_draw_scaled_bitmap(pman->die_sprite, offdeath, 0,
				323, 323,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
	}
}

void pacman_move(Pacman* pacman, Map* M) {
	if (!movetime(pacman->speed))
		return;
	if (game_over)
		return;

	int probe_x = pacman->objData.Coord.x, probe_y = pacman->objData.Coord.y;

	// TODO-Adv: Cheat Handler Cross Wall

	if (pacman_movable(pacman, M, pacman->objData.nextTryMove)) 
		pacman->objData.preMove = pacman->objData.nextTryMove;
	else if (!pacman_movable(pacman, M, pacman->objData.preMove)) 
		return;

	switch (pacman->objData.preMove)
	{
	case UP:
		pacman->objData.Coord.y -= 1;
		pacman->objData.preMove = UP;
		break;
	case DOWN:
		pacman->objData.Coord.y += 1;
		pacman->objData.preMove = DOWN;
		break;
	case LEFT:
		pacman->objData.Coord.x -= 1;
		pacman->objData.preMove = LEFT;
		break;
	case RIGHT:
		pacman->objData.Coord.x += 1;
		pacman->objData.preMove = RIGHT;
		break;
	default:
		break;
	}
	pacman->objData.facing = pacman->objData.preMove;
	pacman->objData.moveCD = GAME_TICK_CD;
}

void pacman_eatItem(Pacman* pacman, const char Item) {
	switch (Item)
	{
	case '.':
		stop_bgm(PACMAN_MOVESOUND_ID);
		PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
		break;
	//v TODO-GC-PB: set pacman powerUp mode
	case 'P':
		pacman->powerUp = true;
		//stop_bgm(PACMAN_MOVESOUND_ID);
		//PACMAN_MOVESOUND_ID = play_audio(PACMAN_BEAN, effect_volume);
		break;
	default:
		break;
	}
}

void pacman_NextMove(Pacman* pacman, Directions next) {
	pacman->objData.nextTryMove = next;
}

void pacman_die() {
	//v TODO-GC-game_over: play sound of pacman's death! see shared.c
	// hint: check pacman_eatItem(...) above.
	stop_bgm(PACMAN_MOVESOUND_ID);
	PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume);
}
