#include "SCML_SDL_gpu.h"
#include <list>

using namespace std;
using namespace SCML_SDL_gpu;

GPU_Target* screen = NULL;
Uint8* keystates = NULL;


void main_loop(SCML::Data* data)
{
    
    FileSystem fs;
    fs.load(data);
    printf("Loaded %zu images.\n", fs.images.size());
    
    list<Entity*> entities;
    for(map<int, SCML::Data::Entity*>::iterator e = data->entities.begin(); e != data->entities.end(); e++)
    {
        Entity* entity = new Entity(e->first);
        entity->setFileSystem(&fs);
        entity->setScreen(screen);
        entities.push_back(entity);
    }
    printf("Loaded %zu entities.\n", entities.size());
    
    float x = 400.0f;
    float y = 300.0f;
    float angle = 0.0f;
    float scale = 1.0f;
    
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
        
        if(keystates[SDLK_UP])
            y -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_DOWN])
            y += 100*dt_ms/1000.0f;
        if(keystates[SDLK_LEFT])
            x -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_RIGHT])
            x += 100*dt_ms/1000.0f;
        if(keystates[SDLK_MINUS])
            scale -= 0.2f*dt_ms/1000.0f;
        else if(keystates[SDLK_EQUALS] || keystates[SDLK_PLUS])
            scale += 0.2f*dt_ms/1000.0f;
        if(keystates[SDLK_COMMA])
            angle -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_PERIOD])
            angle += 100*dt_ms/1000.0f;
        
        for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
        {
            (*e)->update(data, dt_ms);
        }
        
        GPU_Clear(screen);
        
        
        for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
        {
            (*e)->draw(data, x, y, angle, scale, scale);
        }
        
        
        GPU_Flip();
        SDL_Delay(10);
        
        Uint32 framefinish = SDL_GetTicks();
        dt_ms = framefinish - framestart;
        framestart = framefinish;
    }
    
    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        delete (*e);
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


