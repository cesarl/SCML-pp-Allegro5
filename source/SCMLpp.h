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
                    std::map<int, Object*> objects;
                    class Object_Ref;
                    std::map<int, Object_Ref*> object_refs;
                        
                    class Bone;
                    std::map<int, Bone*> bones;
                    class Bone_Ref;
                    std::map<int, Bone_Ref*> bone_refs;
                    
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
                        std::string animation;  // Shouldn't this be an int?
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
    Entity::Animation* getAnimation(int entity, int animation) const;
    Entity::Animation::Mainline::Key* getKey(int entity, int animation, int key) const;
    
    int getNextKeyID(int entity, int animation, int lastKey) const;
    Entity::Animation::Timeline::Key* getTimelineKey(int entity, int animation, int timeline, int key);
    Entity::Animation::Timeline::Key::Object* getTimelineObject(int entity, int animation, int timeline, int key);
};


class FileSystem
{
    public:
    
    virtual ~FileSystem(){}
    virtual void load(SCML::Data* data) = 0;
    virtual void clear() = 0;
};


class Entity
{
    public:
    
    int entity;
    int animation;
    int key;
    
    int time;  // milliseconds
    
    Entity();
    Entity(int entity, int animation = 0, int key = 0);
    virtual ~Entity(){}
    
    virtual void update(SCML::Data* data, int dt_ms);
    virtual void draw(SCML::Data* data, float x, float y, float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f) = 0;
    
    virtual void startAnimation(int animation);
};


}


#endif
