#include <allegro5/allegro_primitives.h>
#include "pacman_ninja.h"
#include "map.h"
#include "scene_game.h" //#add
/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;                       // where to put pacman at the beginning
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3; // draw offset
static const int draw_region = 30;                                           // pacman bitmap draw region
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
extern ALLEGRO_SAMPLE *PACMAN_MOVESOUND; // use pacman's sound
extern ALLEGRO_SAMPLE *PACMAN_DEATH_SOUND;
extern ALLEGRO_SAMPLE *PACMAN_POWERUPSOUND;
extern uint32_t GAME_TICK;
extern uint32_t POWERUP_TICK;
extern uint32_t GAME_TICK_CD;
extern uint32_t PMANDIE_TICK;
extern bool game_over;
extern float effect_volume;
extern Pair_IntInt *pmanP; // #add

extern Pair_IntInt *fruits; // bonus from map

/* Declare static function */
static bool ninja_movable(const Ninja *ninja, const Map *M, Directions targetDirec)
{
  int checkx, checky;
  checkx = ninja->objData.Coord.x;
  checky = ninja->objData.Coord.y;

  switch (targetDirec) // use nextTry to check if movable
  {
  case UP: // check pman above position is movable?
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
  // TODO-MC

  if (is_wall_block(M, checkx, checky) || is_room_block(M, checkx, checky))
    return false;

  return true;
}

Ninja *ninja_create(int num) // change
{
  // get position from current pman[0]
  int ninja_grid2_x = fruits[0].x;
  int ninja_grid2_y = fruits[0].y;
  // Allocate dynamic memory for pman pointer;
  Ninja *pman = (Ninja *)malloc(sizeof(Ninja));
  // Pacman *pman2 = (Pacman *)malloc(sizeof(Pacman)); // TODO-MC duplicate

  if (!pman)
    return NULL;
  pman->objData.Coord.x = ninja_grid2_x; 
  pman->objData.Coord.y = ninja_grid2_y;
  pman->objData.Size.x = block_width;
  pman->objData.Size.y = block_height;

  pman->objData.preMove = NONE;
  pman->objData.nextTryMove = NONE;
  pman->speed = basic_speed;

  pman->death_anim_counter = al_create_timer(1.0f / 8.0f);
  // pman->powerUp = false;
  /* load sprites */

  pman->move_sprite = load_bitmap("Assets/pacman_move.png");

  pman->die_sprite = load_bitmap("Assets/pacman_die.png");
  game_log("ninja_create execute");

  return pman;
}

void ninja_destroy(Ninja *pman)
{
//   // $TODO-GC-memory: free pacman resource
//   // image
//   al_destroy_bitmap(pman->move_sprite);
//   al_destroy_bitmap(pman->die_sprite);
//   // if (pman->block_sprite) //this has error #FIX
//   // 	al_destroy_bitmap(pman->block_sprite);
//   //  timer //TODO-MC
//   al_destroy_timer(pman->death_anim_counter);
//   // if (pman->cool_counter) #FIX
//   // 	al_destroy_timer(pman->cool_counter);
//   // malloc
//   free(pman);
  game_log("ninja.c is executed");
}

void ninja_draw(Ninja *pman)
{
  game_log("ninja_draw executed");

  RecArea drawArea = getDrawArea((object *)pman, GAME_TICK_CD);

  // Draw default image: this is before animation(duplicate)
  // al_draw_scaled_bitmap(pman->move_sprite,
  // 											0, 0,																																				// sx, sy
  // 											16, 16,																																			// sw, sh
  // 											drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y, // dx, dy
  // 											draw_region, draw_region, 0);																								// dw, dh, flag

  int offset = 0; // sx frame count decide sx position of image
  if (!game_over)
  {
    // $TODO-GC-animation: We have two frames for each direction. You can use the value of pman->objData.moveCD to determine which frame of the animation to draw.
    // For example, if the value if odd, draw 1st frame. Otherwise, draw 2nd frame.
    // But this frame rate may be a little bit too high. We can use % 32 and draw 1st frame if value is 0~15, and 2nd frame if value is 16~31.
    if (((pman->objData.moveCD >> 4) & 1) == 0) //(pman->objData.moveCD % 2 == 0) //(pman->objData.moveCD & 1) == 0)
    {
      offset = 0; // even
    }
    else if (((pman->objData.moveCD >> 4) & 1) == 1) //(pman->objData.moveCD % 2 == 1)
    {
      offset = 16; // odd
    }

    /*
    NOTE: since modulo operation is expensive in clock cycle perspective (reference: https://stackoverflow.com/questions/27977834/why-is-modulus-operator-slow)
      , you can use & (bitwise and) operator to determine a value is odd or even.
      e.g. If (val & 1 == 1) is true then `val` is odd. If (val & 1 == 0) is false then `val` is even.
      e.g. Similarly, if ((val>>4) & 1 == 0) is true then `val % 32` is 0~15, if ((val>>4) & 1 == 1) is true then `val % 32` is 16~31.
    */

    switch (pman->objData.facing)
    {
    case RIGHT:
      offset += 0;
      break;
    case LEFT:
      offset += 32;
      break;
    case UP:
      offset += 64;
      break;
    case DOWN:
      offset += 96;
      break;
    default:
      offset += 0;
      break;
    }
    al_draw_scaled_bitmap(pman->move_sprite,
                          offset, 0,                                                                  // sx, sy
                          16, 16,                                                                     // sw, sh
                          drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y, // dx, dy
                          draw_region, draw_region, 0);
    // sx, sy: the position from the image file
    // dx, dy: position in the whole map
  }
  else // die anim
  {
    // $TODO-GC-animation: Draw die animation(pman->die_sprite)
    // hint: instead of using pman->objData.moveCD, use pman->death_anim_counter to create animation.
    // refer al_get_timer_count and al_draw_scaled_bitmap. Suggestion frame rate: 8fps.
    // in struct: pman->death_anim_counter = al_create_timer(1.0f / 8.0f); tick time each 1/8 s (return timer object)

    offset = ((al_get_timer_count(pman->death_anim_counter)) % 12) * 16; // 16 * 192(192/16 = 12)
                                                                         // offset: (iterate idx in die_sprite image each 1/8 s) * 16 (each is 16* 16)

    al_draw_scaled_bitmap(pman->die_sprite,
                          offset, 0,                                                                  // sx, sy
                          16, 16,                                                                     // sw, sh
                          drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y, // dx, dy
                          draw_region, draw_region, 0);
  }
}

void ninja_move(Ninja *ninja, Map *M)
{
  if (!movetime(ninja->speed))
    return;
  if (game_over)
    return;
  // decide : NextTryMove: call shortest_path in map.c
  ninja->objData.nextTryMove = shortest_path_direc(M, fruits[0].x, fruits[0].y, 4, 10); // for test
  if (ninja_movable(ninja, M, ninja->objData.nextTryMove))
    ninja->objData.preMove = ninja->objData.nextTryMove;
  else if (!ninja_movable(ninja, M, ninja->objData.preMove))
    return;
  // actual move
  switch (ninja->objData.preMove)
  {
  case UP:
    ninja->objData.Coord.y -= 1;
    ninja->objData.preMove = UP;
    break;
  case DOWN:
    ninja->objData.Coord.y += 1;
    ninja->objData.preMove = DOWN;
    break;
  case LEFT:
    ninja->objData.Coord.x -= 1;
    ninja->objData.preMove = LEFT;
    break;
  case RIGHT:
    ninja->objData.Coord.x += 1;
    ninja->objData.preMove = RIGHT;
    break;
  default:
    break;
  }
  ninja->objData.facing = ninja->objData.preMove;
  ninja->objData.moveCD = GAME_TICK_CD;
}
void ninja_eatItem(Ninja *pacman, const char Item) // for sound
{
  switch (Item)
  {
  case '.': // ninja can only eat normal beans
    stop_bgm(PACMAN_MOVESOUND_ID);
    PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
    break;
    // $TODO-GC-PB: set pacman powerUp mode
  default:
    break;
  }
}

void ninja_NextMove(Ninja *pacman, Directions next)
{
  pacman->objData.nextTryMove = next;
}

void ninja_die() // play PACMAN_DEATH_SOUND
{
  stop_bgm(PACMAN_MOVESOUND_ID);
  PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume); // play once
}

void stop_NINJA_POWERUPSOUND() // create function: to used in scene_game.c
{
  stop_bgm(PACMAN_MOVESOUND_ID);
}
