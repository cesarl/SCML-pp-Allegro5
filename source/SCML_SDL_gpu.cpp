#include "SCML_SDL_gpu.h"
#include "libgen.h"

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
    
    TweenInfo(SCML::Data* data, int entity, int animation, int lastKey, int time)
    {
        
    }
};

// TODO: Add scaling and rotation to this call.
void Entity::draw(SCML::Data* data, SCML_SDL_gpu::FileSystem* fs, GPU_Target* screen, float x, float y)
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
            GPU_BlitTransformX(img, NULL, screen, x + obj->x + img->w/2 - obj->pivot_x*img->w, y - obj->y - img->h/2 + obj->pivot_y*img->h, obj->pivot_x*img->w, -obj->pivot_y*img->h, -obj->angle, obj->scale_x, obj->scale_y);
    }
    
    // Go through each object_ref
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e = key->object_refs.begin(); e != key->object_refs.end(); e++)
    {
        // Dereference object_ref
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj = data->getTimelineObject(entity, current_animation.animation, e->second->timeline, e->second->key);
        if(obj != NULL)
        {
            GPU_Image* img = fs->getImage(obj->folder, obj->file);
            GPU_BlitTransformX(img, NULL, screen, x + obj->x + img->w/2 - obj->pivot_x*img->w, y - obj->y - img->h/2 + obj->pivot_y*img->h, obj->pivot_x*img->w, -obj->pivot_y*img->h, -obj->angle, obj->scale_x, obj->scale_y);
        }
    }
    
}







}

