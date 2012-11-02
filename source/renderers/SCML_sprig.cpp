#include "SCML_sprig.h"
#include "SDL_image.h"
#include <cstdlib>
#include <cmath>

using namespace std;

namespace SCML_sprig
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    SDL_Surface* img = IMG_Load(filename.c_str());
    if(img == NULL)
        return false;
    
    // Convert to display format for optimized blitting
    SDL_Surface* tmp = img;
    img = SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(tmp);
    
    if(img == NULL)
        return false;
    
    if(!images.insert(make_pair(make_pair(folderID, fileID), img)).second)
    {
        printf("SCML_sprig::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", filename.c_str(), folderID, fileID);
        SDL_FreeSurface(img);
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    for(map<pair<int,int>, SDL_Surface*>::iterator e = images.begin(); e != images.end(); e++)
    {
        SDL_FreeSurface(e->second);
    }
    images.clear();
}

std::pair<unsigned int, unsigned int> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    map<pair<int,int>, SDL_Surface*>::const_iterator e = images.find(make_pair(folderID, fileID));
    if(e == images.end())
        return make_pair(0,0);
    return make_pair(e->second->w, e->second->h);
}

SDL_Surface* FileSystem::getImage(int folderID, int fileID) const
{
    map<pair<int,int>, SDL_Surface*>::const_iterator e = images.find(make_pair(folderID, fileID));
    if(e == images.end())
        return NULL;
    return e->second;
}






    
Entity::Entity()
    : SCML::Entity()
{}

Entity::Entity(SCML::Data* data, int entity, int animation, int key)
    : SCML::Entity(data, entity, animation, key)
{}

FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

SDL_Surface* Entity::setScreen(SDL_Surface* scr)
{
    SDL_Surface* old = screen;
    screen = scr;
    return old;
}






void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    y = -y;
    angle = 360 - angle;
}

std::pair<unsigned int, unsigned int> Entity::getImageDimensions(int folderID, int fileID) const
{
    return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
    y = -y;
    angle = 360 - angle;
    
    SDL_Surface* img = file_system->getImage(folderID, fileID);
    
    SPG_TransformX(img, screen, angle, scale_x, scale_y, img->w/2, img->h/2, x, y, SPG_TBLEND);
}





}

