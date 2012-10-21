#ifndef _SDL_GPU_RENDERER_H__
#define _SDL_GPU_RENDERER_H__

#include "SDL_gpu.h"
#include "../SCMLpp.h"
#include <map>

namespace SCML_SDL_gpu
{
    
class FileSystem : public SCML::FileSystem
{
    public:
    
    // Folder, File
    std::map<std::pair<int, int>, GPU_Image*> images;
    // TODO: Destructor, etc.
    
    virtual ~FileSystem();
    virtual void load(SCML::Data* data);
    virtual void clear();
    
    GPU_Image* getImage(int folder, int file) const;
    
};

class Entity : public SCML::Entity
{
    public:
    
    SCML_SDL_gpu::FileSystem* file_system;
    GPU_Target* screen;
    
    Entity();
    Entity(int entity, int animation = 0, int key = 0);
    
    SCML_SDL_gpu::FileSystem* setFileSystem(SCML_SDL_gpu::FileSystem* fs);
    GPU_Target* setScreen(GPU_Target* scr);
    
    virtual void draw(SCML::Data* data, float x, float y, float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f);
};

}



#endif
