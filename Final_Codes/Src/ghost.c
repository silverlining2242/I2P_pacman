#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "ghost.h"
#include "map.h"
#include "pacman_obj.h"
/* global variables*/
// [ NOTE ]
// if you cmhange the map .txt to your own design.
// You have to modify cage_grid_{x,y} to corressponding value also.
// Or you can do some change while loading map (reading .txt file)
// Make the start position metadata stored with map.txt.
const int cage_grid_x = 22, cage_grid_y = 11;
/* shared variables. */
extern uint32_t GAME_TICK;
extern uint32_t POWERUP_TICK;
extern uint32_t GAME_TICK_CD;
extern const int block_width, block_height;
extern int GHOST_NUM;		 // #add
extern Cage_grid *Cages; // #add

/* Internal variables */
static const int fix_draw_pixel_offset_x = -3;
static const int fix_draw_pixel_offset_y = -3;
static const int draw_region = 30;
// [ NOTE - speed again ]
// Again, you see this notaficationd. If you still want to implement something
// fancy with speed, objData->moveCD and GAME_TICK, you can first start on
// working on animation of ghosts and pacman. // Once you finished the animation
// part, you will have more understanding on whole mechanism.
static const int basic_speed = 2;

Ghost *ghost_create(int flag)
{

	// NOTODO
	Ghost *ghost = (Ghost *)malloc(sizeof(Ghost));
	if (!ghost)
		return NULL;

	ghost->go_in_time = GAME_TICK;
	ghost->typeFlag = flag; // flag is name of Ghost
	ghost->objData.Size.x = block_width;
	ghost->objData.Size.y = block_height;

	ghost->objData.nextTryMove = NONE;
	ghost->speed = basic_speed;
	ghost->status = BLOCKED;

	ghost->flee_sprite = load_bitmap("Assets/ghost_flee.png");
	ghost->dead_sprite = load_bitmap("Assets/ghost_dead.png");

	int cage_grid2_x = Cages[flag].cage_grid_x;
	int cage_grid2_y = Cages[flag].cage_grid_y;

	// TODO-GC-ghost: Create other type ghost, load corresponding sprites.
	// TODO-IF: You may design your own special tracking rules.
	switch (ghost->typeFlag)
	{
	case Blinky:														 // 0 , idx of ghosts
		ghost->objData.Coord.x = cage_grid2_x; // cage_grid_x
		ghost->objData.Coord.y = cage_grid2_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_move_script_random;
		break;
	case Pinky:
		// *load move script of shortest_path
		ghost->objData.Coord.x = cage_grid2_x;
		ghost->objData.Coord.y = cage_grid2_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_pink.png");
		ghost->move_script = &ghost_move_script_shortest_path;
		break;
	case Inky: // Blue
		// *load move script of shortest_path
		ghost->objData.Coord.x = cage_grid2_x;
		ghost->objData.Coord.y = cage_grid2_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_blue.png");
		ghost->move_script = &ghost_move_script_shortest_path;
		break;
	case Clyde:
		// *load move script of shortest_path
		ghost->objData.Coord.x = cage_grid2_x;
		ghost->objData.Coord.y = cage_grid2_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_orange.png");
		ghost->move_script = &ghost_move_script_shortest_path;
		break;
	default:
		ghost->objData.Coord.x = cage_grid2_x;
		ghost->objData.Coord.y = cage_grid2_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_move_script_random;
		break;
	}
	return ghost;
}
void ghost_destory(Ghost *ghost)
{
	// TODO-GC-memory: free ghost resource

	/*
		al_destory_bitmap(...);
		...
		free(ghost);
	*/
	// image
	al_destroy_bitmap(ghost->dead_sprite);
	al_destroy_bitmap(ghost->flee_sprite);
	al_destroy_bitmap(ghost->move_sprite);
	// malloc
	free(ghost);
}
void ghost_draw(Ghost *ghost) // func to draw single ghost
{
	RecArea drawArea = getDrawArea((object *)ghost, GAME_TICK_CD);

	// #Draw default image //duplicate
	// al_draw_scaled_bitmap(ghost->move_sprite, 0, 0,
	// 											16, 16,
	// 											drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
	// 											draw_region, draw_region, 0);

	// Draw ghost according to its status and use ghost->objData.moveCD value to determine which frame of the animation to draw.
	// hint: please refer comments in pacman_draw
	// Since ghost has more status, we suggest you finish pacman_draw first. The logic is very similar.

	int bitmap_x_offset = 0;

	if (ghost->status == FLEE)
	{
		// $TODO-PB-animation: ghost FLEE animation, draw blue flee sprites,
		//						 while time is running out, alternatively draw blue and white flee sprites.
		// *draw ghost->flee_sprite
		/* hint: try to add some function in scene_game.h and scene_game.c that
			gets the value of `power_up_timer` and `power_up_duration`.
		*/
		// changed speed in toggle, so need to adjust / 2 accordingly, or it won't move during draw
		if (((ghost->objData.moveCD >> 3) & 1) == 0) // change based on speed
			bitmap_x_offset = 0;											 // when CD: even
		else
			bitmap_x_offset = 16; // when CD: odd

		if (POWERUP_TICK >= 7) // it has run out of 70 % of the time of power mode)//blink blue white
		{
			// alternately draw blue and white sprites
			// game_log("about to end PB: %d\n",POWERUP_TICK);
			if (((ghost->objData.moveCD >> 4) & 1) == 0) // flick quicker so set >>4
				bitmap_x_offset += 0;
			else
				bitmap_x_offset += 32;
			al_draw_scaled_bitmap(ghost->flee_sprite,
														bitmap_x_offset, 0,
														16, 16,
														drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
														draw_region, draw_region, 0);
		}
		else // only blue
		{
			// draw only blue sprite
			al_draw_scaled_bitmap(ghost->flee_sprite,
														bitmap_x_offset, 0,
														16, 16,
														drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
														draw_region, draw_region, 0);
		}
	}

	else if (ghost->status == GO_IN)
	{
		// $TODO-PB-animation: ghost going animation
		// *draw ghost->dead_sprite
		switch (ghost->objData.facing) // no leg move, so remove odd/ even
		{
		case RIGHT:
			bitmap_x_offset += 0;
			break;
		case LEFT:
			bitmap_x_offset += 16;
			break;
		case UP:
			bitmap_x_offset += 32;
			break;
		case DOWN:
			bitmap_x_offset += 48;
			break;
		default:
			bitmap_x_offset += 0;
			break;
		}
		al_draw_scaled_bitmap(ghost->dead_sprite,
													bitmap_x_offset, 0,																													// sx, sy
													16, 16,																																			// sw, sh
													drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y, // dx, dy
													draw_region, draw_region, 0);
	}
	else
	{
		// $TODO-GC-animation: ghost animation
		// *draw ghost->move_sprite
		if (((ghost->objData.moveCD >> 4) & 1) == 0)
			bitmap_x_offset = 0; // when CD: even
		else
			bitmap_x_offset = 16; // when CD: odd

		switch (ghost->objData.facing)
		{
		case RIGHT:
			bitmap_x_offset += 0;
			break;
		case LEFT:
			bitmap_x_offset += 32;
			break;
		case UP:
			bitmap_x_offset += 64;
			break;
		case DOWN:
			bitmap_x_offset += 96;
			break;
		default:
			bitmap_x_offset += 0;
			break;
		}
		al_draw_scaled_bitmap(ghost->move_sprite,
													bitmap_x_offset, 0,																													// sx, sy
													16, 16,																																			// sw, sh
													drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y, // dx, dy
													draw_region, draw_region, 0);
	}
}

void ghost_NextMove(Ghost *ghost, Directions next)
{
	ghost->objData.nextTryMove = next;
}
void printGhostStatus(GhostStatus S)
{

	switch (S)
	{

	case BLOCKED: // stay inside the ghost room
		game_log("BLOCKED");
		break;
	case GO_OUT: // going out the ghost room
		game_log("GO_OUT");
		break;
	case FREEDOM: // free at the map
		game_log("FREEDOM");
		break;
	case GO_IN:
		game_log("GO_IN");
		break;
	case FLEE:
		game_log("FLEE");
		break;
	default:
		game_log("status error");
		break;
	}
}
bool ghost_movable(const Ghost *ghost, const Map *M, Directions targetDirec, bool disallow_room)
{
	// TODO-HACKATHON 2-3: Determine if the current direction is movable.
	// Basically, this is a ghost version of `pacman_movable`.
	// So if you have finished (and you should), you can just "copy and paste"
	// and do some small alternation.
	int checkx, checky;
	checkx = ghost->objData.Coord.x;
	checky = ghost->objData.Coord.y;

	switch (targetDirec)
	{
	case UP:
		checky -= 1;
		break;
	case DOWN:
		checky += 1;
		break;
	case LEFT:
		checkx -= 1;
		break;
	case RIGHT:
		checkx += 1;
		break;
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}
	// game_log("(%d,%d)\n",checkx,checky);
	if (is_wall_block(M, checkx, checky) || (disallow_room && is_room_block(M, checkx, checky)))
		return false;

	return true;
	// TODO-HACKATHON 1-2: Determine if the current direction is movable.
	// That is to say, your pacman shouldn't penetrate 'wall' and 'room'
	// , where room is reserved for ghost to set up.
	// 1) For the current direction `targetDirec`, use pre-implemented function
	// `is_wall_block` and `is_room_block` to check if the block is wall or room. (they are both defined in map.c)
	// 2) the coordinate data of pacman is stored in pacman->objData.Coord
	// it is a self-defined pair IntInt type. Trace the code and utilize it.

	//... pacman->objData.Coord.x, ... pacman->objData.Coord.y;
}

void ghost_toggle_FLEE(Ghost *ghost, bool setFLEE)
{
	// $TODO-PB: change ghosts state when power bean is eaten by pacman.
	// When pacman eats the power bean, only ghosts who are in state FREEDOM will change to state FLEE.
	// For those who are not (BLOCK, GO_IN, etc.), they won't change state.
	// Spec above is based on the classic PACMAN game.
	// setFLEE = true => set to FLEE, setFLEE = false => reset to FREEDOM

	if (setFLEE)
	{
		// set FREEDOM ghost's status to FLEE and make them slow
		if (ghost->status == FREEDOM)
		{
			ghost->status = FLEE;
			ghost->speed = 1; // should be 1, change to 2 for testing
		}
	}
	else
	{
		// set FLEE ghost's status to FREESOME and back to prev speed
		if (ghost->status == FLEE)
		{
			ghost->status = FREEDOM;
			ghost->speed = 2;
		}
	}
}

void ghost_collided(Ghost *ghost)
{
	if (ghost->status == FLEE)
	{
		ghost->status = GO_IN;
		ghost->speed = 4; // Go back to cage faster
	}
}
