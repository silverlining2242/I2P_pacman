
#include "ghost.h"
#include "pacman_obj.h"
#include "map.h"
/* Shared variables */
#define GO_OUT_TIME 256
extern uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern uint32_t POWERUP_TICK;
extern ALLEGRO_TIMER *game_tick_timer;
extern ALLEGRO_TIMER *power_up_tick_timer;
extern const int cage_grid_x, cage_grid_y;

/* Declare static function prototypes */
// static function reference: https://stackoverflow.com/questions/558122/what-is-a-static-function-in-c
static void ghost_move_script_FREEDOM_random(Ghost *ghost, Map *M);
static void ghost_move_script_FREEDOM_shortest_path(Ghost *ghost, Map *M, Pacman *pman);
static void ghost_move_script_BLOCKED(Ghost *ghost, Map *M);
static void ghost_move_script_GO_IN(Ghost *ghost, Map *M);
static void ghost_move_script_GO_OUT(Ghost *ghost, Map *M);
static void ghost_move_script_FLEE(Ghost *ghost, Map *M, const Pacman *const pacman);
static void ghost_move_script_STOP(Ghost *ghost, Map *M); // #add

static void ghost_move_script_FREEDOM_random(Ghost *ghost, Map *M)
{
	// TODO-HACKATHON 2-4: Uncomment the following code and finish pacman picking random direction.
	// hint: see generateRandomNumber in utility.h (duplicate)

	// static Directions proba[4]; // possible movement
	// int cnt = 0;
	// for (Directions i = 1; i <= 4; i++) // i is directions
	// {
	// 	if (ghost_movable(ghost, M, i, true)) // set true to break out the ghost block B
	// 		proba[cnt++] = i;										// available movable directions
	// }
	// ghost_NextMove(ghost, proba[generateRandomNumber(0, cnt - 1)]); // after iter cnt + 1 so -1 (gen 0 - 3)

	// $TODO-GC-random_movement: (Not in Hackathon)
	// Description:
	// For red(Blinky) ghost, we ask you to implement an random strategy ghost,
	// which means moving in random direction.
	// But your random strategy have to designed carefully so that ghost won't walk back and forth.
	// (The code above DO perform walking back and forth.)
	// Replace the above code by finish followings.
	// hint: record the previous move, and skip it when adding direction into array proba

	Directions counter_one;					// init with RIGHT
	switch (ghost->objData.preMove) // preMove(decide to move), the last direct we moved
	{
	case RIGHT:						// if last time we go R, next we should not go back L
		counter_one = LEFT; // counter_one: the last direction we should not go back
		break;
	case LEFT:
		counter_one = RIGHT;
		break;
	case UP:
		counter_one = DOWN;
		break;
	case DOWN:
		counter_one = UP;
		break;
	default:
		counter_one = NONE;
		break;
	}

	static Directions proba[4]; // possible movement
	int cnt = 0;
	for (Directions i = 1; i <= 4; i++)													// i is directions
		if (i != counter_one && ghost_movable(ghost, M, i, true)) // set true to break out the ghost block B
			proba[cnt++] = i;																				// available movable directions
	if (cnt >= 1)
	{ // ghost->objData.nextTryMove = next; random choose available move
		ghost_NextMove(ghost, proba[generateRandomNumber(0, cnt - 1)]);
	}
	else
	{ // for the dead end case, move original back
		ghost_NextMove(ghost, counter_one);
	}
}

static void ghost_move_script_FREEDOM_shortest_path(Ghost *ghost, Map *M, Pacman *pman)
{
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pman->objData.Coord.x, pman->objData.Coord.y);
}

static void ghost_move_script_BLOCKED(Ghost *ghost, Map *M)
{

	switch (ghost->objData.preMove)
	{
	case UP:
		if (ghost->objData.Coord.y == cage_grid_x ) //#change 10
			ghost_NextMove(ghost, DOWN);
		else
			ghost_NextMove(ghost, UP);
		break;
	case DOWN:
		if (ghost->objData.Coord.y == cage_grid_y ) //12
			ghost_NextMove(ghost, UP);
		else
			ghost_NextMove(ghost, DOWN);
		break;
	default:
		ghost_NextMove(ghost, UP);
		break;
	}
}

static void ghost_move_script_GO_IN(Ghost *ghost, Map *M)
{
	// Description
	// `shortest_path_direc` is a function that returns the direction of shortest path.
	// Check `map.c` for its detail usage.
	// For GO_IN state.
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, cage_grid_x, cage_grid_y); // #need to check ghost_movable(ghost, M, i, true) or not?
	ghost_NextMove(ghost, ghost->objData.nextTryMove);
}

static void ghost_move_script_GO_OUT(Ghost *ghost, Map *M)
{
	// Description
	// Here we always assume the room of ghosts opens upward.
	// And used a greedy method to drag ghosts out of room.
	// You should modify here if you have different implementation/design of room.
	if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B')
		ghost_NextMove(ghost, UP);
	else
		ghost->status = FREEDOM;
}

static void ghost_move_script_STOP(Ghost *ghost, Map *M) // #add
{
	ghost->objData.nextTryMove = NONE;
	ghost_NextMove(ghost, ghost->objData.nextTryMove);
}
static void ghost_move_script_FLEE(Ghost *ghost, Map *M, const Pacman *const pacman)
{
	// $TODO-PB: escape from pacman
	// Description:
	// The concept here is to simulate ghosts running away from pacman while pacman is having power bean ability.
	// To achieve this, think in this way. We first get the direction to shortest path to pacman, call it K (K is either UP, DOWN, RIGHT or LEFT).
	// Then we choose other available direction rather than direction K.
	// In this way, ghost will escape from pacman.
	// use FREEDOM MODE for testing
	/**/
	Directions counter_one;					// init with RIGHT
	switch (ghost->objData.preMove) // preMove(decide to move), the last direct we moved
	{
	case RIGHT:						// if last time we go R, next we should not go back L
		counter_one = LEFT; // counter_one: the last direction we should not go back
		break;
	case LEFT:
		counter_one = RIGHT;
		break;
	case UP:
		counter_one = DOWN;
		break;
	case DOWN:
		counter_one = UP;
		break;
	default:
		counter_one = NONE;
		break;
	}
	// if pacman move, ghost go opposite, if not go random
	Directions shortestDirection = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pacman->objData.Coord.x, pacman->objData.Coord.y);
	Directions antishortestDirection = shortestDirection;
	switch (shortestDirection)
	{
	case RIGHT:
		antishortestDirection = LEFT;
		break;
	case LEFT:
		antishortestDirection = RIGHT;
		break;
	case UP:
		antishortestDirection = DOWN;
		break;
	case DOWN:
		antishortestDirection = UP;
		break;
	default:
		break;
	}
	if (ghost_movable(ghost, M, antishortestDirection, true))
		ghost_NextMove(ghost, antishortestDirection); // ghost->objData.nextTryMove is updated to the direction
	else																						// if it's wall change to random directions beside shortestD
	{
		static Directions proba[4];
		proba[0] = shortestDirection;
		int cnt = 1;																																					// start from 1
		for (Directions i = 1; i <= 4; i++)																										// i is directions
			if (i != counter_one && i != shortestDirection && ghost_movable(ghost, M, i, true)) // set true to break out the ghost block B
				proba[cnt++] = i;																																	// available movable directions
		if (cnt >= 2)																																					// beside contest and short has two choice
		{																																											// ghost->objData.nextTryMove = next; random choose available move
			ghost_NextMove(ghost, proba[generateRandomNumber(1, cnt - 1)]);
		}
		else if (cnt == 1 && ghost_movable(ghost, M, shortestDirection, true))
		{ // for the dead end case, move original back
			ghost_NextMove(ghost, proba[0]);
		}
		else
		{
			ghost_NextMove(ghost, counter_one);
		}
	}
}

void ghost_move_script_random(Ghost *ghost, Map *M, Pacman *pacman)
{
	if (!movetime(ghost->speed))
		return;
	switch (ghost->status)
	{
	case BLOCKED:
		ghost_move_script_BLOCKED(ghost, M);
		if (al_get_timer_count(game_tick_timer) > GO_OUT_TIME)
			ghost->status = GO_OUT;
		break;
	case FREEDOM:
		ghost_move_script_FREEDOM_random(ghost, M);
		break;
	case GO_OUT:
		ghost_move_script_GO_OUT(ghost, M);
		break;
	case GO_IN:
		ghost_move_script_GO_IN(ghost, M);
		if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B')
		{
			ghost->status = GO_OUT;
			ghost->speed = 2; // reset the speed after back to the cage.
		}
		break;
	case FLEE:
		ghost_move_script_FLEE(ghost, M, pacman);
		break;
	case STOP:
		ghost_move_script_STOP(ghost, M);
		break;
	default:
		break;
	}

	if (ghost_movable(ghost, M, ghost->objData.nextTryMove, false)) // nextTryMove is for check
	{
		ghost->objData.preMove = ghost->objData.nextTryMove;
		ghost->objData.nextTryMove = NONE;
	}
	else if (!ghost_movable(ghost, M, ghost->objData.preMove, false))
		return;

	switch (ghost->objData.preMove) // use preMove to decide where to walk
	{
	case RIGHT:
		ghost->objData.Coord.x += 1;
		break;
	case LEFT:
		ghost->objData.Coord.x -= 1;
		break;
	case UP:
		ghost->objData.Coord.y -= 1;
		break;
	case DOWN:
		ghost->objData.Coord.y += 1;
		break;
	default:
		break;
	}
	ghost->objData.facing = ghost->objData.preMove;
	ghost->objData.moveCD = GAME_TICK_CD;
}

void ghost_move_script_shortest_path(Ghost *ghost, Map *M, Pacman *pacman)
{
	// TODO-GC-movement: do a little modification on shortest path move script
	// Since always shortest path strategy is too strong, player have no chance to win this.
	// hint: Do shortest path sometime and do random move sometime.
	if (!movetime(ghost->speed))
		return;
	switch (ghost->status)
	{
	case BLOCKED:
		ghost_move_script_BLOCKED(ghost, M);
		if (al_get_timer_count(game_tick_timer) - ghost->go_in_time > GO_OUT_TIME)
			ghost->status = GO_OUT;
		break;
	case FREEDOM:
		ghost_move_script_FREEDOM_shortest_path(ghost, M, pacman);
		break;
	case GO_OUT:
		ghost_move_script_GO_OUT(ghost, M);
		break;
	case GO_IN:
		ghost_move_script_GO_IN(ghost, M);
		if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B')
		{
			ghost->status = BLOCKED;
			ghost->speed = 2; // reset the speed after back to the cage.
			ghost->go_in_time = al_get_timer_count(game_tick_timer);
		}
		break;
	case FLEE:
		ghost_move_script_FLEE(ghost, M, pacman);
		break;
	default:
		break;
	}

	if (ghost_movable(ghost, M, ghost->objData.nextTryMove, false))
	{
		ghost->objData.preMove = ghost->objData.nextTryMove;
		ghost->objData.nextTryMove = NONE;
	}
	else if (!ghost_movable(ghost, M, ghost->objData.preMove, false))
		return;

	switch (ghost->objData.preMove)
	{
	case RIGHT:
		ghost->objData.Coord.x += 1;
		break;
	case LEFT:
		ghost->objData.Coord.x -= 1;
		break;
	case UP:
		ghost->objData.Coord.y -= 1;
		break;
	case DOWN:
		ghost->objData.Coord.y += 1;
		break;
	default:
		break;
	}
	ghost->objData.facing = ghost->objData.preMove;
	ghost->objData.moveCD = GAME_TICK_CD;
}