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



class Vector3
{
    public:
    float data[3];
    
    Vector3();
    Vector3(float a, float b, float c);
    
    float operator[](int index) const;
};

Vector3::Vector3()
{
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
}

Vector3::Vector3(float a, float b, float c)
{
    data[0] = a;
    data[1] = b;
    data[2] = c;
}

float Vector3::operator[](int index) const
{
    return data[index];
}

class Matrix3x3
{
    public:
    
    // Column-major: index = row + column*numColumns
    float data[9];
    
    Matrix3x3();
    Matrix3x3(float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33);
    Matrix3x3(const Matrix3x3& matrix);
    
    static const Matrix3x3& getIdentity();
    
    float get(int row, int column) const;
    
    // Post-multiply operations
    void scale(float scale_x, float scale_y);
    void translate(float dx, float dy);
    void rotate(float radians);
    // Pre-multiply operations
    void prescale(float scale_x, float scale_y);
    void pretranslate(float dx, float dy);
    void prerotate(float radians);
};

Matrix3x3::Matrix3x3()
{
    for(int i = 0; i < 9; i++)
        data[i] = 0;
}

Matrix3x3::Matrix3x3(float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33)
{
    data[0] = a11;
    data[1] = a21;
    data[2] = a31;
    data[3] = a12;
    data[4] = a22;
    data[5] = a32;
    data[6] = a13;
    data[7] = a23;
    data[8] = a33;
}

Matrix3x3::Matrix3x3(const Matrix3x3& matrix)
{
    memcpy(data, matrix.data, sizeof(float)*9);
}


const Matrix3x3& Matrix3x3::getIdentity()
{
    static const Matrix3x3 identity(1,0,0,0,1,0,0,0,1);
    return identity;
}

float Matrix3x3::get(int row, int column) const
{
    return data[row + column*3];
}


Matrix3x3 multiply(const Matrix3x3& A, const Matrix3x3& B)
{
    return Matrix3x3(A.get(0,0)*B.get(0,0) + A.get(0,1)*B.get(1,0) + A.get(0,2)*B.get(2,0),
                     A.get(0,0)*B.get(0,1) + A.get(0,1)*B.get(1,1) + A.get(0,2)*B.get(2,1),
                     A.get(0,0)*B.get(0,2) + A.get(0,1)*B.get(1,2) + A.get(0,2)*B.get(2,2),
                     
                     A.get(1,0)*B.get(0,0) + A.get(1,1)*B.get(1,0) + A.get(1,2)*B.get(2,0),
                     A.get(1,0)*B.get(0,1) + A.get(1,1)*B.get(1,1) + A.get(1,2)*B.get(2,1),
                     A.get(1,0)*B.get(0,2) + A.get(1,1)*B.get(1,2) + A.get(1,2)*B.get(2,2),
                     
                     A.get(2,0)*B.get(0,0) + A.get(2,1)*B.get(1,0) + A.get(2,2)*B.get(2,0),
                     A.get(2,0)*B.get(0,1) + A.get(2,1)*B.get(1,1) + A.get(2,2)*B.get(2,1),
                     A.get(2,0)*B.get(0,2) + A.get(2,1)*B.get(1,2) + A.get(2,2)*B.get(2,2)
                     );
}

// Post-multiply
void Matrix3x3::scale(float scale_x, float scale_y)
{
	data[0] *= scale_x;
	data[1] *= scale_x;
	data[2] *= scale_x;
	data[3] *= scale_y;
	data[4] *= scale_y;
	data[5] *= scale_y;
}

void Matrix3x3::translate(float dx, float dy)
{
	data[6] += data[0]*dx + data[3]*dy;
	data[7] += data[1]*dx + data[4]*dy;
	data[8] += data[2]*dx + data[5]*dy;
}

void Matrix3x3::rotate(float radians)
{
	float cosT = cos(radians);
	float sinT = sin(radians);
	float a = data[0];
	float b = data[3];
	float d = data[1];
	float e = data[4];
	float g = data[2];
	float h = data[5];
	data[0] = a*cosT+b*sinT;
	data[1] = d*cosT+e*sinT;
	data[2] = g*cosT+h*sinT;
	data[3] = -a*sinT+b*cosT;
	data[4] = -d*sinT+e*cosT;
	data[5] = -g*sinT+h*cosT;
}

// Pre-multiply
void Matrix3x3::prescale(float scale_x, float scale_y)
{
	data[0] *= scale_x;
	data[1] *= scale_y;
	data[3] *= scale_x;
	data[4] *= scale_y;
	data[6] *= scale_x;
	data[7] *= scale_y;
}

void Matrix3x3::pretranslate(float dx, float dy)
{
	float g = data[2];
	float h = data[5];
	float i = data[8];
	data[0] += g*dx;
	data[1] += g*dy;
	data[3] += h*dx;
	data[4] += h*dy;
	data[6] += i*dx;
	data[7] += i*dy;
}

void Matrix3x3::prerotate(float radians)
{
	float cosT = cos(radians);
	float sinT = sin(radians);
	float a = data[0];
	float b = data[3];
	float c = data[6];
	float d = data[1];
	float e = data[4];
	float f = data[7];
	data[0] = a*cosT-d*sinT;
	data[1] = a*sinT+d*cosT;
	data[3] = b*cosT-e*sinT;
	data[4] = b*sinT+e*cosT;
	data[6] = c*cosT-f*sinT;
	data[7] = c*sinT+f*cosT;
}

/*
a11 a12 a13    v1           a11*v1 + a12*v2 + a13*v3
a21 a22 a23    v2     =     a21*v1 + a22*v2 + a23*v3
a31 a32 a33    v3           a31*v1 + a32*v2 + a33*v3
*/
Vector3 multiply(const Matrix3x3& A, const Vector3& v)
{
    return Vector3(A.get(0,0)*v[0] + A.get(0,1)*v[1] + A.get(0,2)*v[2],
                   A.get(1,0)*v[0] + A.get(1,1)*v[1] + A.get(1,2)*v[2],
                   A.get(2,0)*v[0] + A.get(2,1)*v[1] + A.get(2,2)*v[2]
                   );
}

inline float lerp(float a, float b, float t)
{
    return a + (b-a)*t;
}

void rotate_point(float& x, float& y, float angle, float origin_x, float origin_y)
{
    float s = sin(angle*M_PI/180);
    float c = cos(angle*M_PI/180);
    //float xnew = ((x-origin_x) * c) - ((y-origin_y) * s);
    //float ynew = ((x-origin_x) * s) + ((y-origin_y) * c);
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
            float parent_angle = 0.0f;
            float parent_scale_x = 1.0f;
            float parent_scale_y = 1.0f;
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
                /*GPU_Line(screen, x + parent_x, y + parent_y, x + parent_x + 50*cos(parent_angle*M_PI/180), y + parent_y + 50*sin(parent_angle*M_PI/180), green);
                GPU_Circle(screen, x + bx, y + by, 5, blue);
                printf("bone bxy: %.0f, %.0f\n", bx, by);
                printf("bone xy: %.0f, %.0f\n", bone1->x, -bone1->y);
                printf("bone angle: %.0f\n", bone1->angle);*/
                parent_scale_x *= lerp(bone1->scale_x, bone2->scale_x, t);
                parent_scale_y *= lerp(bone1->scale_y, bone2->scale_y, t);
                
                b_key1++;
                b_key2++;
            }
            
            
            //printf("parent xy: %.0f, %.0f\n", parent_x, parent_y);
            //printf("parent angle: %.0f\n", parent_angle);
            r_x *= parent_scale_x;
            r_y *= parent_scale_y;
            angle_i += parent_angle;
            rotate_point(r_x, r_y, parent_angle, parent_x, parent_y);
            
            //SDL_Color orange = {255, 168, 0, 255};
            //GPU_Line(screen, x + parent_x, y + parent_y, x + r_x, y + r_y, orange);
            
            scale_x_i *= parent_scale_x;
            scale_y_i *= parent_scale_y;
            
            //GPU_BlitTransform(img, NULL, screen, x + r_x, y + r_y, angle + angle_i, scale_x*scale_x_i, scale_y*scale_y_i);
            GPU_BlitTransformX(img, NULL, screen, x + offset_x + r_x, y + offset_y + r_y, -offset_x, -offset_y, angle_i + angle, scale_x_i*scale_x, scale_y_i*scale_y);
            
            // debug draw pivot
            /*SDL_Color red = {255, 0, 0, 255};
            GPU_CircleFilled(screen, x + r_x, y + r_y, 3, red);
            printf("xy: %.0f, %.0f\n", r_x, r_y);*/
            
        }
        
        e1++;
        e2++;
    }
}







}

