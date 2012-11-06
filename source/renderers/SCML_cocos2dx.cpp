#include "SCML_cocos2dx.h"
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace cocos2d;

namespace SCML_cocos2dx
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
	CCSprite* img = CCSprite::create(filename.c_str());
	
    if(img == NULL)
        return false;
    
    if(!images.insert(make_pair(make_pair(folderID, fileID), img)).second)
    {
        printf("SCML_cocos2dx::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", filename.c_str(), folderID, fileID);
        return false;
    }
    else
		img->retain();
    return true;
}

void FileSystem::clear()
{
    for(map<pair<int,int>, CCSprite*>::iterator e = images.begin(); e != images.end(); e++)
    {
        e->second->release();
    }
    images.clear();
}

std::pair<unsigned int, unsigned int> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    map<pair<int,int>, CCSprite*>::const_iterator e = images.find(make_pair(folderID, fileID));
    if(e == images.end())
        return make_pair(0,0);
    return make_pair(e->second->boundingBox().size.width, e->second->boundingBox().size.height);
}

CCSprite* FileSystem::getImage(int folderID, int fileID) const
{
    map<pair<int,int>, CCSprite*>::const_iterator e = images.find(make_pair(folderID, fileID));
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






void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    //y = -y;
    angle = 360 - angle;
}

std::pair<unsigned int, unsigned int> Entity::getImageDimensions(int folderID, int fileID) const
{
    return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
    //y = -y;
    angle = 360 - angle;
    
    CCSprite* img = file_system->getImage(folderID, fileID);
    if(img == NULL)
        return;

	x += getPositionX();
	y += getPositionY();

	img->setPosition(ccp(x,y));
	img->setRotation(angle);
	img->setScaleX(scale_x);
	img->setScaleY(scale_y);

	img->visit();
}




}

