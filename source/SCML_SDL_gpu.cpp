#include "SCML_SDL_gpu.h"
#include "libgen.h"
#include <cmath>

using namespace std;

namespace SCML_SDL_gpu
{


void FileSystem::load(SCML::Data* data)
{
    if(data == NULL)
        return;
    
    char buf[1000];
    snprintf(buf, 1000, "%s", data->name.c_str());
    string basedir = dirname(buf);
    basedir += '/';
    
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
    : entity(-1), current_animation()
{}

Entity::Entity(int entity, int animation, int key)
    : entity(entity), current_animation(animation, key)
{}

void Entity::startAnimation(int animation)
{
    current_animation.animation = animation;
    current_animation.key = 0;
    current_animation.time = 0;
}

Entity::Animation::Animation()
    : animation(-1), key(-1), time(0)
{}

Entity::Animation::Animation(int animation, int key)
    : animation(animation), key(key), time(0)
{}

// Gets the next key index according to the animation's looping setting.
int getNextKey(SCML::Data* data, int entity, int animation, int lastKey)
{
    if(data == NULL || entity < 0 || animation < 0 || lastKey < 0)
        return -1;
    
    
    SCML::Data::Entity::Animation* animationP = data->getAnimation(entity, animation);
    if(animationP == NULL)
        return -2;
    
    if(animationP->looping == "true")
    {
        // If we've reached the end of the keys, loop.
        if(lastKey+1 >= int(animationP->mainline.keys.size()))
            return animationP->loop_to;
        else
            return lastKey+1;
    }
    else if(animationP->looping == "ping_pong")
    {
        // TODO: Implement ping_pong animation
        return -3;
    }
    else  // assume "false"
    {
        // If we've haven't reached the end of the keys, return the next one.
        if(lastKey+1 < int(animationP->mainline.keys.size()))
            return lastKey+1;
        else // if we have reached the end, stick to this key
            return lastKey;
    }
}

void Entity::update(SCML::Data* data, int dt_ms)
{
    if(data == NULL || entity < 0 || current_animation.animation < 0 || current_animation.key < 0)
        return;
    
    
    SCML::Data::Entity::Animation* animation = data->getAnimation(entity, current_animation.animation);
    if(animation == NULL)
        return;
    
    int nextKey = getNextKey(data, entity, current_animation.animation, current_animation.key);
    int nextTime = 0;
    if(nextKey < current_animation.key)
    {
        // Next key is not after this one, so use end of animation time
        nextTime = animation->length;
    }
    else
    {
        // Get nextTime from the nextKey
        SCML::Data::Entity::Animation::Mainline::Key* nextKeyP = data->getKey(entity, current_animation.animation, nextKey);
        if(nextKeyP != NULL)
        {
            nextTime = nextKeyP->time;
        }
    }
    
    current_animation.time += dt_ms;
    if(current_animation.time >= nextTime)
    {
        int overshot = current_animation.time - nextTime;
        
        // Advance to next key
        current_animation.key = nextKey;
        
        // Get the starting time from the new key.
        SCML::Data::Entity::Animation::Mainline::Key* key = data->getKey(entity, current_animation.animation, current_animation.key);
        if(key != NULL)
        {
            current_animation.time = key->time + overshot;
        }
    }
}


class TweenInfo
{
    public:
    
    int entity;
    int animation;
    int lastKey;
    int nextKey;
    float t;
    SCML::Data::Entity::Animation::Mainline::Key* key1;
    SCML::Data::Entity::Animation::Mainline::Key* key2;
    
    TweenInfo(SCML::Data* data, int entity, int animation, int lastKey, int time)
        : entity(entity), animation(animation), lastKey(lastKey), t(0.0f), key1(NULL), key2(NULL)
    {
        nextKey = getNextKey(data, entity, animation, lastKey);
        
        // Get key
        key1 = data->getKey(entity, animation, lastKey);
        if(key1 == NULL)
            return;
        
        // Get next key
        key2 = data->getKey(entity, animation, nextKey);
        if(key2 == NULL)
            return;
        
        if(key2->time == key1->time)
            return;
        
        t = (time - key1->time)/float(key2->time - key1->time);  // wrong!  Use the timeline key.
    }
};

inline float lerp(float a, float b, float t)
{
    return a + (b-a)*t;
}

// TODO: Add scaling and rotation to this call.
void Entity::draw(SCML::Data* data, SCML_SDL_gpu::FileSystem* fs, GPU_Target* screen, float x, float y, float angle, float scale_x, float scale_y)
{
    // Get key
    SCML::Data::Entity::Animation::Mainline::Key* key = data->getKey(entity, current_animation.animation, current_animation.key);
    if(key == NULL)
        return;
    
    // TODO: Follow z_index for drawing order
    
    // Go through each temp object
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object*>::iterator e = key->objects.begin(); e != key->objects.end(); e++)
    {
        SCML::Data::Entity::Animation::Mainline::Key::Object* obj = e->second;
        
        GPU_Image* img = fs->getImage(obj->folder, obj->file);
        
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
    TweenInfo tween_info(data, entity, current_animation.animation, current_animation.key, current_animation.time);
    SCML::Data::Entity::Animation::Mainline::Key* key1 = key;
    SCML::Data::Entity::Animation::Mainline::Key* key2 = data->getKey(entity, current_animation.animation, tween_info.nextKey);
    
    map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e1 = key1->object_refs.begin();
    map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e2 = key2->object_refs.begin();
    while(e1 != key1->object_refs.end() && e2 != key2->object_refs.end())
    {
        // Dereference object_refs
        SCML::Data::Entity::Animation::Timeline::Key* t_key1 = data->getTimelineKey(entity, current_animation.animation, e1->second->timeline, e1->second->key);
        SCML::Data::Entity::Animation::Timeline::Key* t_key2 = data->getTimelineKey(entity, current_animation.animation, e2->second->timeline, e2->second->key);
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj1 = data->getTimelineObject(entity, current_animation.animation, e1->second->timeline, e1->second->key);
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj2 = data->getTimelineObject(entity, current_animation.animation, e2->second->timeline, e2->second->key);
        if(obj2 == NULL)
            obj2 = obj1;
        if(t_key1 != NULL && t_key2 != NULL && obj1 != NULL && obj2 != NULL)
        {
            // No image tweening
            GPU_Image* img = fs->getImage(obj1->folder, obj1->file);
            
            float t = 0.0f;
            if(t_key2->time != t_key1->time)
                t = (current_animation.time - t_key1->time)/float(t_key2->time - t_key1->time);
            
            float pivot_x = lerp(obj1->pivot_x, obj2->pivot_x, t);
            float pivot_y = lerp(obj1->pivot_y, obj2->pivot_y, t);
            
            // Is 'spin' what you are coming from (key1) or what you are going to (key2)?
            float angle_i;
            if(t_key1->spin > 0 && obj2->angle - obj1->angle < 0.0f)
                angle_i = lerp(obj1->angle, obj2->angle + 360, t);
            else if(t_key1->spin < 0 && obj2->angle - obj1->angle > 0.0f)
                angle_i = lerp(obj1->angle, obj2->angle - 360, t);
            else
                angle_i = lerp(obj1->angle, obj2->angle, t);
            float scale_x_i = lerp(obj1->scale_x, obj2->scale_x, t);
            float scale_y_i = lerp(obj1->scale_y, obj2->scale_y, t);
            
            float ax = lerp(obj1->x, obj2->x, t);
            float bx = img->w/2;
            float cx = -pivot_x*img->w;
            
            float ay = lerp(-obj1->y, -obj2->y, t);
            float by = -img->h/2;
            float cy = pivot_y*img->h;
            
            float xx = (ax + bx)*scale_x;
            float yy = (ay + by)*scale_y;
            float c = cos(angle*M_PI/180.0f);
            float s = sin(angle*M_PI/180.0f);
            GPU_BlitTransformX(img, NULL, screen, x + cx*scale_x + xx*c - yy*s, y + cy*scale_y + xx*s + yy*c, pivot_x*img->w*scale_x, -pivot_y*img->h*scale_y, -angle_i + angle, scale_x_i*scale_x, scale_y_i*scale_y);
        }
        
        e1++;
        e2++;
    }
    
    /*// Go through each object_ref
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e = key->object_refs.begin(); e != key->object_refs.end(); e++)
    {
        // Dereference object_ref
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj = data->getTimelineObject(entity, current_animation.animation, e->second->timeline, e->second->key);
        if(obj != NULL)
        {
            GPU_Image* img = fs->getImage(obj->folder, obj->file);
            
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
    }*/
}







}

