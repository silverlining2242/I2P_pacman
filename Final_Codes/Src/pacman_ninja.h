#ifndef PACMAN_NINJA_H
#define PACMAN_NINJA_H

#include "utility.h"
#include "game.h"

typedef struct Ghost Ghost;
typedef struct Map Map;

typedef struct Ninja //bonus: don't comment out any of it so we can convert to (object *) in draw function
{
  object objData;
  bitmapdata imgdata;
  int speed;
  bool powerUp; // *important: this boolean indicates if pacman is in power bean mode now.
  ALLEGRO_TIMER *death_anim_counter; 
  ALLEGRO_BITMAP *move_sprite;
  ALLEGRO_BITMAP *die_sprite;
  ALLEGRO_BITMAP *block_sprite; // #TODO-MC
  ALLEGRO_TIMER *cool_counter;  // #TODO-MC

} Ninja;


Ninja *ninja_create(int num);
void ninja_destroy(Ninja *pman);

void ninja_draw(Ninja *pacman);
void ninja_move(Ninja *ninja, Map *M);
void ninja_eatItem(Ninja *pacman, const char Item);

void ninja_NextMove(Ninja *pacman, Directions next);
void ninja_die();

#endif // !PACMAN_OBJ_H
