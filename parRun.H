#ifndef parRun_H
#define parRun_H

#include <fstream>
namespace Koala
{

class parRun
{
private:
        const char* path;

public:
    // Constructor

        parRun();

    // inline
     
       inline const char* Path() const{ return path; }

    // IO

       std::string getFileFullPath
       (
         const char* fileName          ,
         const char* fileSuffix = NULL ,
         const char* filePath   = NULL
       ) const;
};
}

#endif
