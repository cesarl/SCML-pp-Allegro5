#include "SDL_Helpers.h"
#include "goodio.h"
#include <cmath>
#include <vector>
#include <iostream>

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

using namespace std;


std::string toString(bool b)
{
    return (b? "true" : "false");
}

std::string toString(unsigned int n)
{
    char buf[20];
    sprintf(buf, "%u", n);
    return buf;
}

std::string toString(int n)
{
    char buf[20];
    sprintf(buf, "%d", n);
    return buf;
}

char* stripTrailingDecimalZeros(char* buf)
{
    if(buf == NULL)
        return NULL;
    
    char* c = buf;
    
    // See if it has a decimal
    bool hasDecimal = false;
    while(*c != '\0')
    {
        if(*c == '.')
            hasDecimal = true;
        c++;
    }
    
    // If there is a decimal part, remove all the trailing zeros and the decimal point if we reach it.
    if(hasDecimal)
    {
        while(c != buf)
        {
            c--;
            if(*c == '0')
                *c = '\0';
            else
            {
                if(*c == '.')
                    *c = '\0';
                break;
            }
        }
    }
    
    return buf;
}

std::string toString(float f, int precision)
{
    char buf[20];
    if(precision >= 0)
        snprintf(buf, 20, "%.*f", precision, f);
    else
    {
        snprintf(buf, 20, "%f", f);
        stripTrailingDecimalZeros(buf);
    }
    return buf;
}

/*
std::string toString(float f, int precision, bool scientific)
{
    char buf[20];
    if(precision >= 0)
        snprintf(buf, 20, scientific? "%.*g" : "%.*f", precision, f);
    else
        snprintf(buf, 20, scientific? "%g" : "%f", f);
    return buf;
}
*/


Pointf Pointf::rotate(float radians) const
{
    float s = sin(radians);
    float c = cos(radians);
    return Pointf(x*c - y*s, x*s + y*c);
}

std::string Point::to_string() const
{
    return "(" + toString(x) + "," + toString(y) + ")";
}

std::string Pointf::to_string() const
{
    return "(" + toString(x) + "," + toString(y) + ")";
}



Rect::Rect()
    : x(0), y(0), w(0), h(0)
{}

Rect::Rect(Sint16 x, Sint16 y)
    : x(x), y(y), w(0), h(0)
{}

Rect::Rect(Sint16 x, Sint16 y, Uint16 w, Uint16 h)
    : x(x), y(y), w(w), h(h)
{}

Rect::Rect(const SDL_Rect& rect)
    : x(rect.x), y(rect.y), w(rect.w), h(rect.h)
{}

Rect::Rect(const Point& p1, const Point& p2)
    : x(0), y(0), w(0), h(0)
{
    x = MIN(p1.x, p2.x);
    y = MIN(p1.y, p2.y);
    w = fabs(p2.x - p1.x);
    h = fabs(p2.y - p1.y);
}

SDL_Rect Rect::to_SDL_Rect() const
{
    SDL_Rect r = {x, y, w, h};
    return r;
}

std::string Rect::to_string() const
{
    return "(" + toString(x) + "," + toString(y) + "," + toString(w) + "," + toString(h) + ")";
}

bool Rect::contains(int X, int Y) const
{
    return (x <= X && x + w >= X && y <= Y && y + h >= Y);
}

bool Rect::contains(const Point& p) const
{
    return (x <= p.x && x + w >= p.x && y <= p.y && y + h >= p.y);
}

bool Rect::contains(const Pointf& p) const
{
    return (x <= p.x && x + w >= p.x && y <= p.y && y + h >= p.y);
}








Rectf::Rectf()
    : x(0), y(0), w(0), h(0)
{}

Rectf::Rectf(float x, float y)
    : x(x), y(y), w(0), h(0)
{}

Rectf::Rectf(float x, float y, float w, float h)
    : x(x), y(y), w(w), h(h)
{}

Rectf::Rectf(const SDL_Rect& rect)
    : x(rect.x), y(rect.y), w(rect.w), h(rect.h)
{}

Rectf::Rectf(const Pointf& p1, const Pointf& p2)
    : x(0), y(0), w(0), h(0)
{
    x = MIN(p1.x, p2.x);
    y = MIN(p1.y, p2.y);
    w = fabs(p2.x - p1.x);
    h = fabs(p2.y - p1.y);
}

SDL_Rect Rectf::to_SDL_Rect() const
{
    SDL_Rect r = {x, y, w, h};
    return r;
}

std::string Rectf::to_string() const
{
    return "(" + toString(x) + "," + toString(y) + "," + toString(w) + "," + toString(h) + ")";
}

bool Rectf::contains(float X, float Y) const
{
    if(w >= 0.0f && h >= 0.0f)
        return (x <= X && x + w >= X && y <= Y && y + h >= Y);
    if(w < 0.0f && h < 0.0f)
        return (x + w <= X && x >= X && y + h <= Y && y >= Y);
    if(w < 0.0f)
        return (x + w <= X && x >= X && y <= Y && y + h >= Y);
    return (x <= X && x + w >= X && y + h <= Y && y >= Y);
}




Color::Color()
    : r(0), g(0), b(0), a(255)
{}
Color::Color(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b), a(255)
{}
Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a)
{}

Color& Color::rgb(unsigned char r, unsigned char g, unsigned char b)
{
    this->r = r;
    this->g = g;
    this->b = b;

    return *this;
}

Color& Color::rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;

    return *this;
}


Color& Color::setR(unsigned char r)
{
    this->r = r;
    return *this;
}
Color Color::newR(unsigned char r) const
{
    return Color(r, g, b, a);
}

Color& Color::setG(unsigned char g)
{
    this->g = g;
    return *this;
}
Color Color::newG(unsigned char g) const
{
    return Color(r, g, b, a);
}

Color& Color::setB(unsigned char b)
{
    this->b = b;
    return *this;
}
Color Color::newB(unsigned char b) const
{
    return Color(r, g, b, a);
}

Color& Color::setA(unsigned char a)
{
    this->a = a;
    return *this;
}
Color Color::newA(unsigned char a) const
{
    return Color(r, g, b, a);
}

#define MIN3(a,b,c) ((a)<(b) && (a)<(c)? (a) : ((b)<(c)? (b) : (c)))
#define MAX3(a,b,c) ((a)>(b) && (a)>(c)? (a) : ((b)>(c)? (b) : (c)))

Color Color::rgb_to_hsv() const
{
	float r = this->r/255.0f;
	float g = this->g/255.0f;
	float b = this->b/255.0f;

	float h, s, v;

	float min, max, delta;
	min = MIN3( r, g, b );
	max = MAX3( r, g, b );

	v = max;				// v
	delta = max - min;
	if( max != 0 && min != max)
	{
		s = delta / max;		// s

		if( r == max )
			h = ( g - b ) / delta;		// between yellow & magenta
		else if( g == max )
			h = 2 + ( b - r ) / delta;	// between cyan & yellow
		else
			h = 4 + ( r - g ) / delta;	// between magenta & cyan
		h *= 60;				// degrees
		if( h < 0 )
			h += 360;
	}
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = 0;// really undefined: -1
	}

    return Color(h * 256.0f/360.0f, s * 255, v * 255, a);
}

Color Color::hsv_to_rgb() const
{
    float h = r/255.0f;
    float s = g/255.0f;
    float v = b/255.0f;

    int H = floor(h*5.999f);
    float chroma = v*s;
    float x = chroma * (1 - fabs(fmod(h*5.999f, 2) - 1));

    unsigned char R = 0, G = 0, B = 0;
    switch(H)
    {
        case 0:
            R = 255*chroma;
            G = 255*x;
        break;
        case 1:
            R = 255*x;
            G = 255*chroma;
        break;
        case 2:
            G = 255*chroma;
            B = 255*x;
        break;
        case 3:
            G = 255*x;
            B = 255*chroma;
        break;
        case 4:
            R = 255*x;
            B = 255*chroma;
        break;
        case 5:
            R = 255*chroma;
            B = 255*x;
        break;
    }

    unsigned char m = 255*(v - chroma);

    return Color(R+m, G+m, B+m, a);
}

Uint32 Color::to_format(SDL_PixelFormat* format) const
{
    return SDL_MapRGBA(format, r, g, b, a);
}

SDL_Color Color::to_SDL_Color() const
{
    SDL_Color c = {r, g, b, a};
    return c;
}

std::string Color::to_string() const
{
	if(a == 255)
        return "(" + toString(r) + "," + toString(g) + "," + toString(b) + ")";
    else
        return "(" + toString(r) + "," + toString(g) + "," + toString(b) + "," + toString(a) + ")";
}

bool Color::operator==(const Color& color) const
{
    return (r == color.r && g == color.g && b == color.b && a == color.a);
}

bool Color::operator!=(const Color& color) const
{
    return (r != color.r || g != color.g || b != color.b || a != color.a);
}












std::string toLower(const std::string& str)
{
    std::string result = str;
    for(unsigned int i = 0; i < result.size(); i++)
    {
        result[i] = tolower(result[i]);
    }
    return result;
}


int toInt(const std::string& str)
{
    return atoi(str.c_str());
}

bool toBool(const std::string& str)
{
    if(toLower(str) == "true")
        return true;
    if(toLower(str) == "false")
        return false;
    return atoi(str.c_str());
}

float toFloat(const std::string& str)
{
    return atof(str.c_str());
}


std::string getInsideParens(const std::string& s)
{
    size_t first = s.find_first_of("({");
    if(first > 0 || first == string::npos)
        first = 0;
    else
        first++;

    size_t last = s.find_last_of(")}");
    if(last < s.size()-1 || last == string::npos)
        last = s.size();

    return s.substr(first, last-1);
}


// Takes a string like (40, 50, 50, 60) and interprets it as (x, y, w, h) for a Rect
Rect toRect(const std::string& str)
{
    try
    {
        string s = getInsideParens(str);

        vector<string> ls = ioExplodev(s, ',');
        if(ls.size() != 4)
        {
            cerr << "toRect failed: Invalid number of parameters: " << ls.size() << endl;
            return Rect(0,0,0,0);
        }


        return Rect(atoi(ls[0].c_str()), atoi(ls[1].c_str()), atoi(ls[2].c_str()), atoi(ls[3].c_str()));
    }
    catch(exception& e)
    {
        cerr << "toRect failed: " << e.what() << endl;
        return Rect(0,0,0,0);
    }
}

Point toPoint(const string& str)
{
    try
    {
        string s = getInsideParens(str);

        vector<string> ls = ioExplodev(s, ',');
        if(ls.size() != 2)
        {
            cerr << "toPoint failed: Invalid number of parameters: " << ls.size() << endl;
            return Point(0,0);
        }

        return Point(atoi(ls[0].c_str()), atoi(ls[1].c_str()));
    }
    catch(exception& e)
    {
        cerr << "toPoint failed: " << e.what() << endl;
        return Point(0,0);
    }
}

Pointf toPointf(const string& str)
{
    try
    {
        string s = getInsideParens(str);

        vector<string> ls = ioExplodev(s, ',');
        if(ls.size() != 2)
        {
            cerr << "toPoint failed: Invalid number of parameters: " << ls.size() << endl;
            return Pointf(0,0);
        }

        return Pointf(atof(ls[0].c_str()), atof(ls[1].c_str()));
    }
    catch(exception& e)
    {
        cerr << "toPoint failed: " << e.what() << endl;
        return Pointf(0,0);
    }
}


Color toColor(const string& str)
{
    try
    {
        string s = getInsideParens(str);

        vector<string> ls = ioExplodev(s, ',');


        if(ls.size() == 3)
        {
            return Color(atoi(ls[0].c_str()), atoi(ls[1].c_str()), atoi(ls[2].c_str()));
        }
        if(ls.size() == 4)
        {
            return Color(atoi(ls[0].c_str()), atoi(ls[1].c_str()), atoi(ls[2].c_str()), atoi(ls[3].c_str()));
        }


        cerr << "toColor failed: Invalid number of parameters: " << ls.size() << endl;
        return Color(0,0,0);
    }
    catch(exception& e)
    {
        cerr << "toColor failed: " << e.what() << endl;
        return Color(0,0,0);
    }
}







std::string toString(const Pointf& p, int precision)
{
    char buf[101];
    snprintf(buf, 100, "(%s, %s)", toString(p.x, precision).c_str(), toString(p.y, precision).c_str());
    return buf;
}


