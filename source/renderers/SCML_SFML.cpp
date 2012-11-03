#include "SCML_SFML.h"
#include "SFML/Graphics.hpp"
#include <cstdlib>
#include <cmath>

using namespace std;

namespace SCML_SFML
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    sf::Texture* img = new sf::Texture;
    img->loadFromFile(filename);
    if(img == NULL)
        return false;
    
    if(!images.insert(make_pair(make_pair(folderID, fileID), img)).second)
    {
        printf("SCML_SFML::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", filename.c_str(), folderID, fileID);
        delete img;
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    for(map<pair<int,int>, sf::Texture*>::iterator e = images.begin(); e != images.end(); e++)
    {
        delete e->second;
    }
    images.clear();
}

std::pair<unsigned int, unsigned int> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    map<pair<int,int>, sf::Texture*>::const_iterator e = images.find(make_pair(folderID, fileID));
    if(e == images.end())
        return make_pair(0,0);
    return make_pair(e->second->getSize().x, e->second->getSize().y);
}

sf::Texture* FileSystem::getImage(int folderID, int fileID) const
{
    map<pair<int,int>, sf::Texture*>::const_iterator e = images.find(make_pair(folderID, fileID));
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

sf::RenderTarget* Entity::setScreen(sf::RenderTarget* scr)
{
    sf::RenderTarget* old = screen;
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
    
    sf::Texture* img = file_system->getImage(folderID, fileID);
    if(img == NULL)
        return;
    
    sf::Sprite sprite(*img);
    sprite.setOrigin(img->getSize().x/2, img->getSize().y/2);
    sprite.setScale(scale_x, scale_y);
    sprite.setRotation(angle);
    sprite.setPosition(x, y);
    
    screen->draw(sprite);
    //SPG_TransformX(img, screen, angle, scale_x, scale_y, img->w/2, img->h/2, x, y, SPG_TBLEND);
}





}

