/********************************************************************
                            OpenAlchemist
 
  File : GameEngine.cpp
  Description : 
  License : GNU General Public License 2 or +
  Author : Guillaume Delhumeau <guillaume.delhumeau@gmail.com>
 
 
 *********************************************************************/

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "GameEngine.h"
#include "Preferences.h"
#include "CommonResources.h"
#include "misc.h"
#include "LoadingScreen.h"

GameEngine::GameEngine(CL_DisplayWindow *window, bool opengl)
{
    this -> _p_window = window;
    this -> _render_mode = opengl;
    _p_loading_screen = NULL;
}

GameEngine::~GameEngine()
{}

void GameEngine::init()
{
    _running = true;

    _p_loading_screen = new LoadingScreen();
    _p_loading_screen -> set_progression(0.0f);

    CommonResources *resources = common_resources_get_instance();
    Preferences *pref = pref_get_instance();

    _fps_getter.set_fps_limit(pref -> maxfps);

    resources -> init(this);
    _common_state.init();
    _ingame_state.init();
    _gameover_state.init();
    _pausemenu_state.init();
    _skinsmenu_state.init();
    _optionsmenu_state.init();
    _title_state.init();
    _quitmenu_state.init();

    _p_loading_screen -> set_progression(1.0f / 12.0f);

    set_skin(pref -> skin);
    resize(_p_window -> get_width(), _p_window -> get_height());

    delete _p_loading_screen;
    _p_loading_screen = NULL;
}

void GameEngine::run()
{
    init();

    if(_running)
    {
        set_state_title();

        CommonResources *resources = common_resources_get_instance();
        resources -> player1.new_game();

        while (_running)
        {
            _common_state.events();
            _common_state.update();
            _common_state.draw();

            GameState* current_state = _states_stack.top();
            current_state -> events();
            current_state -> update();

            // Drawing the front layer behind the current state or not
            if (current_state -> front_layer_behind())
            {
                resources -> front_layer.draw();
                current_state -> draw();
            }
            else
            {
                current_state -> draw();
                resources -> front_layer.draw();
            }


            // Get the Framerate
            resources -> fps = _fps_getter.get_fps();
            resources -> time_interval = get_time_interval(resources->fps);


            CL_Display::flip();

            // This call updates input and performs other "housekeeping"
            // Call this each frame
            // Also, gives the CPU a rest for 10 milliseconds to catch up
            CL_System::keep_alive();
        }
    }
}

void GameEngine::stop()
{
    _running = false;
}

void GameEngine::set_state_title()
{
    while (!_states_stack.empty())
    {
        _states_stack.pop();
    }
    _states_stack.push(&_title_state);
    _title_state.start();
}

void GameEngine::set_state_new_game_menu()
{}

void GameEngine::set_state_pause_menu()
{
    if (_states_stack.top() != &_pausemenu_state)
    {
        _states_stack.push(&_pausemenu_state);
        _pausemenu_state.start();
    }
}

void GameEngine::set_state_ingame()
{
    CommonResources *common_resources = common_resources_get_instance();
    common_resources -> p_current_player = &(common_resources -> player1);
    _states_stack.push(&_ingame_state);
}

void GameEngine::set_state_gameover(int mode)
{
    _gameover_state.set_mode(mode);
    _gameover_state.start();
    _states_stack.push(&_gameover_state);
}

/*void GameEngine::set_state_highscore()
 {
         states_stack.push(&highscore_state);
 }*/


void GameEngine::set_state_options_menu()
{
    if (_states_stack.top() != &_optionsmenu_state)
    {
        _states_stack.push(&_optionsmenu_state);
        _optionsmenu_state.start();
        _pausemenu_state.start();
    }
}

void GameEngine::set_state_skin_menu()
{
    if (_states_stack.top() != &_skinsmenu_state)
    {
        _states_stack.push(&_skinsmenu_state);
        _skinsmenu_state.start();
        _pausemenu_state.start();
        _optionsmenu_state.start();
    }
}

void GameEngine::set_state_quit_menu(int action)
{
    if (_states_stack.top() != &_quitmenu_state)
    {
        _quitmenu_state.set_action(action);
        _states_stack.push(&_quitmenu_state);
        _pausemenu_state.start();
        _quitmenu_state.start();
    }
}

void GameEngine::stop_current_state()
{
    _states_stack.pop();
}

void GameEngine::toggle_screen()
{
    /*Preferences *pref = pref_get_instance();
     pref -> fullscreen = !pref -> fullscreen;

     if(pref -> fullscreen)
     {
             window->set_fullscreen(800,600,0,0);
             CL_Mouse::hide();

             if(pref -> widescreen && opengl)
             {
                     CL_GraphicContext *gc = window -> get_gc();
                     gc -> set_scale(0.83, 1.0);
                     gc -> add_translate(80, 0, 0);					
             }
     }
     else
     {
             window->set_windowed();
             CL_Mouse::show();

             CL_GraphicContext *gc = window -> get_gc();
             gc -> set_scale(1.0, 1.0);
             gc -> set_translate(0, 0, 0);

     }

     pref -> write();*/

    if (_p_window -> get_width() == 800)
    {

        _p_window -> set_size(640, 480);
        CL_GraphicContext *gc = _p_window -> get_gc();
        double scale_width = 640 / 800.0;
        double scale_height = 480 / 600.0;
        gc -> set_scale(scale_width, scale_height);
        gc -> add_translate(0, 150, 0);
    }
    else
    {
        _p_window -> set_size(800, 600);
        CL_GraphicContext *gc = _p_window -> get_gc();
        gc -> set_scale(1.0, 1.0);
    }


}

/**
 * Called when user resize the window
 */
void GameEngine::resize(int width, int height)
{
    //static int old_width = 0;
    //static int old_height = 0;

    if (!_p_window -> is_fullscreen())
    {
        CL_GraphicContext *gc = _p_window -> get_gc();

        double ratio = (double) width / (double) height;

        /*if(old_width != width || old_height != height)
         {			
                 old_width = width;
                 old_height = height;			

                 if(ratio > 800.0 / 600.0 * 1.01)
                 {
                         width = height * 1.33;
                         window -> set_size(width, height);
                 }
                 else if (ratio < 800.0 / 600.0 * 0.99)
                 {
                         height = width * 0.75;
                         window -> set_size(width, height);
                 }

                 double scale_width = width / 800.0;
                 double scale_height= height / 600.0;
                 gc -> set_scale(scale_width, scale_height);
         }
         else*/
        {

            if (ratio > 800.0 / 600.0 * 1.01)
            {
                double n_width = height * 1.333333;
                int dx = (width - n_width) / 2;
                double scale_width = n_width / 800.0;
                double scale_height = height / 600.0;
                gc -> set_scale(scale_width, scale_height);
                gc -> add_translate(dx, 0, 0);

            }
            else if (ratio < 800.0 / 600.0 * 0.99)
            {
                double n_height = width * 0.75;
                int dy = (height - n_height) / 2;
                double scale_width = width / 800.0;
                double scale_height = n_height / 600.0;
                gc -> set_scale(scale_width, scale_height);
                gc -> add_translate(0, dy, 0);
            }

        }

    }
}

int GameEngine::get_fps()
{
    return _fps_getter.get_fps();
}

bool GameEngine::is_opengl_used()
{
    return _render_mode;
}

bool GameEngine::is_fullscreen()
{
    Preferences *pref = pref_get_instance();
    return pref -> fullscreen;
}

void GameEngine::set_skin(std::string skin)
{
    CommonResources *resources = common_resources_get_instance();
    Preferences *pref = pref_get_instance();

    std::string old_skin = pref -> skin;

    try
    {
        pref -> skin = skin;

        if(_running)
        {
            resources -> load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(2.0f / 12.0f);
        }

        if(_running)
        {
            _title_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(3.0f / 12.0f);
        }

        if(_running)
        {
            _common_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(4.0f / 12.0f);
        }

        if(_running)
        {
            _ingame_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(5.0f / 12.0f);
        }


        if(_running)
        {
            _gameover_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(6.0f / 12.0f);
        }


        if(_running)
        {
            _pausemenu_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(7.0f / 12.0f);
        }


        if(_running)
        {
            _skinsmenu_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(8.0f / 12.0f);
        }


        if(_running)
        {
            _optionsmenu_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(9.0f / 12.0f);
        }

        if(_running)
        {
            _optionsmenu_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(10.0f / 12.0f);
        }


        if(_running)
        {
            _title_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(11.0f / 12.0f);
        }


        if(_running)
        {
            _quitmenu_state.load_gfx(pref -> skin);
            _p_loading_screen -> set_progression(12.0f / 12.0f);
        }

        pref -> write();

    }
    catch (CL_Error err)
    {
        std::cout << "Skin error : " << err.message << std::endl;
        std::cout << "Error in : " << skin << std::endl;
        if (old_skin.compare(skin))
        {
            std::cout << "Now loading default skin." << std::endl;
            skin = get_skins_path() + get_path_separator() + "aqua.zip";
            set_skin(skin);
        }
        else
        {
            throw err;
        }
    }
}

void GameEngine::set_skin_element(u_int element)
{
    _skinsmenu_state.set_skin_elements(element);
}