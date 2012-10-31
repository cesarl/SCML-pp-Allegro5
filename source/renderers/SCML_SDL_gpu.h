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
    
    virtual ~FileSystem();
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename);
    virtual void clear();
    virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const;
    
    GPU_Image* getImage(int folderID, int fileID) const;
    
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
    
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle);
    virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const;
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y);
};

}



#endif
