// **********************************************************************
//                            OpenAlchemist
//                        ---------------------
//
//  File        : OptionsMenuState.h
//  Description : 
//  Author      : Guillaume Delhumeau <guillaume.delhumeau@gmail.com>
//  License     : GNU General Public License 2 or higher
//
// **********************************************************************

#include "../memory.h"
#include <ClanLib/core.h>

#include "OptionsMenuState.h"
#include "../CommonResources.h"
#include "../GameEngine.h"
#include "../misc.h"
#include "../Preferences.h"
#include "../AudioManager.h"

/************************************************************************/
/* Items                                                                */
/************************************************************************/
enum{
	OPTIONS_ITEM_RENDER,
	OPTIONS_ITEM_FULLSCREEN,
	OPTIONS_ITEM_FRAMERATE,
	OPTIONS_ITEM_COLORBLIND,
	OPTIONS_ITEM_SOUND,
	OPTIONS_ITEM_MUSIC,
	OPTIONS_ITEM_QUIT
};

/************************************************************************/
/* Render choices                                                       */
/************************************************************************/
enum{
#ifdef WITH_DX_9
	RENDER_CHOICE_DX_9 = 0,
	RENDER_CHOICE_OPENGL,
	RENDER_CHOICE_OPENGL2,
	RENDER_CHOICE_SOFTWARE,
#else
	RENDER_CHOICE_OPENGL = 0,
	RENDER_CHOICE_OPENGL2,
	RENDER_CHOICE_SOFTWARE
#endif
};

/************************************************************************/
/* No/Yes                                                               */
/************************************************************************/
enum{
	ITEM_NO = 0,
	ITEM_YES
};

/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
OptionsMenuState::OptionsMenuState()
{
}

/************************************************************************/
/* Destructor                                                           */
/************************************************************************/
OptionsMenuState::~OptionsMenuState()
{
	term();
}

/************************************************************************/
/* Init                                                                 */
/************************************************************************/
void OptionsMenuState::init()
{
	_items.clear();
	_items.insert (_items.end (), &_render_item);
	_items.insert (_items.end (), &_fullscreen_item);
	_items.insert (_items.end (), &_framerate_item);
	_items.insert (_items.end (), &_colorblind_item);
	_items.insert (_items.end (), &_sound_level_item);
	_items.insert (_items.end (), &_music_level_item);
	_items.insert (_items.end (), &_quit_item);
}

/************************************************************************/
/* Term                                                                 */
/************************************************************************/
void OptionsMenuState::term()
{
	unload_gfx();
}

/************************************************************************/
/* Load GFX                                                             */
/************************************************************************/
void OptionsMenuState::load_gfx(CL_GraphicContext& gc, std::string skin)
{
	unload_gfx();

	// Getting skins resources
	CL_VirtualFileSystem vfs(skin, true);
	CL_VirtualDirectory vd(vfs, "./");	
	CL_ResourceManager gfx("menu_options.xml",vd);

	// First, the sprites
	_background = CL_Sprite(gc, "menu_options/dialog_background", &gfx);


	int x = 400 - _background.get_width () / 2;
	int y = 300 - _background.get_height () / 2;

	_render_item.set_description_sprites(gc, gfx,
		"menu_options/render/unselected",
		"menu_options/render/selected");		
#ifdef WITH_DX_9
	_render_item.add_choice(gc, gfx, "menu_options/render-choices/dx9");
#endif
	_render_item.add_choice(gc, gfx, "menu_options/render-choices/opengl");
	_render_item.add_choice(gc, gfx, "menu_options/render-choices/opengl2");
	_render_item.add_choice(gc, gfx, "menu_options/render-choices/software");

	_fullscreen_item.clear_choices();
	_fullscreen_item.set_description_sprites(gc, gfx,
		"menu_options/fullscreen/unselected",
		"menu_options/fullscreen/selected");

	_fullscreen_item.add_choice(gc,  gfx, "menu_options/item-no");
	_fullscreen_item.add_choice(gc,  gfx, "menu_options/item-yes");

	_framerate_item.set_description_sprites(gc, gfx,
		"menu_options/framerate/unselected",
		"menu_options/framerate/selected");

	_framerate_item.clear_choices();
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/30");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/40");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/50");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/60");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/80");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/100");
	_framerate_item.add_choice(gc, gfx, "menu_options/framerate-choices/no-limit");

	_colorblind_item.set_description_sprites(gc, gfx,
		"menu_options/colorblind/unselected",
		"menu_options/colorblind/selected");
	_colorblind_item.clear_choices();
	_colorblind_item.add_choice(gc, gfx, "menu_options/item-no");
	_colorblind_item.add_choice(gc, gfx, "menu_options/item-yes");

	_sound_level_item.set_description_sprites(gc, gfx, 
		"menu_options/sound/unselected",
		"menu_options/sound/selected");

	_music_level_item.set_description_sprites(gc, gfx, 
		"menu_options/music/unselected",
		"menu_options/music/selected");

	_quit_item.set_gfx(gc, gfx, 
		"menu_options/quit/unselected",
		"menu_options/quit/selected");

	_sound_level_item.clear_choices();
	_music_level_item.clear_choices();

	for(int i=0; i<=10; ++i)
	{
		_sound_level_item.add_choice(gc, gfx, "menu_options/sound_level/"+to_string(i));
		_music_level_item.add_choice(gc, gfx, "menu_options/sound_level/"+to_string(i));
	}

	_sound_level_item.set_x(x + CL_Integer_to_int("menu_options/sound/left", &gfx));
	_sound_level_item.set_y(y + CL_Integer_to_int("menu_options/sound/top", &gfx));

	_sound_level_item.set_choice_x(x + CL_Integer_to_int("menu_options/sound/left", &gfx)+250);
	_sound_level_item.set_choice_y(y + CL_Integer_to_int("menu_options/sound/top", &gfx));

	_music_level_item.set_x(x + CL_Integer_to_int("menu_options/music/left", &gfx));
	_music_level_item.set_y(y + CL_Integer_to_int("menu_options/music/top", &gfx));

	_music_level_item.set_choice_x(x + CL_Integer_to_int("menu_options/music/left", &gfx)+250);
	_music_level_item.set_choice_y(y + CL_Integer_to_int("menu_options/music/top", &gfx));		

	_render_item.set_x(x + CL_Integer_to_int("menu_options/render/left", &gfx));
	_render_item.set_y(y + CL_Integer_to_int("menu_options/render/top", &gfx));

	_render_item.set_choice_x(x + CL_Integer_to_int("menu_options/render-choices/left", &gfx));
	_render_item.set_choice_y(y + CL_Integer_to_int("menu_options/render/top", &gfx));

	_fullscreen_item.set_x(x + CL_Integer_to_int("menu_options/fullscreen/left", &gfx));
	_fullscreen_item.set_y(y + CL_Integer_to_int("menu_options/fullscreen/top", &gfx));

	_fullscreen_item.set_choice_x(x + CL_Integer_to_int("menu_options/fullscreen-choices/left", &gfx));
	_fullscreen_item.set_choice_y(y + CL_Integer_to_int("menu_options/fullscreen/top", &gfx));	

	_colorblind_item.set_x(x + CL_Integer_to_int("menu_options/colorblind/left", &gfx));
	_colorblind_item.set_y(y + CL_Integer_to_int("menu_options/colorblind/top", &gfx));

	_colorblind_item.set_choice_x(x + CL_Integer_to_int("menu_options/colorblind-choices/left", &gfx));
	_colorblind_item.set_choice_y(y + CL_Integer_to_int("menu_options/colorblind/top", &gfx));	

	_framerate_item.set_x(x + CL_Integer_to_int("menu_options/framerate/left", &gfx));
	_framerate_item.set_y(y + CL_Integer_to_int("menu_options/framerate/top", &gfx));

	_framerate_item.set_choice_x(x + CL_Integer_to_int("menu_options/framerate-choices/left", &gfx));
	_framerate_item.set_choice_y(y + CL_Integer_to_int("menu_options/framerate/top", &gfx));	

	_quit_item.set_x(x + CL_Integer_to_int("menu_options/quit/left", &gfx));
	_quit_item.set_y(y + CL_Integer_to_int("menu_options/quit/top", &gfx));

	Preferences *p_pref = pref_get_instance();		
	switch(p_pref->render_target)
	{
#ifdef WITH_DX_9
		case Preferences::DX_9:
			_render_item.set_current_choice(RENDER_CHOICE_DX_9);
#endif
		case Preferences::OPENGL_1:
			_render_item.set_current_choice(RENDER_CHOICE_OPENGL);
			break;
		case Preferences::OPENGL_2:
			_render_item.set_current_choice(RENDER_CHOICE_OPENGL2);
			break;
		case Preferences::SOFTWARE:
			_render_item.set_current_choice(RENDER_CHOICE_SOFTWARE);
			break;
	}

	if(p_pref -> fullscreen)
	{
		_fullscreen_item.set_current_choice(ITEM_YES);
	}
	else
	{
		_fullscreen_item.set_current_choice(ITEM_NO);
	}
	if(p_pref -> colorblind)
	{
		_colorblind_item.set_current_choice(ITEM_YES);
	}
	else
	{
		_colorblind_item.set_current_choice(ITEM_NO);
	}
	if(p_pref -> maxfps <= 30)
	{
		_framerate_item.set_current_choice(0);
	}
	else if(p_pref -> maxfps <= 40)
	{
		_framerate_item.set_current_choice(1);
	}
	else if(p_pref -> maxfps <= 50)
	{
		_framerate_item.set_current_choice(2);
	}
	else if(p_pref -> maxfps <= 60)
	{
		_framerate_item.set_current_choice(3);
	}
	else if(p_pref -> maxfps <= 80)
	{
		_framerate_item.set_current_choice(4);
	}
	else if(p_pref -> maxfps <= 100)
	{
		_framerate_item.set_current_choice(5);
	}
	else
	{
		_framerate_item.set_current_choice(6);
	}

	_sound_level_item.set_current_choice(p_pref -> sound_level / 10);
	_music_level_item.set_current_choice(p_pref -> music_level / 10);

}

/************************************************************************/
/* Unload GFX                                                           */
/************************************************************************/
void OptionsMenuState::unload_gfx()
{
	_render_item.unload_gfx();
	_fullscreen_item.unload_gfx();
	_framerate_item.unload_gfx();
	_sound_level_item.unload_gfx();
	_music_level_item.unload_gfx();	
}

/************************************************************************/
/* Action performed                                                     */
/************************************************************************/
void OptionsMenuState::action_performed(int selection, ActionType action_type)
{
	if(ACTION_TYPE_ENTER == action_type)				
	{
		switch(selection)
		{					
		case OPTIONS_ITEM_QUIT:
			{							
				_p_common_resources -> p_engine -> stop_current_state ();
			}
			break;
		}					
	}
}

/************************************************************************/
/* Update child                                                         */
/************************************************************************/
void OptionsMenuState::update_child()
{
	Preferences* p_pref = pref_get_instance();
	switch(_render_item.get_current_choice())
	{
	case RENDER_CHOICE_SOFTWARE:
		if(p_pref->render_target != Preferences::SOFTWARE)
		{		
			p_pref->render_target = Preferences::SOFTWARE;
			p_pref->write();
		}
		break;
	case RENDER_CHOICE_OPENGL:
		if(p_pref->render_target != Preferences::OPENGL_1)
		{
			p_pref->render_target = Preferences::OPENGL_1;
			p_pref->write();
		}
		break;
	case RENDER_CHOICE_OPENGL2:
		if(p_pref->render_target != Preferences::OPENGL_2)
		{
			p_pref->render_target = Preferences::OPENGL_2;
			p_pref->write();
		}
		break;
#ifdef WITH_DX_9
	case RENDER_CHOICE_DX_9:
		if(p_pref->render_target != Preferences::DX_9)
		{
			p_pref->render_target = Preferences::DX_9;
			p_pref->write();
		}
		break;
#endif
	}
	
	bool fullscreen = (int) _fullscreen_item.get_current_choice() == ITEM_YES;
	if(p_pref -> fullscreen != fullscreen)
	{
		_p_common_resources->p_engine->toggle_screen();
	}

	bool colorblind = (int) _colorblind_item.get_current_choice() == ITEM_YES;
	if(p_pref->colorblind != colorblind)
	{
		_p_common_resources->p_engine->toggle_colorblind();
	}	

	if((int)_sound_level_item.get_current_choice() != p_pref -> sound_level / 10)
	{
		p_pref -> sound_level = (int) _sound_level_item.get_current_choice() * 10;
		g_audio_manager.set_sounds_volume(p_pref -> sound_level / 100.0f);
		p_pref -> write();
	}

	if((int)_music_level_item.get_current_choice() != p_pref -> music_level / 10)
	{
		p_pref -> music_level = (int) _music_level_item.get_current_choice() * 10;
		g_audio_manager.set_music_volume(p_pref -> music_level / 100.0f);
		p_pref -> write();
	}

	// Framerate limit
	int maxfps = 60;
	switch(_framerate_item.get_current_choice())
	{
	case 0:
		maxfps = 30;
		break;
	case 1:
		maxfps = 40;
		break;
	case 2:
		maxfps = 50;
		break;
	case 3:
		maxfps = 60;
		break;
	case 4:
		maxfps = 80;
		break;
	case 5:
		maxfps = 100;
		break;
	case 6:
		maxfps = 1000;
		break;						
	}
	if(maxfps != p_pref->maxfps)
	{
		p_pref->maxfps = maxfps;
		_p_common_resources -> p_engine -> refresh_framerate_limit();
	}
}

/************************************************************************/
/* Toggle screen                                                        */
/************************************************************************/
void OptionsMenuState::toggle_screen()
{
	Preferences* p_pref = pref_get_instance();		
	if(p_pref -> fullscreen)
	{
		_fullscreen_item.set_current_choice(ITEM_YES);
	}
	else
	{
		_fullscreen_item.set_current_choice(ITEM_NO);
	}	
}