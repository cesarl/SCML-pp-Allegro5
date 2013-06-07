#ifndef					_ALLEGRO5_RENDERER_H__
# define				_ALLEGRO5_RENDERER_H__

#include				<allegro5/allegro.h>
#include				<allegro5/allegro_image.h>
#include				"SCMLpp.h"

/*! \brief Namespace for Allegro5 renderer
*/
namespace SCML_AL
{
  class FileSystem : public SCML::FileSystem
  {
  public:
    SCML_MAP(SCML_PAIR(int, int), ALLEGRO_BITMAP*) images;
    virtual ~FileSystem();
    virtual bool			loadImageFile(int folderID, int fileID, const std::string& filename);
    virtual void			clear();
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    ALLEGRO_BITMAP			*getImage(int folderID, int fileID) const;
};

  class Entity : public SCML::Entity
  {
  public:
    FileSystem				*file_system;
    ALLEGRO_DISPLAY			*screen;

    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);

    FileSystem				*setFileSystem(FileSystem* fs);
    ALLEGRO_DISPLAY			*setScreen(ALLEGRO_DISPLAY *scr);

    virtual void			convert_to_SCML_coords(float& x,
								 float& y,
								 float& angle);
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID,
								       int fileID) const;
    virtual void			draw_internal(int folderID,
							int fileID,
							float x,
							float y,
							float angle,
							float scale_x,
							float scale_y);
  };
}



#endif					// _ALLEGRO5_RENDERER_H__
