#ifndef __SpriterEntityNode_H__
#define __SpriterEntityNode_H__

#include "cocos2d.h"
#include "SCMLpp.h"
#include <cstdlib>
#include <cmath>

using namespace std;

class SpriterFileSystem : public SCML::FileSystem
{
public:

	// Folder, File
	std::map<std::pair<int, int>, cocos2d::CCSprite*> images;

	virtual ~SpriterFileSystem();

	virtual bool loadImageFile(int folderID, int fileID, const std::string& filename);

	virtual void clear();

	cocos2d::CCSprite* getImage(int folderID, int fileID) const;

	virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const;
};

class SpriterEntityNode : public cocos2d::CCNode, public SCML::Entity
{

private:
	
	SpriterFileSystem* mFileSystem;
	SCML::Data* mData;

	SpriterEntityNode(const char* scmlFileName);

public:

	static SpriterEntityNode* create(const char* scmlFileName)
	{
		SpriterEntityNode* spriterEntityNode = new SpriterEntityNode(scmlFileName);
		spriterEntityNode->autorelease();
		return spriterEntityNode;
	}

	virtual ~SpriterEntityNode();

	virtual void convert_to_SCML_coords(float& x, float& y, float& angle);

	virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const;

	// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
	virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y);

	virtual void update(float dt);

	virtual void draw(void);

};

#endif // __SpriterEntityNode_H__
