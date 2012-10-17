#include "SCML_SDL_gpu.h"



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

void Entity::draw(SCML::Data* data, GPU_Target* screen, float x, float y)
{
    printf("Entity: %d, Animation: %d, Key: %d\n", entity, current_animation.animation, current_animation.key);
}

