#ifndef _SCMLPP_H__
#define _SCMLPP_H__

#include <string>
#include <map>
#include <list>
#include "tinyxml.h"

namespace SCML
{

class Data
{
    public:
    
    std::string name;
    std::string scml_version;
    std::string generator;
    std::string generator_version;
    bool pixel_art_mode;
    
    class Folder;
    std::map<int, Folder*> folders;
    class Atlas;
    std::map<int, Atlas*> atlases;
    class Entity;
    std::map<int, Entity*> entities;
    class Character_Map;
    std::map<int, Character_Map*> character_maps;
    
    Data();
    Data(const std::string& file);
    Data(TiXmlElement* elem);
    Data(const Data& copy);
    Data& operator=(const Data& copy);
    ~Data();
    
    bool load(const std::string& file);
    bool load(TiXmlElement* elem);
    Data& clone(const Data& copy, bool skip_base = false);
    void log(int recursive_depth = 0) const;
    void clear();
    
    

    class Meta_Data
    {
        public:
        
        class Variable;
        std::map<std::string, Variable*> variables;
        class Tag;
        std::map<std::string, Tag*> tags;
        
        Meta_Data();
        Meta_Data(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        class Variable
        {
            public:
            
            std::string name;
            std::string type;
            
            std::string value_string;
            int value_int;
            float value_float;
            
            Variable();
            Variable(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
        };
        
        class Tag
        {
            public:
            
            std::string name;
            
            Tag();
            Tag(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
        };
    };
    

    class Meta_Data_Tweenable
    {
        public:
        
        class Variable;
        std::map<std::string, Variable*> variables;
        class Tag;
        std::map<std::string, Tag*> tags;
        
        Meta_Data_Tweenable();
        Meta_Data_Tweenable(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        class Variable
        {
            public:
            
            std::string name;
            std::string type;
            std::string value_string;
            int value_int;
            float value_float;
            std::string curve_type;
            float c1;
            float c2;
            
            Variable();
            Variable(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
            
        };
        
        class Tag
        {
            public:
            
            std::string name;
            
            Tag();
            Tag(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
        };
    };
    
    Meta_Data* meta_data;

    class Folder
    {
        public:
        
        int id;
        std::string name;
        
        class File;
        std::map<int, File*> files;
        
        Folder();
        Folder(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        class File
        {
            public:
            
            std::string type;
            int id;
            std::string name;
            float pivot_x;
            float pivot_y;
            int width;
            int height;
            int atlas_x;
            int atlas_y;
            int offset_x;
            int offset_y;
            int original_width;
            int original_height;
            
            File();
            File(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
            
        };
    };

    class Atlas
    {
        public:
        int id;
        std::string data_path;
        std::string image_path;
        
        class Folder;
        std::map<int, Folder*> folders;
        
        Atlas();
        Atlas(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        class Folder
        {
            public:
            
            int id;
            std::string name;
        
            class Image;
            std::map<int, Image*> images;
        
            Folder();
            Folder(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
            
            
            class Image
            {
                public:
                
                int id;
                std::string full_path;
        
                Image();
                Image(TiXmlElement* elem);
                
                bool load(TiXmlElement* elem);
                void log(int recursive_depth = 0) const;
                void clear();
                
            };
        };
    };

    class Entity
    {
        public:
            
        int id;
        std::string name;
    
        class Animation;
        std::map<int, Animation*> animations;
    
        Entity();
        Entity(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        Meta_Data* meta_data;

        class Animation
        {
            public:
            
            int id;
            std::string name;
            int length;
            std::string looping;
            int loop_to;
            
            Meta_Data* meta_data;
            
            // More to follow...
            class Mainline
            {
                public:
                
                Mainline();
                Mainline(TiXmlElement* elem);
                
                bool load(TiXmlElement* elem);
                void log(int recursive_depth = 0) const;
                void clear();
                
                class Key;
                std::map<int, Key*> keys;
                
                class Key
                {
                    public:
                    
                    int id;
                    int time;
                    Meta_Data* meta_data;
                    
                    Key();
                    Key(TiXmlElement* elem);
                    
                    bool load(TiXmlElement* elem);
                    void log(int recursive_depth = 0) const;
                    void clear();
                    
                    
                    class Object;
                    class Object_Ref;
                    
                    class Object_Container
                    {
                        public:
                        Object* object;
                        Object_Ref* object_ref;
                        
                        Object_Container(Object* object)
                            : object(object), object_ref(NULL)
                        {}
                        Object_Container(Object_Ref* object_ref)
                            : object(NULL), object_ref(object_ref)
                        {}
                        
                        bool hasObject() const
                        {
                            return (object != NULL);
                        }
                        bool hasObject_Ref() const
                        {
                            return (object_ref != NULL);
                        }
                    };
                    
                    std::map<int, Object_Container> objects;
                    
                    
                    class Bone;
                    class Bone_Ref;
                    
                    class Bone_Container
                    {
                        public:
                        Bone* bone;
                        Bone_Ref* bone_ref;
                        
                        Bone_Container(Bone* bone)
                            : bone(bone), bone_ref(NULL)
                        {}
                        Bone_Container(Bone_Ref* bone_ref)
                            : bone(NULL), bone_ref(bone_ref)
                        {}
                        
                        bool hasBone() const
                        {
                            return (bone != NULL);
                        }
                        bool hasBone_Ref() const
                        {
                            return (bone_ref != NULL);
                        }
                    };
                    
                    std::map<int, Bone_Container> bones;
                    
                    class Bone
                    {
                        public:
                        
                        int id;
                        int parent;  // a bone id
                        float x;
                        float y;
                        float angle;
                        float scale_x;
                        float scale_y;
                        float r;
                        float g;
                        float b;
                        float a;
                        Meta_Data* meta_data;
                        
                        
                        Bone();
                        Bone(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                        
                    };
                    
                    class Bone_Ref
                    {
                        public:
                        
                        int id;
                        int parent;  // a bone id
                        int timeline;
                        int key;
                        
                        Bone_Ref();
                        Bone_Ref(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                    };
                        
                    class Object
                    {
                        public:
                        
                        int id;
                        int parent; // a bone id
                        std::string object_type;
                        int atlas;
                        int folder;
                        int file;
                        std::string usage;
                        std::string blend_mode;
                        std::string name;
                        float x;
                        float y;
                        float pivot_x;
                        float pivot_y;
                        int pixel_art_mode_x;
                        int pixel_art_mode_y;
                        int pixel_art_mode_pivot_x;
                        int pixel_art_mode_pivot_y;
                        float angle;
                        float w;
                        float h;
                        float scale_x;
                        float scale_y;
                        float r;
                        float g;
                        float b;
                        float a;
                        std::string variable_type;
                        std::string value_string;
                        int value_int;
                        int min_int;
                        int max_int;
                        float value_float;
                        float min_float;
                        float max_float;
                        int animation;
                        float t;
                        int z_index;
                        float volume;
                        float panning;
                        
                        Meta_Data* meta_data;
                        
                        Object();
                        Object(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                        
                    };
                        
                    class Object_Ref
                    {
                        public:
                        
                        int id;
                        int parent;  // a bone id
                        int timeline;
                        int key;
                        int z_index;
                        
                        Object_Ref();
                        Object_Ref(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                    };
                };
                
            };
            
            Mainline mainline;
        
            class Timeline;
            std::map<int, Timeline*> timelines;
        
            Animation();
            Animation(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
            
            
            
            class Timeline
            {
                public:
                
                int id;
                std::string name;
                std::string object_type;
                std::string variable_type;
                std::string usage;
                Meta_Data* meta_data;
                
                Timeline();
                Timeline(TiXmlElement* elem);
                
                bool load(TiXmlElement* elem);
                void log(int recursive_depth = 0) const;
                void clear();
                
                class Key;
                std::map<int, Key*> keys;
                
                class Key
                {
                    public:
                    
                    int id;
                    int time;
                    std::string curve_type;
                    float c1;
                    float c2;
                    int spin;
                    
                    bool has_object;
                    
                    Key();
                    Key(TiXmlElement* elem);
                    
                    bool load(TiXmlElement* elem);
                    void log(int recursive_depth = 0) const;
                    void clear();
                    
                    
                    Meta_Data_Tweenable* meta_data;
                
                    class Bone
                    {
                        public:
                        
                        float x;
                        float y;
                        float angle;
                        float scale_x;
                        float scale_y;
                        float r;
                        float g;
                        float b;
                        float a;
                        Meta_Data_Tweenable* meta_data;
                        
                        Bone();
                        Bone(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                    };
                    
                    Bone bone;
                    
                    class Object
                    {
                        public:
                        
                        //std::string object_type; // Does this exist?
                        int atlas;
                        int folder;
                        int file;
                        //std::string usage;  // Does this exist?
                        std::string name;
                        float x;
                        float y;
                        float pivot_x;
                        float pivot_y;
                        // pixel_art_mode stuff?
                        float angle;
                        float w;
                        float h;
                        float scale_x;
                        float scale_y;
                        float r;
                        float g;
                        float b;
                        float a;
                        std::string blend_mode;
                        //std::string variable_type; // Does this exist?
                        std::string value_string;
                        int value_int;
                        int min_int;
                        int max_int;
                        float value_float;
                        float min_float;
                        float max_float;
                        int animation;
                        float t;
                        //int z_index; // Does this exist?  Object_Ref has it, so probably not.
                        float volume;
                        float panning;
                        Meta_Data_Tweenable* meta_data;
                        
                        Object();
                        Object(TiXmlElement* elem);
                        
                        bool load(TiXmlElement* elem);
                        void log(int recursive_depth = 0) const;
                        void clear();
                        
                    };
                    
                    Object object;
                };
            };
        };
    };

    class Character_Map
    {
        public:
        
        int id;
        std::string name;
        
        Character_Map();
        Character_Map(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
        class Map
        {
            public:
            
            int atlas;
            int folder;
            int file;
            int target_atlas;
            int target_folder;
            int target_file;
            
            Map();
            Map(TiXmlElement* elem);
            
            bool load(TiXmlElement* elem);
            void log(int recursive_depth = 0) const;
            void clear();
        };
        
        Map map;
    };

    class Document_Info
    {
        public:
        
        std::string author;
        std::string copyright;
        std::string license;
        std::string version;
        std::string last_modified;
        std::string notes;
        
        Document_Info();
        Document_Info(TiXmlElement* elem);
        
        bool load(TiXmlElement* elem);
        void log(int recursive_depth = 0) const;
        void clear();
        
    };
    
    Document_Info document_info;
    
    int getNumAnimations(int entity) const;
};

/*! A storage class for images in a renderer-specific format (to be inherited).
 */
class FileSystem
{
    public:
    
    virtual ~FileSystem(){}
    
	/*! Loads all images referenced by the given SCML data.
	 * \param data SCML data object
	 */
    virtual void load(SCML::Data* data);
    
	/*! Loads an image from a file and stores it so that the folderID and fileID can be used to reference the image.
	 * \param folderID Integer folder ID
	 * \param fileID Integer file ID
	 * \param filename Path of the image file
	 * \return true on success, false on failure
	 */
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename) = 0;
    
	/*! Cleans up all memory used by the FileSystem to store images, resetting it to an empty state.
	 */
    virtual void clear() = 0;
    
	/*! Gets the dimensions of an image
	 * \param folderID Integer folder ID
	 * \param fileID Integer file ID
	 * \return A pair consisting of the width and height of the image.  Returns (0,0) on error.
	 */
    virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const = 0;
};


/*! A class to directly interface with SCML character data and draw it (to be inherited).
 * Derived classes provide the means for the Entity to draw itself with a specific renderer.
 */
class Entity
{
    public:
    
    /*! Integer index of the SCML entity */
    int entity;
    /*! Integer index of the current SCML entity's animation */
    int animation;
    /*! Integer index of the current animation's current mainline keyframe */
    int key;
    
    /*! Time (in milliseconds) tracking the position of the animation from its beginning. */
    int time;
    
    
    std::string name;

    class Animation;
    std::map<int, Animation*> animations;
    
    //Meta_Data* meta_data;

    class Animation
    {
        public:
        
        int id;
        std::string name;
        int length;
        std::string looping;
        int loop_to;
        
        //Meta_Data* meta_data;
        
        class Mainline
        {
            public:
            
            Mainline(SCML::Data::Entity::Animation::Mainline* mainline);
            
            void clear();
            
            class Key;
            std::map<int, Key*> keys;
            
            class Key
            {
                public:
                
                int id;
                int time;
                //Meta_Data* meta_data;
                
                Key(SCML::Data::Entity::Animation::Mainline::Key* key);
                
                void clear();
                
                class Object;
                class Object_Ref;
                
                class Object_Container
                {
                    public:
                    Object* object;
                    Object_Ref* object_ref;
                    
                    Object_Container(Object* object)
                        : object(object), object_ref(NULL)
                    {}
                    Object_Container(Object_Ref* object_ref)
                        : object(NULL), object_ref(object_ref)
                    {}
                    
                    bool hasObject() const
                    {
                        return (object != NULL);
                    }
                    bool hasObject_Ref() const
                    {
                        return (object_ref != NULL);
                    }
                };
                
                std::map<int, Object_Container> objects;
                
                
                class Bone;
                class Bone_Ref;
                
                class Bone_Container
                {
                    public:
                    Bone* bone;
                    Bone_Ref* bone_ref;
                    
                    Bone_Container(Bone* bone)
                        : bone(bone), bone_ref(NULL)
                    {}
                    Bone_Container(Bone_Ref* bone_ref)
                        : bone(NULL), bone_ref(bone_ref)
                    {}
                    
                    bool hasBone() const
                    {
                        return (bone != NULL);
                    }
                    bool hasBone_Ref() const
                    {
                        return (bone_ref != NULL);
                    }
                };
                
                std::map<int, Bone_Container> bones;
                
                
                class Bone
                {
                    public:
                    
                    int id;
                    int parent;  // a bone id
                    float x;
                    float y;
                    float angle;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    //Meta_Data* meta_data;
                    
                    Bone(SCML::Data::Entity::Animation::Mainline::Key::Bone* bone);
                    
                    void clear();
                    
                };
                
                class Bone_Ref
                {
                    public:
                    
                    int id;
                    int parent;  // a bone id
                    int timeline;
                    int key;
                    
                    Bone_Ref(SCML::Data::Entity::Animation::Mainline::Key::Bone_Ref* bone_ref);
                    
                    void clear();
                };
                
                class Object
                {
                    public:
                    
                    int id;
                    int parent; // a bone id
                    std::string object_type;
                    int atlas;
                    int folder;
                    int file;
                    std::string usage;
                    std::string blend_mode;
                    std::string name;
                    float x;
                    float y;
                    float pivot_x;
                    float pivot_y;
                    int pixel_art_mode_x;
                    int pixel_art_mode_y;
                    int pixel_art_mode_pivot_x;
                    int pixel_art_mode_pivot_y;
                    float angle;
                    float w;
                    float h;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    std::string variable_type;
                    std::string value_string;
                    int value_int;
                    int min_int;
                    int max_int;
                    float value_float;
                    float min_float;
                    float max_float;
                    int animation;
                    float t;
                    int z_index;
                    float volume;
                    float panning;
                    
                    //Meta_Data* meta_data;
                    
                    Object(SCML::Data::Entity::Animation::Mainline::Key::Object* object);
                    
                    void clear();
                    
                };
                    
                class Object_Ref
                {
                    public:
                    
                    int id;
                    int parent;  // a bone id
                    int timeline;
                    int key;
                    int z_index;
                    
                    Object_Ref(SCML::Data::Entity::Animation::Mainline::Key::Object_Ref* object_ref);
                    
                    void clear();
                };
            };
            
        };
        
        Mainline mainline;
    
        class Timeline;
        std::map<int, Timeline*> timelines;
    
        Animation(SCML::Data::Entity::Animation* animation);
        
        void clear();
        
        
        
        class Timeline
        {
            public:
            
            int id;
            std::string name;
            std::string object_type;
            std::string variable_type;
            std::string usage;
            //Meta_Data* meta_data;
            
            Timeline(SCML::Data::Entity::Animation::Timeline* timeline);
            
            void clear();
            
            class Key;
            std::map<int, Key*> keys;
            
            class Key
            {
                public:
                
                int id;
                int time;
                std::string curve_type;
                float c1;
                float c2;
                int spin;
                
                bool has_object;
                
                Key(SCML::Data::Entity::Animation::Timeline::Key* key);
                
                void clear();
                
                
                //Meta_Data_Tweenable* meta_data;
            
                class Bone
                {
                    public:
                    
                    float x;
                    float y;
                    float angle;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    //Meta_Data_Tweenable* meta_data;
                    
                    Bone(SCML::Data::Entity::Animation::Timeline::Key::Bone* bone);
                    
                    void clear();
                };
                
                Bone bone;
                
                class Object
                {
                    public:
                    
                    //std::string object_type; // Does this exist?
                    int atlas;
                    int folder;
                    int file;
                    //std::string usage;  // Does this exist?
                    std::string name;
                    float x;
                    float y;
                    float pivot_x;
                    float pivot_y;
                    // pixel_art_mode stuff?
                    float angle;
                    float w;
                    float h;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    std::string blend_mode;
                    //std::string variable_type; // Does this exist?
                    std::string value_string;
                    int value_int;
                    int min_int;
                    int max_int;
                    float value_float;
                    float min_float;
                    float max_float;
                    int animation;
                    float t;
                    //int z_index; // Does this exist?  Object_Ref has it, so probably not.
                    float volume;
                    float panning;
                    //Meta_Data_Tweenable* meta_data;
                    
                    Object(SCML::Data::Entity::Animation::Timeline::Key::Object* object);
                    
                    void clear();
                    
                };
                
                Object object;
            };
        };
    };
    
    
    
    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);
    virtual ~Entity();
    
    virtual void load(SCML::Data* data);

    virtual void clear();
    
	/*! Converts the given values from the renderer-specific coordinate system to the SCML coordinate system.
	 * SCML coords: +x to the right, +y up, +angle counter-clockwise
	 * \param x x-position in renderer coordinate system
	 * \param y y-position in renderer coordinate system
	 * \param angle Angle (in degrees) in renderer coordinate system
	 */
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle)
    {}
    
	/*! Gets the dimensions of an image (from a FileSystem, presumably)
	 * \param folderID Integer folder ID of the image
	 * \param fileID Integer file ID of the image
	 * \return A pair consisting of the width and height of the image.  Returns (0,0) on error.
	 */
    virtual std::pair<unsigned int, unsigned int> getImageDimensions(int folderID, int fileID) const = 0;
    
	/*! Updates the state of the entity, incrementing its timer and changing the keyframe.
	 * \param dt_ms Change in time since last update, in milliseconds
	 */
    virtual void update(int dt_ms);
    
	/*! Draws the entity using a specific renderer by calling draw_internal().
	 * \param x x-position in renderer coordinate system
	 * \param y y-position in renderer coordinate system
	 * \param angle Angle (in degrees) in renderer coordinate system
	 * \param scale_x Scale factor in the x-direction
	 * \param scale_y Scale factor in the y-direction
	 */
    virtual void draw(float x, float y, float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f);
    
    virtual void draw_simple_object(float x, float y, float angle, float scale_x, float scale_y, Animation::Mainline::Key::Object* obj);
    virtual void draw_tweened_object(float x, float y, float angle, float scale_x, float scale_y, Animation::Mainline::Key::Object_Ref* ref1, Animation::Mainline::Key::Object_Ref* ref2);
    
	/*! Draws an image using a specific renderer.
	 * \param folderID Integer folder ID of the image
	 * \param fileID Integer file ID of the image
	 * \param x x-position in SCML coordinate system
	 * \param y y-position in SCML coordinate system
	 * \param angle Angle (in degrees) in SCML coordinate system
	 * \param scale_x Scale factor in the x-direction
	 * \param scale_y Scale factor in the y-direction
	 */
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y) = 0;
    
	/*! Chooses and resets the current animation.
	 * \param animation Integer animation ID
	 */
    virtual void startAnimation(int animation);
    
    
    int getNumAnimations() const;
    Animation* getAnimation(int animation) const;
    Animation::Mainline::Key* getKey(int animation, int key) const;
    Animation::Mainline::Key::Bone_Ref* getBoneRef(int animation, int key, int bone_ref) const;
    Animation::Mainline::Key::Object_Ref* getObjectRef(int animation, int key, int object_ref) const;
    
    int getNextKeyID(int animation, int lastKey) const;
    Animation::Timeline::Key* getTimelineKey(int animation, int timeline, int key);
    Animation::Timeline::Key::Object* getTimelineObject(int animation, int timeline, int key);
    Animation::Timeline::Key::Bone* getTimelineBone(int animation, int timeline, int key);
};


}


#endif
