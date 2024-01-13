#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"

// TODO-HACKATHON 2-0: Create one ghost
// Just modify the GHOST_NUM to 1
// #define GHOST_NUM 4
int GHOST_NUM = 4;
// $TODO-GC-ghost: create a least FOUR ghost!
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern uint32_t POWERUP_TICK;
extern uint32_t PMANDIE_TICK;
extern ALLEGRO_TIMER *game_tick_timer;
int game_main_Score = 0;
bool game_over = false;
char *beans_text;						 // #add
extern int Play1Keys[4];		 // #add from shared.c
extern bool isValidPlay1Key; // #add from shared.c
extern bool dieAnimDone;		 // #add

/* Internal variables*/
static ALLEGRO_TIMER *power_up_timer; // why it static??
static const int power_up_duration = 10;
static Pacman *pman;
static Map *basic_map;
static Ghost **ghosts;
bool debug_mode = false;
bool cheat_mode = false;
bool CM_K = false; // TODO-CM
bool CM_S = false;
bool wasCM_S = false; // #add to improve efficiency to avoid reset normal when CM_S not active
bool CM_L = false;

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

static void init(void)
{
	game_over = false;
	game_main_Score = 0;
	// create map
	basic_map = create_map(NULL);
	// $TODO-GC-read_txt: Create map from .txt file so that you can design your own map!!
	// basic_map = create_map("Assets/map_nthu.txt"); //*okay
	if (!basic_map)
	{
		game_abort("error on creating map");
	}
	// create pacman
	pman = pacman_create();
	if (!pman)
	{
		game_abort("error on creating pacamn\n");
	}

	// allocate ghost memory
	// TODO-HACKATHON 2-1: Allocate dynamic memory for ghosts array.
	ghosts = (Ghost **)malloc(sizeof(Ghost *) * GHOST_NUM);

	//
	if (!ghosts)
	{
		game_log("We haven't create any ghosts!\n");
	}
	else
	{
		// TODO-HACKATHON 2-2: create a ghost.
		// Try to look the definition of ghost_create in ghost.h and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++)
		{
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick per second
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	else
		game_log("power_up_timer created\n");
	return;
}

static void step(void)
{
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++)
	{
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void)
{
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// TODO-HACKATHON 1-3: check which item you are going to eat and use `pacman_eatItem` to deal with it.

	switch (basic_map->map[Grid_y][Grid_x]) // it's reverse
	{
	case '.':
		// game_main_Score
		game_main_Score++;
		pacman_eatItem(pman, '.');
		break;
	case 'P':
		// $TODO-GC-PB: ease power bean
		pman->powerUp = true;
		// eat and play bgm
		pacman_eatItem(pman, 'P');
		// stop and reset power_up_timer value
		// start the timer
		POWERUP_TICK = set_power_up_timer_tick(1); // mode 1 to init
		game_log("power_up_timer start at : %dn", POWERUP_TICK);
		break;
	default:
		break;
	}

	// TODO-HACKATHON 1-4: erase the item you eat from map
	// Be careful, don't erase the wall block.
	if (basic_map->map[Grid_y][Grid_x] == '.')
		basic_map->map[Grid_y][Grid_x] = ' ';
	// PB
	if (basic_map->map[Grid_y][Grid_x] == 'P')
		basic_map->map[Grid_y][Grid_x] = ' ';
}
static void status_update(void)
{
	// $TODO-PB: check powerUp duration

	if (pman->powerUp)
	{
		// change ghost status
		for (int i = 0; i < GHOST_NUM; i++)
		{
			// bool setFLEE = (ghosts[i]->status == FLEE) ? true : false;
			ghost_toggle_FLEE(ghosts[i], true);
		}
		// Check the value of power_up_timer
		POWERUP_TICK = al_get_timer_count(power_up_timer); // start from 0 count to 10(included)

		if (POWERUP_TICK >= power_up_duration) // PB end >= 10, it start from 0
		{
			pman->powerUp = false;
			game_log(" %d exceed %d, switch powerUP to %d\n", POWERUP_TICK, power_up_duration, pman->powerUp);
			set_power_up_timer_tick(0); // mode 0 to stop and reset count to 0
			game_log("powerup_tick: %d\n", POWERUP_TICK);
			// call function to stop POWERUPSOUND
			stop_PACMAN_POWERUPSOUND();

			// If runs out of time reset all relevant variables and ghost's status back to FREEDOM
			for (int i = 0; i < GHOST_NUM; i++)
			{
				// use toggle to change status
				// bool setFLEE = (ghosts[i]->status == FREEDOM) ? true : false;
				ghost_toggle_FLEE(ghosts[i], false);
			}
		}
		// hint: ghost_toggle_FLEE
		// ghost status to FLEE
	}
	// TODO-IF: Cheat Mode
	if (cheat_mode)
	{
		// game_log("cheat_mode in status_update");
		if (CM_S)
		{
			if (!wasCM_S) // only set stop when it's not already stop
			{
				for (int i = 0; i < GHOST_NUM; i++)
					ghost_toggle_STOP(ghosts[i], true);
				wasCM_S = true;
			}
			if (CM_K) //press K still valid
			{
				for (int i = 0; i < GHOST_NUM; i++)
					ghost_toggle_GOIN(ghosts[i], true);
				CM_K = false; // Reset CM_K
			}
		}
		else
		{
			if (wasCM_S) // reset stop only if prev CM_S is active
			{
				for (int i = 0; i < GHOST_NUM; i++)
					ghost_toggle_STOP(ghosts[i], false);
				wasCM_S = false;
			}
			if (CM_K)
			{
				for (int i = 0; i < GHOST_NUM; i++)
					ghost_toggle_GOIN(ghosts[i], true);
				CM_K = false; // Reset CM_K
			}
		}
		// if (CM_K)
		// {
		//  for loop to set ghost_toggle_GOIN(ghosts[i], true);
		// 	CM_K = false; // reset
		// }
		// if (CM_S)
		// {
		// 		for loop to set ghost_toggle_STOP(ghosts[i], true);// not reset until press again
		// }
		// else // error: keep update to normal so overwrite CM_K
		// {
		// 		for loop to set ghost_toggle_STOP(ghosts[i], false);
		// }
	}
	else // if cheatmode off but there is K, L wipe out
	{
		if (wasCM_S || CM_K || CM_S)
		{
			for (int i = 0; i < GHOST_NUM; i++)
			{
				ghost_toggle_STOP(ghosts[i], false);
				ghost_toggle_GOIN(ghosts[i], false);
			}
			CM_K = false;
			CM_S = false;
			wasCM_S = false;
		}
		CM_L = false;
	}
	// draw pmanArea for check collide
	RecArea pmanArea = getDrawArea((object *)pman, GAME_TICK_CD);
	for (int i = 0; i < GHOST_NUM; i++)
	{
		// draw ghost
		RecArea ghostArea = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		bool isCollide = RecAreaOverlap(&pmanArea, &ghostArea);
		if (ghosts[i]->status == GO_IN)
		{
			continue;
		}
		else if (ghosts[i]->status == FREEDOM || ghosts[i]->status == STOP) // #add
		{
			// $TODO-GC-game_over: use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect if pacman and ghosts collide with each other.
			// And perform corresponding operations.
			// [NOTE] You should have some if-else branch here if you want to implement power bean mode.
			// Uncomment Following Code

			if (isCollide) //(!cheat_mode && isCollide) #add
			{
				game_log("collide with ghost %d\n", i);
				al_rest(1.0);

				pacman_die();			// play die sound
				game_over = true; // it will set up timer for death_anim (pman->death_anim_counter)
				break;						// animation shouldn't be trigger twice.
			}
		}
		else if (ghosts[i]->status == FLEE) // ghost eaten by pacman
		{
			// $TODO-GC-PB: if ghost is collided by pacman, it should go back to the cage immediately and come out after a period.

			if (isCollide) //(!cheat_mode && isCollide) //#add
			{
				ghost_collided(ghosts[i]); // pacman eat ghost, change ghost status to GO_IN
			}
		}
	}
}

static void update(void)
{

	if (game_over)
	{
		// $TODO-GC-game_over: start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
		// hint: refer al_get_timer_started(...), al_get_timer_count(...), al_stop_timer(...), al_rest(...)
		/*
			// start the timer if it hasn't been started.
			// check timer counter.
			// stop the timer if counter reach desired time.
			game_change_scene(...);
		*/
		// set timer (pman->death_anim_counter)

		if (!al_get_timer_started(pman->death_anim_counter))
		{
			al_start_timer(pman->death_anim_counter);
		}
		// pacman die anim stop
		else
		{
			PMANDIE_TICK = al_get_timer_count(pman->death_anim_counter);
			// Stop the timer and reset count
			if (PMANDIE_TICK >= 12 - 1)
			{
				al_stop_timer(pman->death_anim_counter);
				al_set_timer_count(pman->death_anim_counter, 0); // reset
				PMANDIE_TICK = al_get_timer_count(pman->death_anim_counter);
				game_log("PMANDIE_TICK:%d\n", PMANDIE_TICK); // #if not change scene it will not stop
				dieAnimDone = true;
			}
		}
		// check change scene
		if (dieAnimDone)
		{
			game_change_scene(scene_menu_create());
			dieAnimDone = false; // back to false for next round
		}

		return;
	}
	// $TODO-PB: add all beans finish game
	if (game_main_Score >= basic_map->beansNum)
	{
		game_change_scene(scene_menu_create());
		return;
	}
	// regular game update logic
	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++)
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void)
{

	al_clear_to_color(al_map_rgb(0, 0, 0));

	// $TODO-GC-scoring: Draw scoreboard, something your may need is sprinf();
	beans_text = (char *)malloc(sizeof(char) * 35);
	sprintf(beans_text, "All: %d    Eat: %d", basic_map->beansNum, game_main_Score);
	al_draw_text(
			menuFont, al_map_rgb(255, 255, 0),
			10, 10, 0,
			beans_text);
	// $TODO-CM: draw on
	if (cheat_mode)
	{
		al_draw_text(
				menuFont, al_map_rgb(255, 255, 0),
				650, 5, 0,
				"Cheat On");
	}

	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);

	// debugging mode
	if (debug_mode)
	{
		draw_hitboxes();
	}
}

static void draw_hitboxes(void)
{
	RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
	al_draw_rectangle(
			pmanHB.x, pmanHB.y,
			pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2);

	for (int i = 0; i < GHOST_NUM; i++)
	{
		RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		al_draw_rectangle(
				ghostHB.x, ghostHB.y,
				ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
				al_map_rgb_f(1.0, 0.0, 0.0), 2);
	}
}

static void printinfo(void)
{
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}

static void destroy(void)
{
	// TODO-CM: make sure cheat mode back to false #add
	cheat_mode = false;
	// $TODO-GC-memory: free map array, Pacman and ghosts
	// pacman
	pacman_destroy(pman);
	// ghost
	for (int i = 0; i < GHOST_NUM; i++)
	{
		ghost_destory(ghosts[i]); // func delete each ghost
	}
	// map
	delete_map(basic_map);
	// malloc
	// free(pman); // don't double free!! comment out
	// free(basic_map);
	free(ghosts);
	free(beans_text);
	// check
	game_log("scene_game.c destroy() func called\n");

	for (int i = 0; i < 4; i++)
	{
		game_log("%d Play1Keys: %d", i, Play1Keys[i]);
	}
	game_log("key full %d", isValidPlay1Key);
}

static void on_key_down(int key_code)
{
	if (isValidPlay1Key)
	{
		// can't use switch case Play1Keys[0]: The case labels in a switch statement must be constant expressions known at compile time, but the contents of Play1Keys are determined at runtime.
		if (key_code == Play1Keys[0])
			pacman_NextMove(pman, UP);
		else if (key_code == Play1Keys[2])
			pacman_NextMove(pman, LEFT);
		else if (key_code == Play1Keys[1])
			pacman_NextMove(pman, DOWN);
		else if (key_code == Play1Keys[3])
			pacman_NextMove(pman, RIGHT);
	}
	else
	{
		switch (key_code)
		{
		// TODO-HACKATHON 1-1: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman, DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		}
	}
	switch (key_code)
	{
	case ALLEGRO_KEY_C:
		cheat_mode = !cheat_mode;
		if (!cheat_mode)
		{
			CM_S = false;
			CM_K = false;
			printf("cheat mode off and turn of CM_S, CMK\n");
		}
		else
		{
			printf("cheat mode on\n");
		}
		break;
	case ALLEGRO_KEY_G:
		debug_mode = !debug_mode;
		break;
	default:
		break;
	}

	if (cheat_mode)
	{
		switch (key_code)
		{
		case ALLEGRO_KEY_K:
			CM_K = !CM_K;
			printf("ghosts start going back to the cage\n");
			break;
		case ALLEGRO_KEY_S:
			CM_S = !CM_S;
			printf("ghosts stop moving\n");
			break;
		case ALLEGRO_KEY_L:
			CM_L = !CM_L;
			printf("pacman can cross wall\n");
			break;
		default:
			break;
		}
	}
}

static void on_mouse_down(int btn, int x, int y, int dz)
{

	// nothing here
}

static void render_init_screen(void)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++)
	{
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 0),
			400, 400,
			ALLEGRO_ALIGN_CENTER,
			"READY!");

	al_flip_display();
	al_rest(2.0);
}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void)
{
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

int32_t set_power_up_timer_tick(int mode) // templaet: get_power_up_timer_tick
{
	if (mode == 1)
	{
		if (al_get_timer_started(power_up_timer)) // if already start, then stop
			al_stop_timer(power_up_timer);
		al_set_timer_count(power_up_timer, 0); // reset timer to 0 and restart
		al_start_timer(power_up_timer);
		return al_get_timer_count(power_up_timer); // return the num of ticks, first init is 0
	}
	if (mode == 0)
	{
		if (al_get_timer_started(power_up_timer)) // if already start, then stop
			al_stop_timer(power_up_timer);
		al_set_timer_count(power_up_timer, 0);
		POWERUP_TICK = 0;
		return al_get_timer_count(power_up_timer);
	}
}
int32_t get_power_up_duration()
{
	return al_get_timer_count(power_up_timer);
}