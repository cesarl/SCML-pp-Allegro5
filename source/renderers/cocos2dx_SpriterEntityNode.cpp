#include "cocos2dx_SpriterEntityNode.h"

USING_NS_CC;

SpriterFileSystem::~SpriterFileSystem()
{
	clear();
}

bool SpriterFileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
	//CCSprite* sprite = CCSprite::createWithSpriteFrameName("filename");

	CCSprite* sprite = CCSprite::create(filename.c_str());

	if (sprite == NULL)
		return false;

	if(!images.insert(make_pair(make_pair(folderID, fileID), sprite)).second)
	{
		printf("SCML_SDL_gpu::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", filename.c_str(), folderID, fileID);		        
		return false;
	}
	else
	{
		sprite->retain();
	}

	return true;
}

/*! \brief Cleans up all memory used by the FileSystem to store images, resetting it to an empty state.
*/
void SpriterFileSystem::clear()
{
	for(map<pair<int,int>, CCSprite*>::iterator e = images.begin(); e != images.end(); e++)
	{
		e->second->release();
	}

	images.clear();
}

CCSprite* SpriterFileSystem::getImage(int folderID, int fileID) const
{
	map<pair<int,int>, CCSprite*>::const_iterator e = images.find(make_pair(folderID, fileID));
	if(e == images.end())
		return NULL;
	return e->second;
}


/*! \brief Gets the dimensions of an image
* \param folderID Integer folder ID
* \param fileID Integer file ID
* \return A pair consisting of the width and height of the image.  Returns (0,0) on error.
*/
std::pair<unsigned int, unsigned int> SpriterFileSystem::getImageDimensions(int folderID, int fileID) const
{		
	map<pair<int,int>, CCSprite*>::const_iterator e = images.find(make_pair(folderID, fileID));

	if(e == images.end())
		return make_pair(0,0);

	float w = e->second->boundingBox().size.width;
	float h = e->second->boundingBox().size.height;

	return make_pair(w, h);
} 

SpriterEntityNode::SpriterEntityNode(const char* scmlFileName):
mFileSystem(NULL)
{
	this->entity = 0;

	mData = new SCML::Data(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(scmlFileName));
	mFileSystem = new SpriterFileSystem();

	mFileSystem->load(mData);
	this->load(mData);		

	scheduleUpdate();
}   	

SpriterEntityNode::~SpriterEntityNode()
{
	if (mFileSystem)
	{
		delete mFileSystem;
		mFileSystem = NULL;
	}

	if (mData)
	{
		delete mData;
		mData = NULL;
	}
}

void SpriterEntityNode::convert_to_SCML_coords(float& x, float& y, float& angle)
{
	y = -y;
	angle = 360 - angle;
}

std::pair<unsigned int, unsigned int> SpriterEntityNode::getImageDimensions(int folderID, int fileID) const
{
	return mFileSystem->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void SpriterEntityNode::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{	
	angle = 360 - angle;

	CCSprite* img = mFileSystem->getImage(folderID, fileID);

	x+= this->getPositionX();
	y+= this->getPositionY();

	img->setPosition(ccp(x,y));
	img->setRotation(angle);
	img->setScaleX(scale_x);
	img->setScaleY(scale_y);

	img->visit();
}


void SpriterEntityNode::update(float dt) {
	SCML::Entity::update(dt * 1000.0f);
}

void SpriterEntityNode::draw(void)
{		  
	SCML::Entity::draw(this->getPositionX(), this->getPositionY(), this->getRotation(), this->getScale(), this->getScale());
}
