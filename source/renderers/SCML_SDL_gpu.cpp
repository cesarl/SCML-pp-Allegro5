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

// This is for rotating untranslated points and offsetting them to a new origin.
void rotate_point(float& x, float& y, float angle, float origin_x, float origin_y)
{
    float s = sin(angle*M_PI/180);
    float c = cos(angle*M_PI/180);
    float xnew = (x * c) - (y * s);
    float ynew = (x * s) + (y * c);
    xnew += origin_x;
    ynew += origin_y;
    
    x = xnew;
    y = ynew;
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
            
            float pivot_x = lerp(obj1->pivot_x, obj2->pivot_x, t)*img->w;
            float pivot_y = -lerp(obj1->pivot_y, obj2->pivot_y, t)*img->h;
            
            // 'spin' is based on what you are coming from (key1) and has nothing to do with what you are going to (key2), I guess...
            float angle_i;
            if(t_key1->spin > 0 && obj2->angle - obj1->angle < 0.0f)
                angle_i = -lerp(obj1->angle, obj2->angle + 360, t);
            else if(t_key1->spin < 0 && obj2->angle - obj1->angle > 0.0f)
                angle_i = -lerp(obj1->angle, obj2->angle - 360, t);
            else
                angle_i = -lerp(obj1->angle, obj2->angle, t);
            float scale_x_i = lerp(obj1->scale_x, obj2->scale_x, t);
            float scale_y_i = lerp(obj1->scale_y, obj2->scale_y, t);
            
            // Move image to draw from and rotate about the pivot point (SDL_gpu draws images at their center point)
            float offset_x = (-pivot_x + img->w/2);
            float offset_y = (-pivot_y - img->h/2);
            
            // Relative object position
            float r_x = lerp(obj1->x, obj2->x, t);
            float r_y = -lerp(obj1->y, obj2->y, t);
            
            
            // Get parent bone hierarchy
            list<SCML::Data::Entity::Animation::Timeline::Key*> parents1;
            list<SCML::Data::Entity::Animation::Timeline::Key*> parents2;
            // Go through all the parents
            for(SCML::Data::Entity::Animation::Mainline::Key::Bone_Ref* bone_ref = data->getBoneRef(entity, animation, e1->second->key, e1->second->parent); bone_ref != NULL; bone_ref = data->getBoneRef(entity, animation, e1->second->key, bone_ref->parent))
            {
                SCML::Data::Entity::Animation::Timeline::Key* k = data->getTimelineKey(entity, animation, bone_ref->timeline, bone_ref->key);
                if(k == NULL || k->has_object)
                    break;
                
                parents1.push_front(k);
            }
            for(SCML::Data::Entity::Animation::Mainline::Key::Bone_Ref* bone_ref = data->getBoneRef(entity, animation, e2->second->key, e2->second->parent); bone_ref != NULL; bone_ref = data->getBoneRef(entity, animation, e2->second->key, bone_ref->parent))
            {
                SCML::Data::Entity::Animation::Timeline::Key* k = data->getTimelineKey(entity, animation, bone_ref->timeline, bone_ref->key);
                if(k == NULL || k->has_object)
                    break;
                
                parents2.push_front(k);
            }
            
            
            SDL_Color green = {0, 255, 0, 255};
            SDL_Color blue = {0, 0, 255, 255};
            
            float parent_x = 0.0f;
            float parent_y = 0.0f;
            float parent_angle = angle;
            float parent_scale_x = scale_x;
            float parent_scale_y = scale_y;
            list<SCML::Data::Entity::Animation::Timeline::Key*>::iterator b_key1 = parents1.begin();
            list<SCML::Data::Entity::Animation::Timeline::Key*>::iterator b_key2 = parents2.begin();
            
            while(b_key1 != parents1.end() && b_key2 != parents2.end())
            {
                SCML::Data::Entity::Animation::Timeline::Key::Bone* bone1 = &(*b_key1)->bone;
                SCML::Data::Entity::Animation::Timeline::Key::Bone* bone2 = &(*b_key2)->bone;
                
                // The transforms are definitely composed without matrices.  Evidence: Rotation does not affect scaling direction.
                // However, the positioning is affected by rotation.
                
                float bx = lerp(bone1->x, bone2->x, t) * parent_scale_x;
                float by = -lerp(bone1->y, bone2->y, t) * parent_scale_y;
                rotate_point(bx, by, parent_angle, parent_x, parent_y);
                
                float angle_b;
                if((*b_key1)->spin > 0 && bone2->angle - bone1->angle < 0.0f)
                    angle_b = lerp(bone1->angle, bone2->angle + 360, t);
                else if((*b_key1)->spin < 0 && bone2->angle - bone1->angle > 0.0f)
                    angle_b = lerp(bone1->angle, bone2->angle - 360, t);
                else
                    angle_b = lerp(bone1->angle, bone2->angle, t);
                
                parent_angle += 360-angle_b;
                parent_x = bx;
                parent_y = by;
                
                // debug draw bone
                /*GPU_Line(screen, x + parent_x, y + parent_y, x + parent_x + 50*cos(parent_angle*M_PI/180), y + parent_y + 50*sin(parent_angle*M_PI/180), green);
                GPU_Circle(screen, x + bx, y + by, 5, blue);*/
                parent_scale_x *= lerp(bone1->scale_x, bone2->scale_x, t);
                parent_scale_y *= lerp(bone1->scale_y, bone2->scale_y, t);
                
                b_key1++;
                b_key2++;
            }
            
            
            // Transform the sprite by the parent transform.
            r_x *= parent_scale_x;
            r_y *= parent_scale_y;
            rotate_point(r_x, r_y, parent_angle, parent_x, parent_y);
            
            angle_i += parent_angle;
            scale_x_i *= parent_scale_x;
            scale_y_i *= parent_scale_y;
            
            // debug draw transform from parent
            /*SDL_Color orange = {255, 168, 0, 255};
            GPU_Line(screen, x + parent_x, y + parent_y, x + r_x, y + r_y, orange);*/
            
            
            // Transform the sprite by its own transform now.
            
            float sprite_x = offset_x*scale_x;
            float sprite_y = offset_y*scale_y;
            rotate_point(sprite_x, sprite_y, angle_i, r_x, r_y);
            
            GPU_BlitTransform(img, NULL, screen, x + sprite_x, y + sprite_y, angle_i, scale_x_i, scale_y_i);
            
            // debug draw pivot
            /*SDL_Color red = {255, 0, 0, 255};
            GPU_CircleFilled(screen, x + r_x, y + r_y, 3, red);*/
            
        }
        
        e1++;
        e2++;
    }
}







}

