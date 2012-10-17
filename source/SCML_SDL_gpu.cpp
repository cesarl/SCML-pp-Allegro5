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

void Entity::update(SCML::Data* data, int dt_ms)
{
    if(data == NULL || entity < 0 || current_animation.animation < 0 || current_animation.key < 0)
        return;
    
    
    SCML::Data::Entity::Animation* animation = data->getAnimation(entity, current_animation.animation);
    if(animation == NULL)
        return;
    
    int numKeys = animation->mainline.keys.size();
    int nextTime = 0;
    if(current_animation.key+1 >= numKeys)
    {
        // No next key, so use end of animation time
        nextTime = animation->length;
    }
    else
    {
        SCML::Data::Entity::Animation::Mainline::Key* nextKey = data->getKey(entity, current_animation.animation, current_animation.key+1);
        if(nextKey != NULL)
        {
            nextTime = nextKey->time;
        }
    }
    
    current_animation.time += dt_ms;
    if(current_animation.time >= nextTime)
    {
        // Advance to next key
        if(animation->looping == "true")
        {
            // If we've reached the end of the keys, loop.
            if(current_animation.key+1 >= int(animation->mainline.keys.size()))
            {
                // Set key to the loop_to key.
                current_animation.key = animation->loop_to;
                // Get the starting time from the new key.
                SCML::Data::Entity::Animation::Mainline::Key* key = data->getKey(entity, current_animation.animation, current_animation.key);
                if(key != NULL)
                {
                    current_animation.time = key->time;
                }
            }
            else
                current_animation.key++;
        }
        else if(animation->looping == "ping_pong")
        {
            // TODO: Implement ping_pong animation
        }
        else  // assume "false"
        {
            // If we've haven't reached the end of the keys, go to the next one.
            if(current_animation.key+1 < int(animation->mainline.keys.size()))
                current_animation.key++;
        }
    }
}

void Entity::draw(SCML::Data* data, SCML_SDL_gpu::FileSystem* fs, GPU_Target* screen, float x, float y)
{
    // Get key
    SCML::Data::Entity::Animation::Mainline::Key* key = data->getKey(entity, current_animation.animation, current_animation.key);
    if(key == NULL)
        return;
    
    // Go through each temp object
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object*>::iterator e = key->objects.begin(); e != key->objects.end(); e++)
    {
        GPU_Image* img = fs->getImage(e->second->folder, e->second->file);
        GPU_Blit(img, NULL, screen, x, y);
    }
    
    // Go through each object_ref
    for(map<int, SCML::Data::Entity::Animation::Mainline::Key::Object_Ref*>::iterator e = key->object_refs.begin(); e != key->object_refs.end(); e++)
    {
        // Dereference object_ref
        SCML::Data::Entity::Animation::Timeline::Key::Object* obj = data->getTimelineObject(entity, current_animation.animation, e->second->timeline, e->second->key);
        if(obj != NULL)
        {
            GPU_Image* img = fs->getImage(obj->folder, obj->file);
            GPU_Blit(img, NULL, screen, x, y);
        }
    }
    
}







}

