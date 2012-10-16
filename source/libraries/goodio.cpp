/*
goodio v1.1 - A small library for simple filesystem interaction.
by Jonathan Dearborn, 2009
grimfang4 [at] gmail [dot] com

See goodio.h for details and license.
*/

#include "goodio.h"

#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

#include <stdexcept> // std::runtime_error

#define IO_COPY_BUFFSIZE 512
#define IO_UNIQUE_MAX 10000


#if defined(WIN32) || defined(_WIN32)
//#define WIN32
#include "windows.h"
#include <time.h>
#endif


#if defined(linux) || defined(__linux) || defined(__linux__)
#define LINUX
#define IO_PATH_MAX PATH_MAX
#else
#define IO_PATH_MAX MAX_PATH
#endif

using namespace std;


bool ioExists(const string& filename)
{
    return (access(filename.c_str(), 0) == 0);
}

bool ioIsDir(const string& filename)
{
    struct stat status;
    stat(filename.c_str(), &status);

    return (status.st_mode & S_IFDIR);
}

bool ioIsFile(const string& filename)
{
    struct stat status;
    stat(filename.c_str(), &status);

    return (status.st_mode & S_IFREG);
}

bool ioIsReadable(const string& filename)
{
    return (access(filename.c_str(), 4) == 0);
}

bool ioIsWriteable(const string& filename)
{
    return (access(filename.c_str(), 2) == 0);
}

bool ioIsReadWriteable(const string& filename)
{
    return (access(filename.c_str(), 6) == 0);
}

bool ioNew(const string& filename, bool readable, bool writeable)
{
    if(ioExists(filename))
        return false;

    FILE* file = fopen(filename.c_str(), "wb");
    if(file == NULL)
        return false;
    fclose(file);
    return true;
}

bool ioNewDir(const string& dirname, int mode)
{
    if(ioExists(dirname))
        return false;

    #ifdef WIN32
    return (mkdir(dirname.c_str()) == 0);
    #else
    int userBits =   (mode & IO_USER?   (mode & IO_READ? S_IXUSR | S_IRUSR : 0) | (mode & IO_WRITE? S_IWUSR : 0) : 0);
    int groupBits =  (mode & IO_GROUP?  (mode & IO_READ? S_IXGRP | S_IRGRP : 0) | (mode & IO_WRITE? S_IWGRP : 0) : 0);
    int othersBits = (mode & IO_OTHERS? (mode & IO_READ? S_IXOTH | S_IROTH : 0) | (mode & IO_WRITE? S_IWOTH : 0) : 0);

    return (mkdir(dirname.c_str(), userBits | groupBits | othersBits) == 0);
    #endif
}

bool ioDelete(const string& filename)
{
    #ifdef WIN32
    return (unlink(filename.c_str()) == 0) || (rmdir(filename.c_str()) == 0);
    #else
    return (remove(filename.c_str()) == 0);
    #endif
}

bool ioMove(const string& source, const string& dest)
{
    if(source == dest)
        return true;

    if(!ioCopy(source, dest))
        return false;

    if(!ioDelete(source))
        return false;

    return true;
}

// This has a same device limitation
bool ioRename(const string& source, const string& dest)
{
    return (rename(source.c_str(), dest.c_str()) == 0);
}

bool ioCopy(const string& source, const string& dest)
{
    if(source == dest)
        return true;

    FILE* src = fopen(source.c_str(), "rb");
    if(src == NULL)
        return false;

    FILE* dst = fopen(dest.c_str(), "wb");
    if(dst == NULL)
    {
        fclose(src);
        return false;
    }

    char buff[IO_COPY_BUFFSIZE];
    unsigned int numBytes;

    while((numBytes = fread(buff, 1, IO_COPY_BUFFSIZE, src)) > 0)
    {
        if(fwrite(buff, 1, numBytes, dst) != numBytes)
        {
            fclose(src);
            fclose(dst);
            return false;
        }
    }

    fclose(src);
    fclose(dst);

    return true;
}

bool ioClear(const string& filename)
{
    if(!ioExists(filename))
        return false;

    FILE* file = fopen(filename.c_str(), "wb");
    if(file == NULL)
        return false;
    fclose(file);
    return true;
}

// Should these have binary counterparts?
// Prepend avoids access permissions, but append respects them...  Oh well!
bool ioPrepend(const string& text, const string& filename)
{
    // Create a temp file
    string temp = ioUniqueName(filename, 0);
    if(temp == "" || !ioNew(temp))
        return false;

    // Put the new stuff into temp
    if(!ioAppend(text, temp))
    {
        ioDelete(temp);
        return false;
    }

    // It may be safer to use just one open/close per file instead of using goodIO:
    //Now use ioCopy's code to append all of the source file to the temp file.
    //ioDelete(filename);
    //FILE* result = fopen(filename.c_str(), "w");
    //copy all of the temp file data to the new file now.
    //ioDelete(temp);

    // Put the old stuff into temp
    if(!ioAppendFile(filename, temp))
    {
        ioDelete(temp);
        return false;
    }

    // Replace the old file
    ioDelete(filename);
    if(!ioRename(temp, filename))
        return false;

    return true;
}

bool ioAppend(const string& text, const string& filename)
{
    FILE* file = fopen(filename.c_str(), "a");
    if(file == NULL)
        return false;

    if(fwrite(text.c_str(), 1, text.length(), file) != text.length())
    {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool ioAppendFile(const string& srcfile, const string& destfile)
{
    FILE* file1 = fopen(destfile.c_str(), "a");
    if(file1 == NULL)
        return false;
    FILE* file2 = fopen(srcfile.c_str(), "r");
    if(file2 == NULL)
    {
        fclose(file1);
        return false;
    }

    char buff[IO_COPY_BUFFSIZE];
    unsigned int numBytes;

    while((numBytes = fread(buff, 1, IO_COPY_BUFFSIZE, file2)) > 0)
    {
        if(fwrite(buff, 1, numBytes, file1) != numBytes)
        {
            fclose(file1);
            fclose(file2);
            return false;
        }
    }

    fclose(file1);
    fclose(file2);
    return true;
}

string ioStripToDir(const string& filename)
{
    size_t lastSlash = filename.find_last_of("/\\");
    if(lastSlash == string::npos)
        return "";
    return filename.substr(0, lastSlash);
}

string ioStripToFile(const string& filename)
{
    return filename.substr(filename.find_last_of("/\\") + 1);  // If npos, it should become 0, which is good :)
}

string ioStripToExt(const string& filename)
{
    size_t lastDot = filename.find_last_of(".");
    size_t lastSlash = filename.find_last_of("/\\");
    if(lastDot == string::npos || (lastSlash != string::npos && lastSlash >= lastDot - 1)) // "/usr/cow" or "/usr/.hidden" or "/usr/tex.d/"
        return "";
    return filename.substr(lastDot + 1);
}

string ioUniqueName(const string& filename, int startAt)
{
    string ext, base;
    size_t lastDot = filename.find_last_of(".");
    if(lastDot == string::npos || filename.find_last_of("/\\") >= lastDot - 1)
    {
        ext = "";
        base = filename;
    }
    else
    {
        ext = filename.substr(lastDot);
        base = filename.substr(0, lastDot);
    }

    char buff[20];
    for(; startAt < IO_UNIQUE_MAX; startAt++)
    {
        sprintf(buff, "%d", startAt);
        if(!ioExists(base + buff + ext))
            return base + buff + ext;
    }

    return "";
}

int ioSize(const string& filename)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return -1;
    return status.st_size;
}

time_t ioTimeAccessed(const string& filename)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return -1;
    return status.st_atime;
}

time_t ioTimeModified(const string& filename)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return -1;
    return status.st_mtime;
}

time_t ioTimeStatus(const string& filename)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return -1;
    return status.st_ctime;
}

string ioTimeString(time_t time)
{
    return ctime(&time);
}

bool ioSetReadable(const string& filename, bool readable)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return false;

    // Preserve the other permission
    int writeBit = status.st_mode & S_IWRITE;
    if(readable)
    {
        if(status.st_mode & S_IREAD)
            return true;
        return (chmod(filename.c_str(), writeBit | S_IREAD) == 0);
    }
    else
    {
        if(!(status.st_mode & S_IREAD))
            return true;
        return (chmod(filename.c_str(), writeBit) == 0);
    }
}

bool ioSetWriteable(const string& filename, bool writeable)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return false;

    // Preserve the other permission
    int readBit = status.st_mode & S_IREAD;
    if(writeable)
    {
        if(status.st_mode & S_IWRITE)
            return true;
        return (chmod(filename.c_str(), readBit | S_IWRITE) == 0);
    }
    else
    {
        if(!(status.st_mode & S_IWRITE))
            return true;
        return (chmod(filename.c_str(), readBit) == 0);
    }
}

bool ioSetReadWriteable(const string& filename, bool ReadWriteable)
{
    struct stat status;
    if(stat(filename.c_str(), &status) < 0)
        return false;

    return (chmod(filename.c_str(), ReadWriteable? S_IREAD | S_IWRITE : 0) == 0);
}

// Returns the executable's name.  This works only for Linux so far and uses /proc, which may be unportable to some UNIX platforms.
// Needs unistd.h and string.
string ioGetProgramPath()
{
    #ifdef WIN32
    TCHAR name[IO_PATH_MAX];
    DWORD len = GetModuleFileName( NULL, name, IO_PATH_MAX );
    name[len] = '\0';

    #elif defined(LINUX)

    char name[IO_PATH_MAX];
    pid_t pid = getpid();
    char symlink[128];
    sprintf(symlink, "/proc/%d/exe", pid);
    ssize_t len = readlink(symlink, name, IO_PATH_MAX);
    name[len] = '\0';
    #endif

    // If you get an error here, where name is undefined, then this function is
    // not implemented for your platform!
    return ioStripToDir(name);
}


bool ioSetCWD(const string& dir)
{
    return (chdir(dir.c_str()) == 0);
}

string ioGetCWD()
{
    char s[IO_PATH_MAX];
    return getcwd(s, IO_PATH_MAX);
}


list<string> ioList(const string& dirname, bool directories, bool files)
{
    list<string> dirList;
    list<string> fileList;

    DIR* dir = opendir(dirname.c_str());
    dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        #ifdef WIN32
        if(ioIsDir(dirname + "/" + entry->d_name))
        #else
        if(entry->d_type == DT_DIR)
        #endif
        {
            if(directories)
                dirList.push_back(entry->d_name);
        }
        else if(files)
            fileList.push_back(entry->d_name);
    }

    closedir(dir);

    dirList.sort();
    fileList.sort();

    fileList.splice(fileList.begin(), dirList);


    return fileList;
}



list<string> ioExplode(const string& str, char delimiter)
{
    list<string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);
    while(pos != string::npos)
    {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, string::npos));

    // Test this:
    /*unsigned int pos;
    do
    {
        pos = str.find_first_of(delimiter, oldPos);
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
    }
    while(pos != string::npos);*/

    return result;
}

vector<string> ioExplodev(const string& str, char delimiter)
{
    vector<string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);
    while(pos != string::npos)
    {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, string::npos));

    // Test this:
    /*unsigned int pos;
    do
    {
        pos = str.find_first_of(delimiter, oldPos);
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
    }
    while(pos != string::npos);*/

    return result;
}


// ioFileReader
ioFileReader::ioFileReader(const string& filename)
    : fin(filename.c_str())
    , file(filename)
{}

ioFileReader::~ioFileReader()
{
    if(fin.is_open())
        fin.close();
}

void ioFileReader::stripCarriageReturn(string& str)
{
    if(str.length() > 0 && str[str.length()-1] == '\r')
        str.erase(str.length()-1);
}

char ioFileReader::get()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    return fin.get();
}

char ioFileReader::peek()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    return fin.peek();
}

string ioFileReader::getLine()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    string result;
    getline(fin, result);
    stripCarriageReturn(result);
    return result;
}

std::string ioFileReader::getString(char delimiter)
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    string result;
    char c;
    do
    {
        c = fin.get();
        if(c == delimiter)
            break;
        result += c;
    }
    while(ready());
    return result;
}

std::string ioFileReader::getStringUntil(char delimiter)
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");

    string result;
    while(ready() && fin.peek() != delimiter)
        result += fin.get();
    while(ready());
    return result;
}

bool ioFileReader::getBool()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    bool result;
    fin >> result;
    return result;
}

int ioFileReader::getInt()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    int result;
    fin >> result;
    return result;
}

unsigned int ioFileReader::getUInt()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    unsigned int result;
    fin >> result;
    return result;
}

long ioFileReader::getLong()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    long result;
    fin >> result;
    return result;
}

unsigned long ioFileReader::getULong()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    unsigned long result;
    fin >> result;
    return result;
}

float ioFileReader::getFloat()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    float result;
    fin >> result;
    return result;
}

double ioFileReader::getDouble()
{
    if(!ready())
        throw std::runtime_error("io::FileReader not ready.");
    double result;
    fin >> result;
    return result;
}

// Ignore the next characters
void ioFileReader::skip(unsigned int numChars)
{
    while(ready() && numChars > 0)
    {
        fin.get();
        numChars--;
    }
}

void ioFileReader::skip(unsigned int numChars, char delimiter)
{
    while(ready() && numChars > 0 && fin.get() != delimiter)
        numChars--;
}

void ioFileReader::skip(char delimiter)
{
    while(ready() && fin.get() != delimiter)
        ;
}

void ioFileReader::skipUntil(unsigned int numChars, char delimiter)
{
    while(ready() && numChars > 0 && fin.peek() != delimiter)
    {
        numChars--;
        fin.get();
    }
}

void ioFileReader::skipUntil(char delimiter)
{
    while(ready() && fin.peek() != delimiter)
        fin.get();
}

void ioFileReader::skipLine()
{
    skip('\n');
}

bool ioFileReader::ready()
{
    return (fin.good() && !fin.eof());
}

