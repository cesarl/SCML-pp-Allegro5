/*
goodio v1.1 - A small library for simple filesystem interaction.
by Jonathan Dearborn 3/28/09 - 10/4/09
grimfang4 [at] gmail [dot] com

The goodio functions are as follows:

Path info:
	string ioGetProgramPath();  // Returns the directory of the executable
	bool ioSetCWD(const string& dir);  // Changes the current working directory
	string ioGetCWD();  // Returns the current working directory

File info:
	bool ioExists(const string& filename);  // Does the file exist?
	bool ioIsDir(const string& filename);  // Is the file a directory?
	bool ioIsFile(const string& filename);  // Is the file a regular file (not a directory)?
	bool ioIsHidden(const string& filename);  // Is the file hidden?
	int ioSize(const string& filename);  // Get the size of a file in bytes
	int ioTimeAccessed(const string& filename);  // Get the time a file was last accessed
	int ioTimeModified(const string& filename);  // Get the time a file was last modified
	int ioTimeStatus(const string& filename);  // Get the time a file last had its status (file properties) changed
	string ioTimeString(time_t time);  // Convert the result of a goodIO time function into a human-readable string
	bool ioIsReadable(const string& filename);  // Do I have 'read' permissions for this file?
	bool ioIsWriteable(const string& filename);  // Do I have 'write' permissions for this file?
	bool ioIsReadWriteable(const string& filename);  // Do I have 'read' and 'write' permissions for this file?
	bool ioSetReadable(const string& filename, bool readable = true);  // Change 'read' permissions
	bool ioSetWriteable(const string& filename, bool writeable = true);  // Change 'write' permissions
	bool ioSetReadWriteable(const string& filename, bool ReadWriteable = true);  // Change 'read' and 'write' permissions
	list<string> ioList(const string& dirname, bool directories = true, bool files = true);  // Returns a list of files in the directory

File editing:
	bool ioNew(const string& filename, bool readable = true, bool writeable = true);  // Create an empty file
	bool ioNewDir(const string& dirname, int mode = IO_USER | IO_READWRITE);  // Create an empty directory (see Notes below)
	bool ioDelete(const string& filename);  // Delete a file
	bool ioMove(const string& source, const string& dest);  // Move a file
	bool ioRename(const string& source, const string& dest);  // Rename a file
	bool ioCopy(const string& source, const string& dest);  // Copy a file
	bool ioClear(const string& filename);  // Erase the contents of a file
	bool ioAppend(const string& filename, const string& text);  // Add text to the end of a file's contents
	bool ioPrepend(const string& text, const string& filename);  // Add text to the beginning of a file's contents
	bool ioAppendFile(const string& srcfile, const string& destfile);  // Concatenate two files

String functions:
	string ioStripToDir(const string& filename);  // Get the parent directory
	string ioStripToFile(const string& filename);  // Get the file name without the path
	string ioStripToExt(const string& filename);  // Get the file extension (type)
	string ioUniqueName(const string& filename, int startAt);  // Get an unused file name (e.g. file.txt -> file5.txt)

FileReader class:
	ioFileReader(const std::string& filename);  // Constructor: Opens the file for reading
	char get();  // Returns the next character
	char peek();  // Returns the next character without removing it from the stream
	std::string getLine();  // Returns an entire line (until a newline)
	std::string getString(char delimiter);  // Returns an entire line until either the delimiter is found or a newline is found
	std::string getStringUntil(char delimiter);  // Returns an entire line until either the delimiter is found or a newline is found.  Doesn't remove delimiter from the stream
	bool getBool();  // Reads a bool value from the file
	int getInt();  // Reads an integer value from the file
	unsigned int getUInt();  // Reads an unsigned integer value from the file
	long getLong();  // Reads a long value from the file
	unsigned long getULong();  // Reads an unsigned long value from the file
	float getFloat();  // Reads a float value from the file
	double getDouble();  // Reads a double value from the file
	void skip(unsigned int numChars);  // Ignore the next characters
	void skip(unsigned int numChars, char delimiter);  // Ignore the next characters until the delimiter is found
	void skip(char delimiter);  // Ignore the next characters until the delimiter is found
	void skipUntil(unsigned int numChars, char delimiter);  //   // Ignore the next characters until the delimiter is found.  Doesn't remove delimiter from the stream
	void skipUntil(char delimiter);  //   // Ignore the next characters until the delimiter is found.  Doesn't remove delimiter from the stream
	void skipLine();  // Ignores an entire line
	bool ready();  // Returns true if there are no problems and there is more to be read from the file




Notes:

Functions which return a 'bool' will return 'true' on success and 'false' on failure.
Functions which return an 'int' will return a value >= 0 on success and -1 on failure.
The privilege flags available to ioNewDir() are:
    IO_NONE, IO_USER, IO_GROUP, IO_OTHERS  // Who can do it
    IO_NONE, IO_READ, IO_WRITE, IO_READWRITE  // What they can do
    e.g. IO_USER | IO_GROUP | IO_WRITE  // Gives write access to the user and group



goodio is released under the TSL:
    The short:
    Use it however you'd like, but give me credit if you share this code
    or a compiled version of the code, whether or not it is modified.
    
    The long:
    Copyright (c) 2009, Jonathan Dearborn
    All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted 
provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list 
      of conditions and the following disclaimer.
    * Redistributions in binary form, excluding commercial executables, must reproduce 
      the above copyright notice, this list of conditions and the following disclaimer 
      in the documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote products 
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY 
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _GOODIO_H__
#define _GOODIO_H__

#include <string>
#include <list>
#include <vector>
#include <fstream>


#define IO_NONE 0
#define IO_READ 1
#define IO_WRITE 2
#define IO_USER 4
#define IO_GROUP 8
#define IO_OTHERS 16

#define IO_READWRITE (IO_READ | IO_WRITE)


extern "C"
{
    
// Path stuff
std::string ioGetProgramPath();

bool ioSetCWD(const std::string& dir);

std::string ioGetCWD();

std::string ioStripToDir(const std::string& filename);

std::string ioStripToFile(const std::string& filename);

std::string ioStripToExt(const std::string& filename);

std::string ioUniqueName(const std::string& filename, int startAt = 1);

// File info
bool ioExists(const std::string& filename);

bool ioIsDir(const std::string& filename);

bool ioIsFile(const std::string& filename);

int ioSize(const std::string& filename);

time_t ioTimeAccessed(const std::string& filename);

time_t ioTimeModified(const std::string& filename);

time_t ioTimeStatus(const std::string& filename);

std::string ioTimeString(time_t time);


// Access testing
bool ioIsReadable(const std::string& filename);

bool ioIsWriteable(const std::string& filename);

bool ioIsReadWriteable(const std::string& filename);

// Access setting
bool ioSetReadable(const std::string& filename, bool readable = true);

bool ioSetWriteable(const std::string& filename, bool writeable = true);

bool ioSetReadWriteable(const std::string& filename, bool ReadWriteable = true);


// File modification
bool ioNew(const std::string& filename, bool readable = true, bool writeable = true);

bool ioNewDir(const std::string& dirname, int mode = IO_USER | IO_READWRITE);

bool ioDelete(const std::string& filename);

bool ioMove(const std::string& source, const std::string& dest);

bool ioCopy(const std::string& source, const std::string& dest);

bool ioRename(const std::string& source, const std::string& dest);

bool ioClear(const std::string& filename);

// File writing

bool ioAppend(const std::string& text, const std::string& filename);

bool ioPrepend(const std::string& text, const std::string& filename);

bool ioAppendFile(const std::string& srcfile, const std::string& destfile);


// Directory listing
std::list<std::string> ioList(const std::string& dirname, bool directories = true, bool files = true);

std::list<std::string> ioExplode(const std::string& str, char delimiter = ' ');
std::vector<std::string> ioExplodev(const std::string& str, char delimiter = ' ');

class ioFileReader
{
private:
    std::ifstream fin;
    std::string file;
 
    ioFileReader (const ioFileReader&);
    ioFileReader& operator=(const ioFileReader&);
    
    void stripCarriageReturn(std::string& str);

public:
    ioFileReader(const std::string& filename);
 
    ~ioFileReader();
    
    char get();
    
    char peek();
 
    std::string getLine();
 
    std::string getString(char delimiter);
    
    // Doesn't remove delimiter from the stream
    std::string getStringUntil(char delimiter);
 
    bool getBool();
 
    int getInt();
 
    unsigned int getUInt();
 
    long getLong();
 
    unsigned long getULong();
 
    float getFloat();
 
    double getDouble();
    
    // Ignore the next characters
    void skip(unsigned int numChars);
    
    void skip(unsigned int numChars, char delimiter);
    
    void skip(char delimiter);
    
    // Doesn't remove delimiter from the stream
    void skipUntil(unsigned int numChars, char delimiter);
    
    // Doesn't remove delimiter from the stream
    void skipUntil(char delimiter);
    
    void skipLine();
    
    bool ready();
};


}


#endif
