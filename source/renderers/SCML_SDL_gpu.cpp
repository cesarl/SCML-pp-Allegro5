#include "SCML_SDL_gpu.h"
#include "libgen.h"
#include <cstdlib>
#include <cmath>
#include <climits>

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

using namespace std;

namespace SCML_SDL_gpu
{

static bool pathIsAbsolute(const std::string& path)
{
    #ifdef WIN32
    if(path.size() < 3)
        return false;
    return (isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/'));
    #else
    if(path.size() < 1)
        return false;
    return (path[0] == '/');
    #endif
    return false;
}

FileSystem::~FileSystem()
{
    clear();
}

void FileSystem::load(SCML::Data* data)
{
    if(data == NULL || data->name.size() == 0)
        return;
    
    string basedir;
    if(!pathIsAbsolute(data->name))
    {
        // Create a relative directory name for the path's base
        char buf[PATH_MAX];
        snprintf(buf, PATH_MAX, "%s", data->name.c_str());
        basedir = dirname(buf);
        if(basedir.size() > 0 && basedir[basedir.size()-1] != '/')
            basedir += '/';
    }
    
    for(map<int, SCML::Data::Folder*>::iterator e = data->folders.begin(); e != data->folders.end(); e++)
    {
        for(map<int, SCML::Data::Folder::File*>::iterator f = e->second->files.begin(); f != e->second->files.end(); f++)
        {
            if(f->second->type == "image")
            {
                printf("Loading \"%s\"\n", (basedir + f->second->name).c_str());
                GPU_Image* img = GPU_LoadImage((basedir + f->second->name).c_str());
                if(img != NULL)
                {
                    if(!images.insert(make_pair(make_pair(e->first, f->first), img)).second)
                    {
                        printf("SCML_SDL_gpu::FileSystem failed to load an image: duplicate folder/file id (%d/%d)\n", e->first, f->first);
                        GPU_FreeImage(img);
                    }
                }
            }
        }
    }
}

void FileSystem::clear()
{
    for(map<pair<int,int>, GPU_Image*>::iterator e = images.begin(); e != images.end(); e++)
    {
        GPU_FreeImage(e->second);
    }
    images.clear();
}

GPU_Image* FileSystem::getImage(int folder, int file) const
{
    map<pair<int,int>, GPU_Image*>::const_iterator e = images.find(make_pair(folder, file));
    if(e == images.end())
        return NULL;
    return e->second;
}






    
Entity::Entity()
    : SCML::Entity()
{}

Entity::Entity(int entity, int animation, int key)
    : SCML::Entity(entity, animation, key)
{}

FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

GPU_Target* Entity::setScreen(GPU_Target* scr)
{
    GPU_Target* old = screen;
    screen = scr;
    return old;
}




inline float lerp(float a, float b, float t)
{
    return a + (b-a)*t;
}


void Entity::draw(SCML::Data* data, float x, float y, float angle, float scale_x, float scale_y)
{
    // Get key
    SCML::Data::Entity::Animation::Mainline::Key* key_ptr = data->getKey(entity, animation, key);
    if(key_ptr == NULL)
        return;
    
    // TODO: Follow z_index for drawing order
    
    // Go through each temp object
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object*>::iterator e = key_ptr->objects.begin(); e != key_ptr->objects.end(); e++)
    {
        SCML::Data::Entity::Animation::Mainline::Key::Object* obj = e->second;
        
        GPU_Image* img = file_system->getImage(obj->folder, obj->file);
        
        float ax = obj->x;
        float bx = img->w/2;
        float cx = -obj->pivot_x*img->w;
        
        float ay = -obj->y;
        float by = -img->h/2;
        float cy = obj->pivot_y*img->h;
        
        float xx = (ax + bx)*scale_x;
        float yy = (ay + by)*scale_y;
        float c = cos(angle*M_PI/180.0f);
        float s = sin(angle*M_PI/180.0f);
        GPU_BlitTransformX(img, NULL, screen, x + cx*scale_x + xx*c - yy*s, y + cy*scale_y + xx*s + yy*c, obj->pivot_x*img->w*scale_x, -obj->pivot_y*img->h*scale_y, -obj->angle + angle, obj->scale_x*scale_x, obj->scale_y*scale_y);
    }
    
    // Assuming that each object in a timeline's key corresponds to the object in every other timeline at the same sequential position...
    SCML::Data::Entity::Animation::Mainline::Key* key1 = key_ptr;
    SCML::Data::Entity::Animation::Mainline::Key* key2 = data->getKey(entity, animation, data->getNextKeyID(entity, animation, key));
    
    map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e1 = key1->object_refs.begin();
    map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e2 = key2->object_refs.begin();
    while(e1 != key1->object_refs.end() && e2 != key2->object_refs.end())
    {
        // Dereference object_refs
        SCML::Data::Entity::Animation::Timeline::Key* t_key1 = data->getTimelineKey(entity, animation, e1->second->timeline, e1->second->key);
        SCML::Data::Entity::Animation::Timeline::Key* t_key2 = data->getTimelineKey(entity, animation, e2->second->timeline, e2->second->key);
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj1 = data->getTimelineObject(entity, animation, e1->second->timeline, e1->second->key);
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj2 = data->getTimelineObject(entity, animation, e2->second->timeline, e2->second->key);
        if(obj2 == NULL)
            obj2 = obj1;
        if(t_key1 != NULL && t_key2 != NULL && obj1 != NULL && obj2 != NULL)
        {
            // No image tweening
            GPU_Image* img = file_system->getImage(obj1->folder, obj1->file);
            
            float t = 0.0f;
            if(t_key2->time != t_key1->time)
                t = (time - t_key1->time)/float(t_key2->time - t_key1->time);
            
            float pivot_x = lerp(obj1->pivot_x, obj2->pivot_x, t);
            float pivot_y = lerp(obj1->pivot_y, obj2->pivot_y, t);
            
            // 'spin' is based on what you are coming from (key1) and has nothing to do with what you are going to (key2), I guess...
            float angle_i;
            if(t_key1->spin > 0 && obj2->angle - obj1->angle < 0.0f)
                angle_i = lerp(obj1->angle, obj2->angle + 360, t);
            else if(t_key1->spin < 0 && obj2->angle - obj1->angle > 0.0f)
                angle_i = lerp(obj1->angle, obj2->angle - 360, t);
            else
                angle_i = lerp(obj1->angle, obj2->angle, t);
            float scale_x_i = lerp(obj1->scale_x, obj2->scale_x, t);
            float scale_y_i = lerp(obj1->scale_y, obj2->scale_y, t);
            
            // Move image to draw from and rotate about the pivot point (SDL_gpu draws images at their center point)
            float offset_x = (-pivot_x*img->w + img->w/2)*scale_x;
            float offset_y = (pivot_y*img->h - img->h/2)*scale_y;
            
            // Rotation of the relative object position
            float r_x = lerp(obj1->x, obj2->x, t)*scale_x;
            float r_y = lerp(-obj1->y, -obj2->y, t)*scale_y;
            float c = cos(angle*M_PI/180.0f);
            float s = sin(angle*M_PI/180.0f);
            
            GPU_BlitTransformX(img, NULL, screen, x + offset_x + (r_x*c - r_y*s), y + offset_y + (r_x*s + r_y*c), -offset_x, -offset_y, -angle_i + angle, scale_x_i*scale_x, scale_y_i*scale_y);
        
            // debug draw pivot
            //SDL_Color red = {255, 0, 0, 255};
            //GPU_CircleFilled(screen, x + offset_x + (r_x*c - r_y*s), y + offset_y + (r_x*s + r_y*c), 3, red);
        }
        
        e1++;
        e2++;
    }
}







}

