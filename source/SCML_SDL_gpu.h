#ifndef _SDL_GPU_RENDERER_H__
#define _SDL_GPU_RENDERER_H__

#include "SDL_gpu.h"
#include "SCMLpp.h"
#include <map>

namespace SCML_SDL_gpu
{
    
class FileSystem
{
    public:
    
    // Folder, File
    std::map<std::pair<int, int>, GPU_Image*> images;
    
    void load(SCML::Data* data);
    void clear();
    
    GPU_Image* getImage(int folder, int file) const;
    
};

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
    void draw(SCML::Data* data, SCML_SDL_gpu::FileSystem* fs, GPU_Target* screen, float x, float y, float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f);
    
    void startAnimation(int animation);
};

}



#endif
