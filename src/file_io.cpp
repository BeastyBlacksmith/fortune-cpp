#include "file_io.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <fstream>


namespace {

    std::string error()
    {
        char *s = strerror(errno);
        return std::string(s);
    }


}  // namespace


File::File(std::string path, std::ios_base::openmode mode) :
    file(path, mode),
    path(path)
{
    if (file.fail()) {
        throw std::runtime_error(path + ": " + error());
    }
}

File::File(std::string path) : File(path, std::fstream::in | std::fstream::out)
{
}

uint32_t File::read32b()
{
    char b[4];
    file.read(b, 4);
    if (file.eof()) {
        throw std::runtime_error(path + ": short read");
    }
    return (((uint32_t)(uint8_t)b[0]) << 24) |
           (((uint32_t)(uint8_t)b[1]) << 16) |
           (((uint32_t)(uint8_t)b[2]) << 8)  |
             (uint32_t)(uint8_t)b[3];
}

File& File::read(char *buffer, int size)
{
    file.read(buffer, size);
    if (file.eof()) {
        throw std::runtime_error(path + ": short read");
    }
    return *this;
}

File& File::seekg(off_t pos)
{
    file.seekg(pos);
    return *this;
}

bool File::is_file(std::string const& name)
{
    struct stat st;
    if (stat(name.c_str(), &st) < 0) {
        throw std::runtime_error(name + ": " + error());
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

bool File::is_directory(std::string const& name)
{
    struct stat st;
    if (stat(name.c_str(), &st) < 0) {
        throw std::runtime_error(name + ": " + error());
    }
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

char File::separator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}


// InputFile

InputFile::InputFile(std::string path) : File(path, std::fstream::in)
{
}


