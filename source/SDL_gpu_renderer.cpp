#include "SDL_gpu.h"
#include "SCMLpp.h"

GPU_Target* screen = NULL;
Uint8* keystates = NULL;

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

void main_loop(SCML::Data* data)
{
    data->log();
    
    bool done = false;
    SDL_Event event;
    
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
            }
        }
        
        GPU_Clear(screen);
        
        
        
        GPU_Flip();
        
        SDL_Delay(10);
    }
    
}
