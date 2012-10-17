#ifndef _SDL_GPU_RENDERER_H__
#define _SDL_GPU_RENDERER_H__

#include "SDL_gpu.h"
#include "SCMLpp.h"



class Entity
{
    public:
    
    int entity;
    
    class Animation
    {
        public:
        
        int animation;
        int key;
        
        int time;  // milliseconds
        
        Animation();
        Animation(int animation, int key);
    };
    
    Animation current_animation;
    
    Entity();
    Entity(int entity, int animation = 0, int key = 0);
    
    void update(SCML::Data* data, int dt_ms);
    void draw(SCML::Data* data, GPU_Target* screen, float x, float y);
    
    void startAnimation(int animation);
};





#endif
