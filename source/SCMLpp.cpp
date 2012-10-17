#include "SCMLpp.h"

#include "XML_Helpers.h"
#include "stdarg.h"

using namespace std;

namespace SCML
{

static void log(const char* formatted_text, ...)
{
    static char buffer[2000];
    if(formatted_text == NULL)
        return;

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);
    
    printf("%s", buffer);
}



Data::Data()
    : pixel_art_mode(false)
{}

Data::Data(const char* file)
    : pixel_art_mode(false)
{
    load(file);
}

Data::Data(TiXmlElement* elem)
    : pixel_art_mode(false)
{
    load(elem);
}

Data::Data(const Data& copy)
    : scml_version(copy.scml_version), generator(copy.generator), generator_version(copy.generator_version), pixel_art_mode(copy.pixel_art_mode)
{
    clone(copy, true);
}

Data& Data::operator=(const Data& copy)
{
    clone(copy);
    return *this;
}

Data& Data::clone(const Data& copy, bool skip_base)
{
    if(!skip_base)
    {
        scml_version = copy.scml_version;
        generator = copy.generator;
        generator_version = copy.generator_version;
        pixel_art_mode = copy.pixel_art_mode;
    }
    
    // TODO: Clone the subobjects
    
    return *this;
}

Data::~Data()
{
    clear();
}

bool Data::load(const char* file)
{
    name = file;
    
    TiXmlDocument doc;

    if(!doc.LoadFile(file))
    {
        SCML::log("SCML::Data failed to load: Couldn't open %s.\n", file);
        SCML::log("%s\n", doc.ErrorDesc());
        return false;
    }

    TiXmlElement* root = doc.FirstChildElement("spriter_data");
    if(root == NULL)
    {
        SCML::log("SCML::Data failed to load: No spriter_data XML element in %s.\n", file);
        return false;
    }
    
    load(root);
    
    doc.Clear();
    return true;
}

bool Data::load(TiXmlElement* elem)
{
    if(elem == NULL)
        return false;
    
    scml_version = xmlGetStringAttr(elem, "scml_version", "");
    generator = xmlGetStringAttr(elem, "generator", "(Spriter)");
    generator_version = xmlGetStringAttr(elem, "generator_version", "(1.0)");
    pixel_art_mode = xmlGetBoolAttr(elem, "pixel_art_mode", false);
    
    TiXmlElement* meta_data_child = elem->FirstChildElement("meta_data");
    if(meta_data_child != NULL)
        meta_data.load(meta_data_child);
    
    for(TiXmlElement* child = elem->FirstChildElement("folder"); child != NULL; child = child->NextSiblingElement("folder"))
    {
        Folder* folder = new Folder;
        if(folder->load(child))
        {
            if(!folders.insert(make_pair(folder->id, folder)).second)
            {
                SCML::log("SCML::Data loaded a folder with a duplicate id (%d).\n", folder->id);
                delete folder;
            }
        }
        else
        {
            SCML::log("SCML::Data failed to load a folder.\n");
            delete folder;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("atlas"); child != NULL; child = child->NextSiblingElement("atlas"))
    {
        Atlas* atlas = new Atlas;
        if(atlas->load(child))
        {
            if(!atlases.insert(make_pair(atlas->id, atlas)).second)
            {
                SCML::log("SCML::Data loaded an atlas with a duplicate id (%d).\n", atlas->id);
                delete atlas;
            }
        }
        else
        {
            SCML::log("SCML::Data failed to load an atlas.\n");
            delete atlas;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("entity"); child != NULL; child = child->NextSiblingElement("entity"))
    {
        Entity* entity = new Entity;
        if(entity->load(child))
        {
            if(!entities.insert(make_pair(entity->id, entity)).second)
            {
                SCML::log("SCML::Data loaded an entity with a duplicate id (%d).\n", entity->id);
                delete entity;
            }
        }
        else
        {
            SCML::log("SCML::Data failed to load an entity.\n");
            delete entity;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("character_map"); child != NULL; child = child->NextSiblingElement("character_map"))
    {
        Character_Map* character_map = new Character_Map;
        if(character_map->load(child))
        {
            if(!character_maps.insert(make_pair(character_map->id, character_map)).second)
            {
                SCML::log("SCML::Data loaded a character_map with a duplicate id (%d).\n", character_map->id);
                delete character_map;
            }
        }
        else
        {
            SCML::log("SCML::Data failed to load a character_map.\n");
            delete character_map;
        }
    }
    
    TiXmlElement* document_info_elem = elem->FirstChildElement("document_info");
    if(document_info_elem != NULL)
        document_info.load(document_info_elem);
    
    return true;
}

void Data::log(int recursive_depth) const
{
    SCML::log("scml_version=%s\n", scml_version.c_str());
    SCML::log("generator=%s\n", generator.c_str());
    SCML::log("generator_version=%s\n", generator_version.c_str());
    SCML::log("pixel_art_mode=%s\n", toString(pixel_art_mode).c_str());
    
    if(recursive_depth == 0)
        return;
    
    meta_data.log(recursive_depth - 1);
    
    for(map<int, Folder*>::const_iterator e = folders.begin(); e != folders.end(); e++)
    {
        SCML::log("Folder:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<int, Atlas*>::const_iterator e = atlases.begin(); e != atlases.end(); e++)
    {
        SCML::log("Atlas:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<int, Entity*>::const_iterator e = entities.begin(); e != entities.end(); e++)
    {
        SCML::log("Entity:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<int, Character_Map*>::const_iterator e = character_maps.begin(); e != character_maps.end(); e++)
    {
        SCML::log("Character_Map:\n");
        e->second->log(recursive_depth - 1);
    }
    
    SCML::log("Document_Info:\n");
    document_info.log(recursive_depth - 1);
}

void Data::clear()
{
    scml_version = "";
    generator = "(Spriter)";
    generator_version = "(1.0)";
    pixel_art_mode = false;
    
    meta_data.clear();
    
    for(map<int, Folder*>::iterator e = folders.begin(); e != folders.end(); e++)
    {
        delete e->second;
    }
    folders.clear();
    
    for(map<int, Atlas*>::iterator e = atlases.begin(); e != atlases.end(); e++)
    {
        delete e->second;
    }
    atlases.clear();
    
    for(map<int, Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        delete e->second;
    }
    entities.clear();
    
    for(map<int, Character_Map*>::iterator e = character_maps.begin(); e != character_maps.end(); e++)
    {
        delete e->second;
    }
    character_maps.clear();
    
    document_info.clear();
}


int Data::getNumAnimations(int entity) const
{
    map<int, Entity*>::const_iterator e = entities.find(entity);
    if(e == entities.end())
        return -1;
    
    return e->second->animations.size();
}

Data::Entity::Animation* Data::getAnimation(int entity, int animation) const
{
    map<int, Entity*>::const_iterator e = entities.find(entity);
    if(e == entities.end())
        return NULL;
    
    map<int, Entity::Animation*>::const_iterator a = e->second->animations.find(animation);
    if(a == e->second->animations.end())
        return NULL;
    
    return a->second;
}

Data::Entity::Animation::Mainline::Key* Data::getKey(int entity, int animation, int key) const
{
    map<int, Entity*>::const_iterator e = entities.find(entity);
    if(e == entities.end())
        return NULL;
    
    map<int, Entity::Animation*>::const_iterator a = e->second->animations.find(animation);
    if(a == e->second->animations.end())
        return NULL;
    
    map<int, Entity::Animation::Mainline::Key*>::const_iterator k = a->second->mainline.keys.find(key);
    if(k == a->second->mainline.keys.end())
        return NULL;
    
    return k->second;
}


Data::Entity::Animation::Timeline::Key::Object* Data::getTimelineObject(int entity, int animation, int timeline, int key)
{
    map<int, Entity*>::const_iterator e = entities.find(entity);
    if(e == entities.end())
        return NULL;
    
    map<int, Entity::Animation*>::const_iterator a = e->second->animations.find(animation);
    if(a == e->second->animations.end())
        return NULL;
    
    map<int, Entity::Animation::Timeline*>::const_iterator t = a->second->timelines.find(timeline);
    if(t == a->second->timelines.end())
        return NULL;
    
    map<int, Entity::Animation::Timeline::Key*>::const_iterator k = t->second->keys.find(key);
    if(k == t->second->keys.end())
        return NULL;
    
    if(!k->second->has_object)
        return NULL;
    
    return &k->second->object;
}






Data::Meta_Data::Meta_Data()
{}

Data::Meta_Data::Meta_Data(TiXmlElement* elem)
{
    load(elem);
}

bool Data::Meta_Data::load(TiXmlElement* elem)
{
    for(TiXmlElement* child = elem->FirstChildElement("variable"); child != NULL; child = child->NextSiblingElement("variable"))
    {
        Variable* variable = new Variable;
        if(variable->load(child))
        {
            if(!variables.insert(make_pair(variable->name, variable)).second)
            {
                SCML::log("SCML::Data::Meta_Data loaded a variable with a duplicate name (%s).\n", variable->name.c_str());
                delete variable;
            }
        }
        else
        {
            SCML::log("SCML::Data::Meta_Data failed to load a variable.\n");
            delete variable;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("tag"); child != NULL; child = child->NextSiblingElement("tag"))
    {
        Tag* tag = new Tag;
        if(tag->load(child))
        {
            if(!tags.insert(make_pair(tag->name, tag)).second)
            {
                SCML::log("SCML::Data::Meta_Data loaded a tag with a duplicate name (%s).\n", tag->name.c_str());
                delete tag;
            }
        }
        else
        {
            SCML::log("SCML::Data::Meta_Data failed to load a tag.\n");
            delete tag;
        }
    }
    
    return true;
}

void Data::Meta_Data::log(int recursive_depth) const
{
    if(recursive_depth == 0)
        return;
    
    for(map<std::string, Variable*>::const_iterator e = variables.begin(); e != variables.end(); e++)
    {
        SCML::log("Variable:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<std::string, Tag*>::const_iterator e = tags.begin(); e != tags.end(); e++)
    {
        SCML::log("Tag:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Meta_Data::clear()
{
    for(map<std::string, Variable*>::iterator e = variables.begin(); e != variables.end(); e++)
    {
        delete e->second;
    }
    variables.clear();
    
    for(map<std::string, Tag*>::iterator e = tags.begin(); e != tags.end(); e++)
    {
        delete e->second;
    }
    tags.clear();
}




Data::Meta_Data::Variable::Variable()
    : type("string"), value_int(0), value_float(0.0f)
{}

Data::Meta_Data::Variable::Variable(TiXmlElement* elem)
    : type("string"), value_int(0), value_float(0.0f)
{
    load(elem);
}

bool Data::Meta_Data::Variable::load(TiXmlElement* elem)
{
    name = xmlGetStringAttr(elem, "name", "");
    type = xmlGetStringAttr(elem, "type", "string");
    
    if(type == "string")
        value_string = xmlGetStringAttr(elem, "value", "");
    else if(type == "int")
        value_int = xmlGetIntAttr(elem, "value", 0);
    else if(type == "float")
        value_float = xmlGetFloatAttr(elem, "value", 0.0f);
    else
        SCML::log("Data::Meta_Data::Variable loaded invalid variable type (%s) named '%s'.\n", type.c_str(), name.c_str());
    return true;
}

void Data::Meta_Data::Variable::log(int recursive_depth) const
{
    SCML::log("name=%s\n", name.c_str());
    SCML::log("type=%s\n", type.c_str());
    if(type == "string")
        SCML::log("value=%s\n", value_string.c_str());
    else if(type == "int")
        SCML::log("value=%d\n", value_int);
    else if(type == "float")
        SCML::log("value=%f\n", value_float);
}

void Data::Meta_Data::Variable::clear()
{
    name.clear();
    type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
}




Data::Meta_Data::Tag::Tag()
{}

Data::Meta_Data::Tag::Tag(TiXmlElement* elem)
{
    load(elem);
}

bool Data::Meta_Data::Tag::load(TiXmlElement* elem)
{
    name = xmlGetStringAttr(elem, "name", "");
    
    return true;
}

void Data::Meta_Data::Tag::log(int recursive_depth) const
{
    SCML::log("name=%s\n", name.c_str());
}

void Data::Meta_Data::Tag::clear()
{
    name.clear();
}





Data::Folder::Folder()
    : id(0)
{}

Data::Folder::Folder(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Folder::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    
    for(TiXmlElement* child = elem->FirstChildElement("file"); child != NULL; child = child->NextSiblingElement("file"))
    {
        File* file = new File;
        if(file->load(child))
        {
            if(!files.insert(make_pair(file->id, file)).second)
            {
                SCML::log("SCML::Data::Folder loaded a file with a duplicate id (%d).\n", file->id);
                delete file;
            }
        }
        else
        {
            SCML::log("SCML::Data::Folder failed to load a file.\n");
            delete file;
        }
    }
    
    return true;
}

void Data::Folder::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    
    if(recursive_depth == 0)
        return;
    
    for(map<int, File*>::const_iterator e = files.begin(); e != files.end(); e++)
    {
        SCML::log("File:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Folder::clear()
{
    id = 0;
    name.clear();
    
    for(map<int, File*>::iterator e = files.begin(); e != files.end(); e++)
    {
        delete e->second;
    }
    files.clear();
}





Data::Folder::File::File()
    : id(0)
{}

Data::Folder::File::File(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Folder::File::load(TiXmlElement* elem)
{
    type = xmlGetStringAttr(elem, "type", "image");
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    pivot_x = xmlGetFloatAttr(elem, "pivot_x", 0.0f);
    pivot_y = xmlGetFloatAttr(elem, "pivot_y", 0.0f);
    width = xmlGetIntAttr(elem, "width", 0);
    height = xmlGetIntAttr(elem, "height", 0);
    atlas_x = xmlGetIntAttr(elem, "atlas_x", 0);
    atlas_y = xmlGetIntAttr(elem, "atlas_y", 0);
    offset_x = xmlGetIntAttr(elem, "offset_x", 0);
    offset_y = xmlGetIntAttr(elem, "offset_y", 0);
    original_width = xmlGetIntAttr(elem, "original_width", 0);
    original_height = xmlGetIntAttr(elem, "original_height", 0);
    
    return true;
}

void Data::Folder::File::log(int recursive_depth) const
{
    SCML::log("type=%s\n", type.c_str());
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    SCML::log("pivot_x=%f\n", pivot_x);
    SCML::log("pivot_y=%f\n", pivot_y);
    SCML::log("width=%d\n", width);
    SCML::log("height=%d\n", height);
    SCML::log("atlas_x=%d\n", atlas_x);
    SCML::log("atlas_y=%d\n", atlas_y);
    SCML::log("offset_x=%d\n", offset_x);
    SCML::log("offset_y=%d\n", offset_y);
    SCML::log("original_width=%d\n", original_width);
    SCML::log("original_height=%d\n", original_height);
}

void Data::Folder::File::clear()
{
    type = "image";
    id = 0;
    name.clear();
    pivot_x = 0.0f;
    pivot_y = 0.0f;
    width = 0;
    height = 0;
    atlas_x = 0;
    atlas_y = 0;
    offset_x = 0;
    offset_y = 0;
    original_width = 0;
    original_height = 0;
}





Data::Atlas::Atlas()
    : id(0)
{}

Data::Atlas::Atlas(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Atlas::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    data_path = xmlGetStringAttr(elem, "data_path", "");
    image_path = xmlGetStringAttr(elem, "image_path", "");
    
    for(TiXmlElement* child = elem->FirstChildElement("folder"); child != NULL; child = child->NextSiblingElement("folder"))
    {
        Folder* folder = new Folder;
        if(folder->load(child))
        {
            if(!folders.insert(make_pair(folder->id, folder)).second)
            {
                SCML::log("SCML::Data::Atlas loaded a folder with a duplicate id (%d).\n", folder->id);
                delete folder;
            }
        }
        else
        {
            SCML::log("SCML::Data::Atlas failed to load a folder.\n");
            delete folder;
        }
    }
    
    return true;
}

void Data::Atlas::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("data_path=%s\n", data_path.c_str());
    SCML::log("image_path=%s\n", image_path.c_str());
    
    if(recursive_depth == 0)
        return;
    
    for(map<int, Folder*>::const_iterator e = folders.begin(); e != folders.end(); e++)
    {
        SCML::log("Folder:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Atlas::clear()
{
    id = 0;
    data_path.clear();
    image_path.clear();
    
    for(map<int, Folder*>::iterator e = folders.begin(); e != folders.end(); e++)
    {
        delete e->second;
    }
    folders.clear();
}





Data::Atlas::Folder::Folder()
    : id(0)
{}

Data::Atlas::Folder::Folder(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Atlas::Folder::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    
    for(TiXmlElement* child = elem->FirstChildElement("image"); child != NULL; child = child->NextSiblingElement("image"))
    {
        Image* image = new Image;
        if(image->load(child))
        {
            if(!images.insert(make_pair(image->id, image)).second)
            {
                SCML::log("SCML::Data::Atlas::Folder loaded an image with a duplicate id (%d).\n", image->id);
                delete image;
            }
        }
        else
        {
            SCML::log("SCML::Data::Atlas::Folder failed to load an image.\n");
            delete image;
        }
    }
    
    return true;
}

void Data::Atlas::Folder::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    
    if(recursive_depth == 0)
        return;
    
    for(map<int, Image*>::const_iterator e = images.begin(); e != images.end(); e++)
    {
        SCML::log("Image:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Atlas::Folder::clear()
{
    id = 0;
    name.clear();
    
    for(map<int, Image*>::iterator e = images.begin(); e != images.end(); e++)
    {
        delete e->second;
    }
    images.clear();
}





Data::Atlas::Folder::Image::Image()
    : id(0)
{}

Data::Atlas::Folder::Image::Image(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Atlas::Folder::Image::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    full_path = xmlGetStringAttr(elem, "full_path", "");
    
    return true;
}

void Data::Atlas::Folder::Image::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("full_path=%s\n", full_path.c_str());
}

void Data::Atlas::Folder::Image::clear()
{
    id = 0;
    full_path.clear();
}









Data::Entity::Entity()
    : id(0)
{}

Data::Entity::Entity(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Entity::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    
    for(TiXmlElement* child = elem->FirstChildElement("animation"); child != NULL; child = child->NextSiblingElement("animation"))
    {
        Animation* animation = new Animation;
        if(animation->load(child))
        {
            if(!animations.insert(make_pair(animation->id, animation)).second)
            {
                SCML::log("SCML::Data::Entity loaded an animation with a duplicate id (%d).\n", animation->id);
                delete animation;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity failed to load an animation.\n");
            delete animation;
        }
    }
    
    return true;
}

void Data::Entity::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    
    if(recursive_depth == 0)
        return;
    
    for(map<int, Animation*>::const_iterator e = animations.begin(); e != animations.end(); e++)
    {
        SCML::log("Animation:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::clear()
{
    id = 0;
    name.clear();
    
    for(map<int, Animation*>::iterator e = animations.begin(); e != animations.end(); e++)
    {
        delete e->second;
    }
    animations.clear();
}









Data::Entity::Animation::Animation()
    : id(0), length(0), looping("true"), loop_to(0)
{}

Data::Entity::Animation::Animation(TiXmlElement* elem)
    : id(0), length(0), looping("true"), loop_to(0)
{
    load(elem);
}

bool Data::Entity::Animation::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    length = xmlGetIntAttr(elem, "length", 0);
    looping = xmlGetStringAttr(elem, "looping", "");
    loop_to = xmlGetIntAttr(elem, "loop_to", 0);
    
    TiXmlElement* mainline_elem = elem->FirstChildElement("mainline");
    if(mainline_elem == NULL || !mainline.load(mainline_elem))
    {
        SCML::log("SCML::Data::Entity::Animation failed to load the mainline.\n");
        mainline.clear();
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("timeline"); child != NULL; child = child->NextSiblingElement("timeline"))
    {
        Timeline* timeline = new Timeline;
        if(timeline->load(child))
        {
            if(!timelines.insert(make_pair(timeline->id, timeline)).second)
            {
                SCML::log("SCML::Data::Entity::Animation loaded a timeline with a duplicate id (%d).\n", timeline->id);
                delete timeline;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation failed to load a timeline.\n");
            delete timeline;
        }
    }
    
    return true;
}

void Data::Entity::Animation::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    SCML::log("length=%d\n", length);
    SCML::log("looping=%s\n", looping.c_str());
    SCML::log("loop_to=%d\n", loop_to);
    
    if(recursive_depth == 0)
        return;
    
    mainline.log(recursive_depth - 1);
    
    for(map<int, Timeline*>::const_iterator e = timelines.begin(); e != timelines.end(); e++)
    {
        SCML::log("Timeline:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::clear()
{
    id = 0;
    name.clear();
    length = 0;
    looping = "true";
    loop_to = 0;
    
    mainline.clear();
    
    for(map<int, Timeline*>::iterator e = timelines.begin(); e != timelines.end(); e++)
    {
        delete e->second;
    }
    timelines.clear();
}









Data::Entity::Animation::Mainline::Mainline()
{}

Data::Entity::Animation::Mainline::Mainline(TiXmlElement* elem)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::load(TiXmlElement* elem)
{
    for(TiXmlElement* child = elem->FirstChildElement("key"); child != NULL; child = child->NextSiblingElement("key"))
    {
        Key* key = new Key;
        if(key->load(child))
        {
            if(!keys.insert(make_pair(key->id, key)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Mainline loaded a key with a duplicate id (%d).\n", key->id);
                delete key;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Mainline failed to load a key.\n");
            delete key;
        }
    }
    
    return true;
}

void Data::Entity::Animation::Mainline::log(int recursive_depth) const
{
    if(recursive_depth == 0)
        return;
        
    for(map<int, Key*>::const_iterator e = keys.begin(); e != keys.end(); e++)
    {
        SCML::log("Key:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Mainline::clear()
{
    for(map<int, Key*>::iterator e = keys.begin(); e != keys.end(); e++)
    {
        delete e->second;
    }
    keys.clear();
}








Data::Entity::Animation::Mainline::Key::Key()
    : id(0), time(0)
{}

Data::Entity::Animation::Mainline::Key::Key(TiXmlElement* elem)
    : id(0), time(0)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    time = xmlGetIntAttr(elem, "time", 0);
    
    TiXmlElement* hierarchy_child = elem->FirstChildElement("hierarchy");
    if(hierarchy_child != NULL && !hierarchy.load(hierarchy_child))
    {
        SCML::log("SCML::Data::Entity::Animation::Mainline::Key failed to load the hierarchy.\n");
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("object"); child != NULL; child = child->NextSiblingElement("object"))
    {
        Object* object = new Object;
        if(object->load(child))
        {
            if(!objects.insert(make_pair(object->id, object)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Mainline::Key loaded an object with a duplicate id (%d).\n", object->id);
                delete object;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Mainline::Key failed to load an object.\n");
            delete object;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("object_ref"); child != NULL; child = child->NextSiblingElement("object_ref"))
    {
        Object_Ref* object_ref = new Object_Ref;
        if(object_ref->load(child))
        {
            if(!object_refs.insert(make_pair(object_ref->id, object_ref)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Mainline::Key loaded an object_ref with a duplicate id (%d).\n", object_ref->id);
                delete object_ref;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Mainline::Key failed to load an object_ref.\n");
            delete object_ref;
        }
    }
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("time=%d\n", time);
    
    if(recursive_depth == 0)
        return;
    
    hierarchy.log(recursive_depth - 1);
    
    for(map<int, Object*>::const_iterator e = objects.begin(); e != objects.end(); e++)
    {
        SCML::log("Object:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<int, Object_Ref*>::const_iterator e = object_refs.begin(); e != object_refs.end(); e++)
    {
        SCML::log("Object_Ref:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Mainline::Key::clear()
{
    id = 0;
    time = 0;
    
    hierarchy.clear();
    
    for(map<int, Object*>::iterator e = objects.begin(); e != objects.end(); e++)
    {
        delete e->second;
    }
    objects.clear();
    
    for(map<int, Object_Ref*>::iterator e = object_refs.begin(); e != object_refs.end(); e++)
    {
        delete e->second;
    }
    object_refs.clear();
}








Data::Entity::Animation::Mainline::Key::Hierarchy::Hierarchy()
{}

Data::Entity::Animation::Mainline::Key::Hierarchy::Hierarchy(TiXmlElement* elem)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Hierarchy::load(TiXmlElement* elem)
{
    for(TiXmlElement* child = elem->FirstChildElement("bone"); child != NULL; child = child->NextSiblingElement("bone"))
    {
        Bone* bone = new Bone;
        if(bone->load(child))
        {
            if(!bones.insert(make_pair(bone->id, bone)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Mainline::Key::Hierarchy loaded a bone with a duplicate id (%d).\n", bone->id);
                delete bone;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Mainline::Key::Hierarchy failed to load a bone.\n");
            delete bone;
        }
    }
    
    for(TiXmlElement* child = elem->FirstChildElement("bone_ref"); child != NULL; child = child->NextSiblingElement("bone_ref"))
    {
        Bone_Ref* bone_ref = new Bone_Ref;
        if(bone_ref->load(child))
        {
            if(!bone_refs.insert(make_pair(bone_ref->id, bone_ref)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Mainline::Key::Hierarchy loaded a bone_ref with a duplicate id (%d).\n", bone_ref->id);
                delete bone_ref;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Mainline::Key::Hierarchy failed to load a bone_ref.\n");
            delete bone_ref;
        }
    }
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::log(int recursive_depth) const
{
    if(recursive_depth == 0)
        return;
        
    for(map<int, Bone*>::const_iterator e = bones.begin(); e != bones.end(); e++)
    {
        SCML::log("Bone:\n");
        e->second->log(recursive_depth - 1);
    }
    
    for(map<int, Bone_Ref*>::const_iterator e = bone_refs.begin(); e != bone_refs.end(); e++)
    {
        SCML::log("Bone_Ref:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::clear()
{
    for(map<int, Bone*>::iterator e = bones.begin(); e != bones.end(); e++)
    {
        delete e->second;
    }
    bones.clear();
    for(map<int, Bone_Ref*>::iterator e = bone_refs.begin(); e != bone_refs.end(); e++)
    {
        delete e->second;
    }
    bone_refs.clear();
}








Data::Entity::Animation::Mainline::Key::Hierarchy::Bone::Bone()
    : id(0), parent(0), x(0.0f), y(0.0f), angle(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}

Data::Entity::Animation::Mainline::Key::Hierarchy::Bone::Bone(TiXmlElement* elem)
    : id(0), parent(0), x(0.0f), y(0.0f), angle(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Hierarchy::Bone::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    parent = xmlGetIntAttr(elem, "parent", 0);
    x = xmlGetFloatAttr(elem, "x", 0.0f);
    y = xmlGetFloatAttr(elem, "y", 0.0f);
    angle = xmlGetFloatAttr(elem, "angle", 0.0f);
    scale_x = xmlGetFloatAttr(elem, "scale_x", 1.0f);
    scale_y = xmlGetFloatAttr(elem, "scale_y", 1.0f);
    r = xmlGetFloatAttr(elem, "r", 1.0f);
    g = xmlGetFloatAttr(elem, "g", 1.0f);
    b = xmlGetFloatAttr(elem, "b", 1.0f);
    a = xmlGetFloatAttr(elem, "a", 1.0f);
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::Bone::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("parent=%d\n", parent);
    SCML::log("x=%f\n", x);
    SCML::log("y=%f\n", y);
    SCML::log("angle=%f\n", angle);
    SCML::log("scale_x=%f\n", scale_x);
    SCML::log("scale_y=%f\n", scale_y);
    SCML::log("r=%f\n", r);
    SCML::log("g=%f\n", g);
    SCML::log("b=%f\n", b);
    SCML::log("a=%f\n", a);
    
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::Bone::clear()
{
    id = 0;
    parent = 0;
    x = 0.0f;
    y = 0.0f;
    angle = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
}








Data::Entity::Animation::Mainline::Key::Hierarchy::Bone_Ref::Bone_Ref()
    : id(0), parent(0), timeline(0), key(0)
{}

Data::Entity::Animation::Mainline::Key::Hierarchy::Bone_Ref::Bone_Ref(TiXmlElement* elem)
    : id(0), parent(0), timeline(0), key(0)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Hierarchy::Bone_Ref::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    parent = xmlGetIntAttr(elem, "parent", 0);
    timeline = xmlGetIntAttr(elem, "timeline", 0);
    key = xmlGetIntAttr(elem, "key", 0);
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::Bone_Ref::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("parent=%d\n", parent);
    SCML::log("timeline=%d\n", timeline);
    SCML::log("key=%d\n", key);
    
}

void Data::Entity::Animation::Mainline::Key::Hierarchy::Bone_Ref::clear()
{
    id = 0;
    parent = 0;
    timeline = 0;
    key = 0;
}








Data::Entity::Animation::Mainline::Key::Object::Object()
    : id(0), parent(0), object_type("sprite"), atlas(0), folder(0), file(0), usage("display"), blend_mode("alpha"), x(0.0f), y(0.0f), pivot_x(0.0f), pivot_y(1.0f), pixel_art_mode_x(0), pixel_art_mode_y(0), pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0), angle(0.0f), w(0.0f), h(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f), variable_type("string"), value_int(0), min_int(0), max_int(0), value_float(0.0f), min_float(0.0f), max_float(0.0f), animation(0), t(0.0f), z_index(0), volume(1.0f), panning(0.0f)
{}

Data::Entity::Animation::Mainline::Key::Object::Object(TiXmlElement* elem)
    : id(0), parent(0), object_type("sprite"), atlas(0), folder(0), file(0), usage("display"), blend_mode("alpha"), x(0.0f), y(0.0f), pivot_x(0.0f), pivot_y(1.0f), pixel_art_mode_x(0), pixel_art_mode_y(0), pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0), angle(0.0f), w(0.0f), h(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f), variable_type("string"), value_int(0), min_int(0), max_int(0), value_float(0.0f), min_float(0.0f), max_float(0.0f), animation(0), t(0.0f), z_index(0), volume(1.0f), panning(0.0f)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Object::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    parent = xmlGetIntAttr(elem, "parent", 0);
    object_type = xmlGetStringAttr(elem, "object_type", "sprite");
    atlas = xmlGetIntAttr(elem, "atlas", 0);
    folder = xmlGetIntAttr(elem, "folder", 0);
    file = xmlGetIntAttr(elem, "file", 0);
    usage = xmlGetStringAttr(elem, "usage", "display");
    blend_mode = xmlGetStringAttr(elem, "blend_mode", "alpha");
    x = xmlGetFloatAttr(elem, "x", 0.0f);
    y = xmlGetFloatAttr(elem, "y", 0.0f);
    pivot_x = xmlGetFloatAttr(elem, "pivot_x", 0.0f);
    pivot_y = xmlGetFloatAttr(elem, "pivot_y", 1.0f);
    pixel_art_mode_x = xmlGetIntAttr(elem, "x", 0);
    pixel_art_mode_y = xmlGetIntAttr(elem, "y", 0);
    pixel_art_mode_pivot_x = xmlGetIntAttr(elem, "pivot_x", 0);
    pixel_art_mode_pivot_y = xmlGetIntAttr(elem, "pivot_y", 0);
    angle = xmlGetFloatAttr(elem, "angle", 0.0f);
    w = xmlGetFloatAttr(elem, "w", 0.0f);
    h = xmlGetFloatAttr(elem, "h", 0.0f);
    scale_x = xmlGetFloatAttr(elem, "scale_x", 1.0f);
    scale_y = xmlGetFloatAttr(elem, "scale_y", 1.0f);
    r = xmlGetFloatAttr(elem, "r", 1.0f);
    g = xmlGetFloatAttr(elem, "g", 1.0f);
    b = xmlGetFloatAttr(elem, "b", 1.0f);
    a = xmlGetFloatAttr(elem, "a", 1.0f);
    variable_type = xmlGetStringAttr(elem, "variable_type", "string");
    if(variable_type == "string")
    {
        value_string = xmlGetStringAttr(elem, "value", "");
    }
    else if(variable_type == "int")
    {
        value_int = xmlGetIntAttr(elem, "value", 0);
        min_int = xmlGetIntAttr(elem, "min", 0);
        max_int = xmlGetIntAttr(elem, "max", 0);
    }
    else if(variable_type == "float")
    {
        value_float = xmlGetFloatAttr(elem, "value", 0.0f);
        min_float = xmlGetFloatAttr(elem, "min", 0.0f);
        max_float = xmlGetFloatAttr(elem, "max", 0.0f);
    }
    animation = xmlGetIntAttr(elem, "animation", 0);
    t = xmlGetFloatAttr(elem, "t", 0.0f);
    z_index = xmlGetIntAttr(elem, "z_index", 0);
    if(object_type == "sound")
    {
        volume = xmlGetFloatAttr(elem, "volume", 1.0f);
        panning = xmlGetFloatAttr(elem, "panning", 0.0f);
    }
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::Object::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("parent=%d\n", parent);
    SCML::log("object_type=%s\n", object_type.c_str());
    SCML::log("atlas=%d\n", atlas);
    SCML::log("folder=%d\n", folder);
    SCML::log("file=%d\n", file);
    SCML::log("usage=%s\n", usage.c_str());
    SCML::log("blend_mode=%s\n", blend_mode.c_str());
    SCML::log("x=%f\n", x);
    SCML::log("y=%f\n", y);
    SCML::log("pivot_x=%f\n", pivot_x);
    SCML::log("pivot_y=%f\n", pivot_y);
    SCML::log("pixel_art_mode_x=%d\n", pixel_art_mode_x);
    SCML::log("pixel_art_mode_y=%d\n", pixel_art_mode_y);
    SCML::log("pixel_art_mode_pivot_x=%d\n", pixel_art_mode_pivot_x);
    SCML::log("pixel_art_mode_pivot_y=%d\n", pixel_art_mode_pivot_y);
    SCML::log("angle=%f\n", angle);
    SCML::log("w=%f\n", w);
    SCML::log("h=%f\n", h);
    SCML::log("scale_x=%f\n", scale_x);
    SCML::log("scale_y=%f\n", scale_y);
    SCML::log("r=%f\n", r);
    SCML::log("g=%f\n", g);
    SCML::log("b=%f\n", b);
    SCML::log("a=%f\n", a);
    SCML::log("variable_type=%s\n", variable_type.c_str());
    if(variable_type == "string")
    {
        SCML::log("value=%s\n", value_string.c_str());
    }
    else if(variable_type == "int")
    {
        SCML::log("value=%d\n", value_int);
        SCML::log("min=%d\n", min_int);
        SCML::log("max=%d\n", max_int);
    }
    else if(variable_type == "float")
    {
        SCML::log("value=%f\n", value_float);
        SCML::log("min=%f\n", min_float);
        SCML::log("max=%f\n", max_float);
    }
    SCML::log("animation=%d\n", animation);
    SCML::log("t=%f\n", t);
    SCML::log("z_index=%d\n", z_index);
    if(object_type == "sound")
    {
        SCML::log("volume=%f\n", volume);
        SCML::log("panning=%f\n", panning);
    }
    // TODO: Remove stuff for object_types that don't need them
}

void Data::Entity::Animation::Mainline::Key::Object::clear()
{
    id = 0;
    parent = 0;
    object_type = "sprite";
    atlas = 0;
    folder = 0;
    file = 0;
    usage = "display";
    blend_mode = "alpha";
    x = 0.0f;
    y = 0.0f;
    pivot_x = 0.0f;
    pivot_y = 0.0f;
    pixel_art_mode_x = 0;
    pixel_art_mode_y = 0;
    pixel_art_mode_pivot_x = 0;
    pixel_art_mode_pivot_y = 0;
    angle = 0.0f;
    w = 0.0f;
    h = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
    variable_type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
    min_int = 0;
    min_float = 0.0f;
    max_int = 0;
    max_float = 0.0f;
    animation = 0;
    t = 0.0f;
    z_index = 0;
    volume = 1.0f;
    panning = 0.0f;
}








Data::Entity::Animation::Mainline::Key::Object_Ref::Object_Ref()
    : id(0), parent(0), timeline(0), key(0), z_index(0)
{}

Data::Entity::Animation::Mainline::Key::Object_Ref::Object_Ref(TiXmlElement* elem)
    : id(0), parent(0), timeline(0), key(0), z_index(0)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Object_Ref::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    parent = xmlGetIntAttr(elem, "parent", 0);
    timeline = xmlGetIntAttr(elem, "timeline", 0);
    key = xmlGetIntAttr(elem, "key", 0);
    z_index = xmlGetIntAttr(elem, "z_index", 0);
    
    return true;
}

void Data::Entity::Animation::Mainline::Key::Object_Ref::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("parent=%d\n", parent);
    SCML::log("timeline=%d\n", timeline);
    SCML::log("key=%d\n", key);
    SCML::log("z_index=%d\n", z_index);
    
}

void Data::Entity::Animation::Mainline::Key::Object_Ref::clear()
{
    id = 0;
    parent = 0;
    timeline = 0;
    key = 0;
    z_index = 0;
}









Data::Entity::Animation::Timeline::Timeline()
    : id(0), object_type("sprite"), variable_type("string"), usage("display")
{}

Data::Entity::Animation::Timeline::Timeline(TiXmlElement* elem)
    : id(0), object_type("sprite"), variable_type("string"), usage("display")
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    object_type = xmlGetStringAttr(elem, "object_type", "sprite");
    variable_type = xmlGetStringAttr(elem, "variable_type", "string");
    
    if(object_type != "sound")
        name = xmlGetStringAttr(elem, "name", "");
    
    if(object_type == "point")
        usage = xmlGetStringAttr(elem, "usage", "neither");
    else if(object_type == "box")
        usage = xmlGetStringAttr(elem, "usage", "collision");
    else if(object_type == "sprite")
        usage = xmlGetStringAttr(elem, "usage", "display");
    else if(object_type == "entity")
        usage = xmlGetStringAttr(elem, "usage", "display");
    
    for(TiXmlElement* child = elem->FirstChildElement("key"); child != NULL; child = child->NextSiblingElement("key"))
    {
        Key* key = new Key;
        if(key->load(child))
        {
            if(!keys.insert(make_pair(key->id, key)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Timeline loaded a key with a duplicate id (%d).\n", key->id);
                delete key;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Timeline failed to load a key.\n");
            delete key;
        }
    }
    
    return true;
}

void Data::Entity::Animation::Timeline::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    SCML::log("object_type=%s\n", object_type.c_str());
    SCML::log("variable_type=%s\n", variable_type.c_str());
    SCML::log("usage=%s\n", usage.c_str());
    
    if(recursive_depth == 0)
        return;
    
    for(map<int, Key*>::const_iterator e = keys.begin(); e != keys.end(); e++)
    {
        SCML::log("Key:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Timeline::clear()
{
    id = 0;
    name.clear();
    object_type = "sprite";
    variable_type = "string";
    usage = "display";
    
    for(map<int, Key*>::iterator e = keys.begin(); e != keys.end(); e++)
    {
        delete e->second;
    }
    keys.clear();
}







Data::Entity::Animation::Timeline::Key::Key()
    : id(0), time(0), curve_type("linear"), c1(0.0f), c2(0.0f), spin(1)
{}

Data::Entity::Animation::Timeline::Key::Key(TiXmlElement* elem)
    : id(0), time(0), curve_type("linear"), c1(0.0f), c2(0.0f), spin(1)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    time = xmlGetIntAttr(elem, "time", 0);
    curve_type = xmlGetStringAttr(elem, "curve_type", "linear");
    c1 = xmlGetFloatAttr(elem, "c1", 0.0f);
    c2 = xmlGetFloatAttr(elem, "c2", 0.0f);
    spin = xmlGetIntAttr(elem, "spin", 1);
    
    
    TiXmlElement* child = elem->FirstChildElement("meta_data");
    if(child != NULL)
        meta_data.load(child);
    
    has_object = true;
    
    child = elem->FirstChildElement("bone");
    if(child != NULL && !bone.load(child))
    {
        SCML::log("SCML::Data::Entity::Animation::Timeline::Key failed to load a bone.\n");
        has_object = false;
    }
        
    child = elem->FirstChildElement("object");
    if(child != NULL && !object.load(child))
    {
        SCML::log("SCML::Data::Entity::Animation::Timeline::Key failed to load an object.\n");
        has_object = true;
    }
    
    return true;
}

void Data::Entity::Animation::Timeline::Key::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("time=%d\n", time);
    SCML::log("curve_type=%s\n", curve_type.c_str());
    SCML::log("c1=%f\n", c1);
    SCML::log("c2=%f\n", c2);
    SCML::log("spin=%d\n", spin);
    
    if(recursive_depth == 0)
        return;
    
    SCML::log("Meta_Data:\n");
    meta_data.log(recursive_depth - 1);
    if(has_object)
    {
        SCML::log("Object:\n");
        object.log(recursive_depth - 1);
    }
    else
    {
        SCML::log("Bone:\n");
        bone.log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Timeline::Key::clear()
{
    id = 0;
    time = 0;
    curve_type = "linear";
    c1 = 0.0f;
    c2 = 0.0f;
    spin = 1;
    
    meta_data.clear();
    bone.clear();
    object.clear();
}








Data::Entity::Animation::Timeline::Key::Meta_Data::Meta_Data()
{}

Data::Entity::Animation::Timeline::Key::Meta_Data::Meta_Data(TiXmlElement* elem)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Meta_Data::load(TiXmlElement* elem)
{
    for(TiXmlElement* child = elem->FirstChildElement("variable"); child != NULL; child = child->NextSiblingElement("variable"))
    {
        Variable* variable = new Variable;
        if(variable->load(child))
        {
            if(!variables.insert(make_pair(variable->name, variable)).second)
            {
                SCML::log("SCML::Data::Entity::Animation::Timeline::Key::Meta_Data loaded a variable with a duplicate name (%s).\n", variable->name.c_str());
                delete variable;
            }
        }
        else
        {
            SCML::log("SCML::Data::Entity::Animation::Timeline::Key::Meta_Data failed to load a variable.\n");
            delete variable;
        }
    }
    
    return true;
}

void Data::Entity::Animation::Timeline::Key::Meta_Data::log(int recursive_depth) const
{
    if(recursive_depth == 0)
        return;
        
    for(map<std::string, Variable*>::const_iterator e = variables.begin(); e != variables.end(); e++)
    {
        SCML::log("Variable:\n");
        e->second->log(recursive_depth - 1);
    }
    
}

void Data::Entity::Animation::Timeline::Key::Meta_Data::clear()
{
    for(map<std::string, Variable*>::iterator e = variables.begin(); e != variables.end(); e++)
    {
        delete e->second;
    }
    variables.clear();
}








Data::Entity::Animation::Timeline::Key::Meta_Data::Variable::Variable()
    : type("string"), value_int(0), value_float(0.0f), curve_type("linear"), c1(0.0f), c2(0.0f)
{}

Data::Entity::Animation::Timeline::Key::Meta_Data::Variable::Variable(TiXmlElement* elem)
    : type("string"), value_int(0), value_float(0.0f), curve_type("linear"), c1(0.0f), c2(0.0f)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Meta_Data::Variable::load(TiXmlElement* elem)
{
    type = xmlGetStringAttr(elem, "type", "string");
    if(type == "string")
        value_string = xmlGetStringAttr(elem, "value", "");
    else if(type == "int")
        value_int = xmlGetIntAttr(elem, "value", 0);
    else if(type == "float")
        value_float = xmlGetFloatAttr(elem, "value", 0.0f);
    
    curve_type = xmlGetStringAttr(elem, "curve_type", "linear");
    c1 = xmlGetFloatAttr(elem, "c1", 0.0f);
    c2 = xmlGetFloatAttr(elem, "c2", 0.0f);
    
    return true;
}

void Data::Entity::Animation::Timeline::Key::Meta_Data::Variable::log(int recursive_depth) const
{
    SCML::log("type=%s\n", type.c_str());
    if(type == "string")
        SCML::log("value=%s\n", value_string.c_str());
    else if(type == "int")
        SCML::log("value=%d\n", value_int);
    else if(type == "float")
        SCML::log("value=%f\n", value_float);
        
    SCML::log("curve_type=%s\n", curve_type.c_str());
    SCML::log("c1=%f\n", c1);
    SCML::log("c2=%f\n", c2);
    
}

void Data::Entity::Animation::Timeline::Key::Meta_Data::Variable::clear()
{
    type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
    
    curve_type = "linear";
    c1 = 0.0f;
    c2 = 0.0f;
}








Data::Entity::Animation::Timeline::Key::Bone::Bone()
    : x(0.0f), y(0.0f), angle(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}

Data::Entity::Animation::Timeline::Key::Bone::Bone(TiXmlElement* elem)
    : x(0.0f), y(0.0f), angle(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Bone::load(TiXmlElement* elem)
{
    x = xmlGetFloatAttr(elem, "x", 0.0f);
    y = xmlGetFloatAttr(elem, "y", 0.0f);
    angle = xmlGetFloatAttr(elem, "angle", 0.0f);
    scale_x = xmlGetFloatAttr(elem, "scale_x", 1.0f);
    scale_y = xmlGetFloatAttr(elem, "scale_y", 1.0f);
    r = xmlGetFloatAttr(elem, "r", 1.0f);
    g = xmlGetFloatAttr(elem, "g", 1.0f);
    b = xmlGetFloatAttr(elem, "b", 1.0f);
    a = xmlGetFloatAttr(elem, "a", 1.0f);
    
    return true;
}

void Data::Entity::Animation::Timeline::Key::Bone::log(int recursive_depth) const
{
    SCML::log("x=%f\n", x);
    SCML::log("y=%f\n", y);
    SCML::log("angle=%f\n", angle);
    SCML::log("scale_x=%f\n", scale_x);
    SCML::log("scale_y=%f\n", scale_y);
    SCML::log("r=%f\n", r);
    SCML::log("g=%f\n", g);
    SCML::log("b=%f\n", b);
    SCML::log("a=%f\n", a);
    
}

void Data::Entity::Animation::Timeline::Key::Bone::clear()
{
    x = 0.0f;
    y = 0.0f;
    angle = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
}








Data::Entity::Animation::Timeline::Key::Object::Object()
    : atlas(0), folder(0), file(0), x(0.0f), y(0.0f), pivot_x(0.0f), pivot_y(1.0f), angle(0.0f), w(0.0f), h(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f), blend_mode("alpha"), value_int(0), min_int(0), max_int(0), value_float(0.0f), min_float(0.0f), max_float(0.0f), t(0.0f), volume(1.0f), panning(0.0f)
{}

Data::Entity::Animation::Timeline::Key::Object::Object(TiXmlElement* elem)
    : atlas(0), folder(0), file(0), x(0.0f), y(0.0f), pivot_x(0.0f), pivot_y(1.0f), angle(0.0f), w(0.0f), h(0.0f), scale_x(1.0f), scale_y(1.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f), blend_mode("alpha"), value_int(0), min_int(0), max_int(0), value_float(0.0f), min_float(0.0f), max_float(0.0f), t(0.0f), volume(1.0f), panning(0.0f)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Object::load(TiXmlElement* elem)
{
    //object_type = xmlGetStringAttr(elem, "object_type", "sprite");
    atlas = xmlGetIntAttr(elem, "atlas", 0);
    folder = xmlGetIntAttr(elem, "folder", 0);
    file = xmlGetIntAttr(elem, "file", 0);
    //usage = xmlGetStringAttr(elem, "usage", "display");
    x = xmlGetFloatAttr(elem, "x", 0.0f);
    y = xmlGetFloatAttr(elem, "y", 0.0f);
    pivot_x = xmlGetFloatAttr(elem, "pivot_x", 0.0f);
    pivot_y = xmlGetFloatAttr(elem, "pivot_y", 1.0f);
    angle = xmlGetFloatAttr(elem, "angle", 0.0f);
    w = xmlGetFloatAttr(elem, "w", 0.0f);
    h = xmlGetFloatAttr(elem, "h", 0.0f);
    scale_x = xmlGetFloatAttr(elem, "scale_x", 1.0f);
    scale_y = xmlGetFloatAttr(elem, "scale_y", 1.0f);
    r = xmlGetFloatAttr(elem, "r", 1.0f);
    g = xmlGetFloatAttr(elem, "g", 1.0f);
    b = xmlGetFloatAttr(elem, "b", 1.0f);
    a = xmlGetFloatAttr(elem, "a", 1.0f);
    blend_mode = xmlGetStringAttr(elem, "blend_mode", "alpha");
    //variable_type = xmlGetStringAttr(elem, "variable_type", "string");
    //if(variable_type == "string")
    {
        value_string = xmlGetStringAttr(elem, "value", "");
    }
    //else if(variable_type == "int")
    {
        value_int = xmlGetIntAttr(elem, "value", 0);
        min_int = xmlGetIntAttr(elem, "min", 0);
        max_int = xmlGetIntAttr(elem, "max", 0);
    }
    //else if(variable_type == "float")
    {
        value_float = xmlGetFloatAttr(elem, "value", 0.0f);
        min_float = xmlGetFloatAttr(elem, "min", 0.0f);
        max_float = xmlGetFloatAttr(elem, "max", 0.0f);
    }
    //animation = xmlGetIntAttr(elem, "animation", 0);
    animation = xmlGetStringAttr(elem, "animation", "");
    t = xmlGetFloatAttr(elem, "t", 0.0f);
    //if(object_type == "sound")
    {
        volume = xmlGetFloatAttr(elem, "volume", 1.0f);
        panning = xmlGetFloatAttr(elem, "panning", 0.0f);
    }
    
    return true;
}

void Data::Entity::Animation::Timeline::Key::Object::log(int recursive_depth) const
{
    //SCML::log("object_type=%s\n", object_type.c_str());
    SCML::log("atlas=%d\n", atlas);
    SCML::log("folder=%d\n", folder);
    SCML::log("file=%d\n", file);
    //SCML::log("usage=%s\n", usage.c_str());
    SCML::log("x=%f\n", x);
    SCML::log("y=%f\n", y);
    SCML::log("pivot_x=%f\n", pivot_x);
    SCML::log("pivot_y=%f\n", pivot_y);
    SCML::log("angle=%f\n", angle);
    SCML::log("w=%f\n", w);
    SCML::log("h=%f\n", h);
    SCML::log("scale_x=%f\n", scale_x);
    SCML::log("scale_y=%f\n", scale_y);
    SCML::log("r=%f\n", r);
    SCML::log("g=%f\n", g);
    SCML::log("b=%f\n", b);
    SCML::log("a=%f\n", a);
    SCML::log("blend_mode=%s\n", blend_mode.c_str());
    //SCML::log("variable_type=%s\n", variable_type.c_str());
    //if(variable_type == "string")
    {
        SCML::log("value=%s\n", value_string.c_str());
    }
    /*else if(variable_type == "int")
    {
        SCML::log("value=%d\n", value_int);
        SCML::log("min=%d\n", min_int);
        SCML::log("max=%d\n", max_int);
    }
    else if(variable_type == "float")
    {
        SCML::log("value=%f\n", value_float);
        SCML::log("min=%f\n", min_float);
        SCML::log("max=%f\n", max_float);
    }*/
    SCML::log("animation=%s\n", animation.c_str());
    SCML::log("t=%f\n", t);
    //if(object_type == "sound")
    {
        SCML::log("volume=%f\n", volume);
        SCML::log("panning=%f\n", panning);
    }
    // TODO: Remove stuff for object_types that don't need them
}

void Data::Entity::Animation::Timeline::Key::Object::clear()
{
    //object_type = "sprite";
    atlas = 0;
    folder = 0;
    file = 0;
    //usage = "display";
    x = 0.0f;
    y = 0.0f;
    pivot_x = 0.0f;
    pivot_y = 0.0f;
    angle = 0.0f;
    w = 0.0f;
    h = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
    blend_mode = "alpha";
    //variable_type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
    min_int = 0;
    min_float = 0.0f;
    max_int = 0;
    max_float = 0.0f;
    animation.clear();
    t = 0.0f;
    volume = 1.0f;
    panning = 0.0f;
}









Data::Character_Map::Character_Map()
    : id(0)
{}

Data::Character_Map::Character_Map(TiXmlElement* elem)
    : id(0)
{
    load(elem);
}

bool Data::Character_Map::load(TiXmlElement* elem)
{
    id = xmlGetIntAttr(elem, "id", 0);
    name = xmlGetStringAttr(elem, "name", "");
    
    TiXmlElement* child = elem->FirstChildElement("map");
    if(child == NULL || !map.load(child))
    {
        SCML::log("SCML::Data::Entity failed to load an animation.\n");
    }
    
    return true;
}

void Data::Character_Map::log(int recursive_depth) const
{
    SCML::log("id=%d\n", id);
    SCML::log("name=%s\n", name.c_str());
    
    if(recursive_depth == 0)
        return;
    
    SCML::log("Map:\n");
    map.log(recursive_depth - 1);
    
}

void Data::Character_Map::clear()
{
    id = 0;
    name.clear();
    
    map.clear();
}







Data::Character_Map::Map::Map()
    : atlas(0), folder(0), file(0), target_atlas(0), target_folder(0), target_file(0)
{}

Data::Character_Map::Map::Map(TiXmlElement* elem)
    : atlas(0), folder(0), file(0), target_atlas(0), target_folder(0), target_file(0)
{
    load(elem);
}

bool Data::Character_Map::Map::load(TiXmlElement* elem)
{
    atlas = xmlGetIntAttr(elem, "atlas", 0);
    folder = xmlGetIntAttr(elem, "folder", 0);
    file = xmlGetIntAttr(elem, "file", 0);
    target_atlas = xmlGetIntAttr(elem, "target_atlas", 0);
    target_folder = xmlGetIntAttr(elem, "target_folder", 0);
    target_file = xmlGetIntAttr(elem, "target_file", 0);
    
    return true;
}

void Data::Character_Map::Map::log(int recursive_depth) const
{
    SCML::log("atlas=%d\n", atlas);
    SCML::log("folder=%d\n", folder);
    SCML::log("file=%d\n", file);
    SCML::log("target_atlas=%d\n", target_atlas);
    SCML::log("target_folder=%d\n", target_folder);
    SCML::log("target_file=%d\n", target_file);
}

void Data::Character_Map::Map::clear()
{
    atlas = 0;
    folder = 0;
    file = 0;
    target_atlas = 0;
    target_folder = 0;
    target_file = 0;
}










Data::Document_Info::Document_Info()
    : author("author not specified"), copyright("copyright info not specified"), license("no license specified"), version("version not specified"), last_modified("date and time not included"), notes("no additional notes")
{}

Data::Document_Info::Document_Info(TiXmlElement* elem)
    : author("author not specified"), copyright("copyright info not specified"), license("no license specified"), version("version not specified"), last_modified("date and time not included"), notes("no additional notes")
{
    load(elem);
}

bool Data::Document_Info::load(TiXmlElement* elem)
{
    author = xmlGetStringAttr(elem, "author", "author not specified");
    copyright = xmlGetStringAttr(elem, "copyright", "copyright info not specified");
    license = xmlGetStringAttr(elem, "license", "no license specified");
    version = xmlGetStringAttr(elem, "version", "version not specified");
    last_modified = xmlGetStringAttr(elem, "last_modified", "date and time not included");
    notes = xmlGetStringAttr(elem, "notes", "no additional notes");
    
    return true;
}

void Data::Document_Info::log(int recursive_depth) const
{
    SCML::log("author=%s\n", author.c_str());
    SCML::log("copyright=%s\n", copyright.c_str());
    SCML::log("license=%s\n", license.c_str());
    SCML::log("version=%s\n", version.c_str());
    SCML::log("last_modified=%s\n", last_modified.c_str());
    SCML::log("notes=%s\n", notes.c_str());
}

void Data::Document_Info::clear()
{
    author = "author not specified";
    copyright = "copyright info not specified";
    license = "no license specified";
    version = "version not specified";
    last_modified = "date and time not included";
    notes = "no additional notes";
}




}
