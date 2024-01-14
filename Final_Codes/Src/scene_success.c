#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

/* Internal Variables*/
static ALLEGRO_BITMAP *gameTitle = NULL;
static ALLEGRO_SAMPLE_ID goodBGM;
static int gameTitleW;
static int gameTitleH;
/*Variable carries from scene_settings.c*/

//
static void init()
{

  gameTitle = load_bitmap("Assets/gamecomplete2.png");
  gameTitleW = al_get_bitmap_width(gameTitle);
  gameTitleH = al_get_bitmap_height(gameTitle);
  stop_bgm(goodBGM);
  goodBGM = play_bgm(goodsound, music_volume);
}

static void draw()
{

  al_clear_to_color(al_map_rgb(0, 0, 0));

  const float scale = 0.7;
  const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
  const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;

  // draw title image
  al_draw_scaled_bitmap(
      gameTitle,
      0, 0,
      gameTitleW, gameTitleH,
      offset_w, offset_h,
      gameTitleW * scale, gameTitleH * scale,
      0);
  al_draw_text(
      menuFont,
      al_map_rgb(255, 255, 255),
      SCREEN_W / 2,
      SCREEN_H - 150,
      ALLEGRO_ALIGN_CENTER,
      "PRESS <ENTER>");

  // TODO-HACKATHON 3-3: Draw button
  // Uncomment and fill the code below
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f)
{

}

static void on_mouse_down()
{

}

static void destroy()
{
  stop_bgm(goodBGM);
  al_destroy_bitmap(gameTitle);
  //	TODO-HACKATHON 3-10: Destroy button images
  //	Uncomment and fill the code below
}

static void on_key_down(int keycode)
{

  switch (keycode)
  {
  case ALLEGRO_KEY_ENTER:
    game_change_scene(scene_menu_create());
    break;
  default:
    break;
  }
}

Scene scene_success_create(void)
{

  Scene scene;
  memset(&scene, 0, sizeof(Scene));
  scene.name = "Success";
  scene.initialize = &init;
  scene.draw = &draw;
  scene.destroy = &destroy;
  scene.on_key_down = &on_key_down;
  scene.on_mouse_move = &on_mouse_move;
  scene.on_mouse_down = &on_mouse_down;
  // -------------------------------------

  game_log("Success scene created");
  return scene;
}