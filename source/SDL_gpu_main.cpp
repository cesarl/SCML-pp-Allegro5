#include "SCML_SDL_gpu.h"
#include <list>

using namespace std;

GPU_Target* screen = NULL;
Uint8* keystates = NULL;


void main_loop(SCML::Data* data)
{
    list<Entity*> entities;
    for(map<int, SCML::Data::Entity*>::iterator e = data->entities.begin(); e != data->entities.end(); e++)
    {
        entities.push_back(new Entity(e->first));
    }
    
    bool done = false;
    SDL_Event event;
    int dt_ms = 0;
    Uint32 framestart = SDL_GetTicks();
    
    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                else if(event.key.keysym.sym == SDLK_SPACE)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->startAnimation(rand()%data->getNumAnimations((*e)->entity));
                    }
                }
            }
        }
        
        for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
        {
            (*e)->update(data, dt_ms);
        }
        
        GPU_Clear(screen);
        
        
        for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
        {
            (*e)->draw(data, screen, 400, 300);
        }
        
        
        GPU_Flip();
        SDL_Delay(10);
        
        Uint32 framefinish = SDL_GetTicks();
        dt_ms = framefinish - framestart;
        framestart = framefinish;
    }
    
}




bool init(unsigned int w, unsigned int h)
{
    SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center"));
    
    screen = GPU_Init(NULL, 800, 600, SDL_SWSURFACE | SDL_DOUBLEBUF);

    if(screen == NULL)
    {
        printf("Couldn't set video mode %dx%d: %s\n", 800, 600, SDL_GetError());
		return false;
    }

    SDL_WM_SetCaption("SCML demo", NULL);
    
    keystates = SDL_GetKeyState(NULL);
    
    return true;
}

void quit()
{
    GPU_Quit();
    SDL_Quit();
}


