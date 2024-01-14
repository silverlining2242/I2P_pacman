// TODO-HACKATHON 3-9: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
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
#include "scene_settings.h"

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO-IF: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
/*variables should have static!*/
static RecArea Rectangle1;
static RecArea Checkbox1;
static RecArea Checkbox2;
static bool editFlag = false;
static int Inputidx = -1;
/*to change extern variable */
extern Play1Keys[4]; // #add in shared.c will be used in scene_game.c
extern bool isValidPlay1Key;
extern int PMAN_NUM; // check MC
extern bool compete_mode;

/* funcs copy from scene_menu.c*/
static void init()
{
	ALLEGRO_EVENT event;
	// const char button1_image_path[50] = "Assets/settings.png";
	// const char button2_image_path[50] = "Assets/settings2.png";
	// btnSettings = button_create(730, 20, 50, 50, button1_image_path, button2_image_path);

	// gameTitle = load_bitmap("Assets/title.png");
	// gameTitleW = al_get_bitmap_width(gameTitle);
	// gameTitleH = al_get_bitmap_height(gameTitle);
	// stop_bgm(menuBGM);
	// menuBGM = play_bgm(themeMusic, music_volume);
}

static void draw(void)
{
	// init to black
	al_clear_to_color(al_map_rgb(0, 0, 0));
	// // for position line check
	// for (int i = 0; i < 800; i += 50)
	// {
	// 	for (int j = 0; j < 810; j += 50)
	// 	{
	// 		al_draw_rectangle(i, j, i + 50, j - 50, al_map_rgb(255, 255, 255), 2); // x y start x y end
	// 	}
	// }

	// draw
	al_draw_text(
			menuFont, al_map_rgb(255, 255, 255),
			50, 50, // start position
			ALLEGRO_ALIGN_LEFT,
			"SET PLAYER 1 KEY U-D-L-R:");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255), 120, 100, ALLEGRO_ALIGN_LEFT, "DO NOT SET REGULAR DIRECTION KEY");

	// draw rectangle and Assign RecArea
	setRecArea(&Rectangle1, 120, 130, 300, 50);
	al_draw_rectangle(120, 130, 420, 180, al_map_rgb(255, 255, 255), 3.5); // 120, 420

	// Draw text input
	for (int i = 0; i < 4; i++)
	{
		if (Play1Keys[i] != 0)
		{
			char *keyName = al_keycode_to_name(Play1Keys[i]);
			al_draw_text(menuFont, al_map_rgb(255, 255, 255), 128 + (i * 36), 140, ALLEGRO_ALIGN_LEFT, keyName); // 2, y
		}
	}
	// Draw Multi mode on/ off
	al_draw_text(menuFont, al_map_rgb(255, 255, 0), 50, 200, ALLEGRO_ALIGN_LEFT, "MUTIPLAYER MODE:");
	al_draw_text(menuFont, al_map_rgb(255, 255, 0), 120, 270, ALLEGRO_ALIGN_LEFT, "COLLABORATE  KEY: / ");
	al_draw_text(menuFont, al_map_rgb(255, 255, 0), 120, 340, ALLEGRO_ALIGN_LEFT, "COMPETE           KEY: SPACE");
	al_draw_text(menuFont, al_map_rgb(255, 255, 0), 120, 400, ALLEGRO_ALIGN_LEFT, "PLAYER 2 KEY: REGULAR KEY");
	// draw tick
	setRecArea(&Checkbox1, 50, 250, 50, 50);
	al_draw_rectangle(50, 250, 100, 300, al_map_rgb(255, 255, 0), 3.5); // x y start x y end

	setRecArea(&Checkbox2, 50, 320, 50, 50);
	al_draw_rectangle(50, 320, 100, 370, al_map_rgb(255, 255, 0), 3.5); // x y start x y end

	// draw solid tick
	if (PMAN_NUM == 2)
		al_draw_filled_rectangle(55, 255, 95, 295, al_map_rgb(255, 255, 0)); // x y start x y end
	if (compete_mode)
		al_draw_filled_rectangle(55, 325, 95, 365, al_map_rgb(255, 255, 0)); // x y start x y end

	// draw back
	al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			800 / 2,
			800 - 150,
			ALLEGRO_ALIGN_CENTER,
			"BACK TO MENU PRESS <Q>");
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f)
{
	//	TODO-HACKATHON 3-7: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
	// btnSettings.hovered = pnt_in_rect(mouse_x, mouse_y, btnSettings.body);
}

static void on_mouse_down()
{
	// check click region
	// input cus key
	if (pnt_in_rect(mouse_x, mouse_y, Rectangle1) && Inputidx < 4)
	{
		editFlag = true;
		game_log("Start input keys");
	}
	else // click outside stop input
	{
		editFlag = false;
	}
	// checkbox
	if (pnt_in_rect(mouse_x, mouse_y, Checkbox1)) //collab
	{
		if(PMAN_NUM==1 && compete_mode!=1) //switch on
			PMAN_NUM =2;
		else //off
			PMAN_NUM =1;
		game_log("click checkbox1 and switch PMANNUM: %d",PMAN_NUM);
	}
	if (pnt_in_rect(mouse_x, mouse_y, Checkbox2)) //compete
	{
		if(compete_mode==0 && PMAN_NUM !=2) //switch on
			compete_mode =1;
		else //off
			compete_mode =0;
		game_log("click checkbox2 and switch compete: %d ", compete_mode);
	}
}
static void on_key_down(int keycode)
{
	if (editFlag)
	{
		switch (keycode)
		{
		case ALLEGRO_KEY_ENTER:
			editFlag = 0;
			break;
		case ALLEGRO_KEY_BACKSPACE:
			// delete
			if (Inputidx >= 0) //*check boundary
				Play1Keys[Inputidx--] = 0;
			break;
		default:
			// input
			if (Inputidx < 3)
				Play1Keys[++Inputidx] = keycode;
			break;
		}
	}
	else
	{
		switch (keycode)
		{
		case ALLEGRO_KEY_Q:
			game_change_scene(scene_menu_create());
			break;
		default:
			break;
		}
	}
}

static void destroy() // copy from scene_menu.c
{
	// check
	for (int i = 0; i < 4; i++)
		game_log("setting keys: %d", Play1Keys[i]);

	for (int i = 0; i < 4; i++) // before changing scene, we can update valid
	{
		if (Play1Keys[i] != 0)
		{
			isValidPlay1Key = true;
		}
		else
		{
			isValidPlay1Key = false;
			break;
		}
	}
	game_log("key full %d", isValidPlay1Key);
}
// The only function that is shared across files.
Scene scene_settings_create(void)
{
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene %d created", scene.name);
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