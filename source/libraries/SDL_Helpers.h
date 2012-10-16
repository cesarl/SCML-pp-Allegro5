#ifndef _SDL_HELPERS_H__
#define _SDL_HELPERS_H__

#include "SDL.h"
#include <string>


class Point
{
    public:
    
    int x, y;
    Point()
        : x(0), y(0)
    {}
    Point(int x, int y)
        : x(x), y(y)
    {}
    
    bool operator==(const Point& p) const
    {
        return (x == p.x && y == p.y);
    }
    bool operator!=(const Point& p) const
    {
        return (x != p.x || y != p.y);
    }
    Point& operator+=(const Point& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }
    Point& operator-=(const Point& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }
    Point operator+(const Point& p) const
    {
        return (Point(*this) += p);
    }
    Point operator-(const Point& p) const
    {
        return (Point(*this) -= p);
    }
    Point operator-() const
    {
        return Point(-x, -y);
    }
    
    std::string to_string() const;
};

class Pointf
{
    public:
    
    float x, y;
    Pointf()
        : x(0), y(0)
    {}
    Pointf(float x, float y)
        : x(x), y(y)
    {}
    
    Pointf rotate(float radians) const;
    
    
    bool operator==(const Pointf& p) const
    {
        return (x == p.x && y == p.y);
    }
    bool operator!=(const Pointf& p) const
    {
        return (x != p.x || y != p.y);
    }
    Pointf& operator+=(const Pointf& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }
    Pointf& operator-=(const Pointf& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }
    Pointf& operator*=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }
    Pointf operator+(const Pointf& p) const
    {
        return (Pointf(*this) += p);
    }
    Pointf operator-(const Pointf& p) const
    {
        return (Pointf(*this) -= p);
    }
    Pointf operator-() const
    {
        return Pointf(-x, -y);
    }
    Pointf operator*(float f) const
    {
        return Pointf(x*f, y*f);
    }
    
    std::string to_string() const;
};

inline Pointf operator*(float f, const Pointf& p)
{
    return Pointf(p.x*f, p.y*f);
}


class Rect
{
    public:
	Sint16 x, y;
	Uint16 w, h;
    
    Rect();
    Rect(Sint16 x, Sint16 y);
    Rect(Sint16 x, Sint16 y, Uint16 w, Uint16 h);
    Rect(const SDL_Rect& rect);
    Rect(const Point& p1, const Point& p2);
    
    SDL_Rect to_SDL_Rect() const;
    std::string to_string() const;
    
    bool contains(int X, int Y) const;
    bool contains(const Point& p) const;
    bool contains(const Pointf& p) const;
};

class Rectf
{
    public:
	float x, y;
	float w, h;
    
    Rectf();
    Rectf(float x, float y);
    Rectf(float x, float y, float w, float h);
    Rectf(const SDL_Rect& rect);
    Rectf(const Pointf& p1, const Pointf& p2);
    
    SDL_Rect to_SDL_Rect() const;
    std::string to_string() const;
    
    bool contains(float X, float Y) const;
};


class Color
{
    public:
    
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    
    Color();
    Color(unsigned char r, unsigned char g, unsigned char b);
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    
    Color& rgb(unsigned char r, unsigned char g, unsigned char b);
    Color& rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    
    Color& setR(unsigned char r);
    Color newR(unsigned char r) const;
    
    Color& setG(unsigned char g);
    Color newG(unsigned char g) const;
    
    Color& setB(unsigned char b);
    Color newB(unsigned char b) const;
    
    Color& setA(unsigned char a);
    Color newA(unsigned char a) const;
    
    Color rgb_to_hsv() const;
    Color hsv_to_rgb() const;
    
    Uint32 to_format(SDL_PixelFormat* format) const;
    SDL_Color to_SDL_Color() const;
    
    std::string to_string() const;
    
    bool operator==(const Color& color) const;
    bool operator!=(const Color& color) const;
};



std::string toString(bool b);
std::string toString(int n);
std::string toString(unsigned int n);

std::string toString(float f, int precision = -1);




bool toBool(const std::string& str);
int toInt(const std::string& str);
float toFloat(const std::string& str);
Rect toRect(const std::string& str);
Color toColor(const std::string& str);
Point toPoint(const std::string& str);
Pointf toPointf(const std::string& str);

std::string toString(const Pointf& p, int precision = -1);

std::string getInsideParens(const std::string& s);


#endif
