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
static bool editFlag = false;
static int Inputidx = -1;
/*to change extern variable */
extern Play1Keys[4]; // #add in shared.c will be used in scene_game.c
extern bool isValidPlay1Key;

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
	al_clear_to_color(al_map_rgb(0, 0, 0));
	// set coordinat
	const float offset_w = 30;
	const float offset_h = 30;
	// draw text
	al_draw_text(
			menuFont, al_map_rgb(255, 255, 255),
			offset_w, offset_h, // start position
			ALLEGRO_ALIGN_LEFT,
			"Set Play 1 Key- U D L R:");
	// draw rectangle and Assign RecArea
	setRecArea(&Rectangle1, 400, 25, 150, 35);
	al_draw_rectangle(400, 25, 550, 60, al_map_rgb(255, 255, 255), 3.7); // width 150

	// Draw the set keys
	for (int i = 0; i < 4; i++)
	{
		if (Play1Keys[i] != 0)
		{
			char *keyName = al_keycode_to_name(Play1Keys[i]);
			al_draw_text(menuFont, al_map_rgb(255, 255, 255), 405 + (i * 36) + 5, 28, ALLEGRO_ALIGN_LEFT, keyName);
		}
	}

	// draw image (load first)
	// drawButton(btnSettings);
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f)
{
	//	TODO-HACKATHON 3-7: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
	// btnSettings.hovered = pnt_in_rect(mouse_x, mouse_y, btnSettings.body);
}

static void on_mouse_down()
{
	if (pnt_in_rect(mouse_x, mouse_y, Rectangle1) && Inputidx < 4)
	{
		editFlag = true;
		game_log("Start input keys");
	}
	else // click outside stop input
	{
		editFlag = false;
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
		case ALLEGRO_KEY_P: 
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
		if (Play1Keys[i]!=0)
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