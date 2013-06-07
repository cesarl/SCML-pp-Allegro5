#include				"SCML_Allegro5.h"
#include				<cmath>
#include				<cstdlib>

using namespace				SCML_AL;

FileSystem::~FileSystem()
{
  this->clear();
}

bool					FileSystem::loadImageFile(int folderID,
								  int fileID,
								  const std::string& filename)
{
  ALLEGRO_BITMAP*			img;

  img = al_load_bitmap(filename.c_str());
  if(img == NULL)
    return false;

  if (!SCML_MAP_INSERT(this->images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
      printf("SCML_SFML::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
      al_destroy_bitmap(img);
      return false;
    }
  return true;
}

void					FileSystem::clear()
{

  typedef SCML_PAIR(int,int)		pair_type;

  SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, ALLEGRO_BITMAP*, item)
    {
      //delete item;
    }
  SCML_END_MAP_FOREACH_CONST;
  this->images.clear();
}

SCML_PAIR(unsigned int, unsigned int)	FileSystem::getImageDimensions(int folderID, int fileID) const
{
  ALLEGRO_BITMAP*			img;

  img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
  if(img == NULL)
    return SCML_MAKE_PAIR(0,0);
  return SCML_MAKE_PAIR(al_get_bitmap_width(img), al_get_bitmap_height(img));
}

ALLEGRO_BITMAP*				FileSystem::getImage(int folderID, int fileID) const
{
  return SCML_MAP_FIND(this->images, SCML_MAKE_PAIR(folderID, fileID));
}





Entity::Entity()
  : SCML::Entity()
{}

Entity::Entity(SCML::Data* data, int entity, int animation, int key)
  : SCML::Entity(data, entity, animation, key)
{}

FileSystem				*Entity::setFileSystem(FileSystem* fs)
{
  FileSystem				*old;

  old = this->file_system;
  this->file_system = fs;
  return old;
}

ALLEGRO_DISPLAY			*Entity::setScreen(ALLEGRO_DISPLAY *scr)
{
  ALLEGRO_DISPLAY			*old;

  old = this->screen;
  this->screen = scr;
  return old;
}


void					Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
  y = -y;
  angle = 360 - angle;
}

SCML_PAIR(unsigned int, unsigned int)	Entity::getImageDimensions(int folderID, int fileID) const
{
  return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void					Entity::draw_internal(int folderID,
							      int fileID,
							      float x,
							      float y,
							      float angle,
							      float scale_x,
							      float scale_y)
{
  y = -y;
  angle = 360 - angle;

  ALLEGRO_BITMAP			*img;

  img = file_system->getImage(folderID, fileID);
  if(img == NULL)
    return;

  // sprite.setOrigin(img->getSize().x/2, img->getSize().y/2);
  // sprite.setScale(scale_x, scale_y);
  // sprite.setRotation(angle);
  // sprite.setPosition(x, y);
  al_draw_tinted_scaled_rotated_bitmap(img,
				       al_map_rgb(255,255,255),
				       al_get_bitmap_width(img) / 2, al_get_bitmap_height(img) / 2,
				       x, y,
				       scale_x, scale_y,
				       angle * ALLEGRO_PI / 180, 0);
  // screen->draw(sprite);
}
