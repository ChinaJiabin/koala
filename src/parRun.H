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

        // Read file
        void openFile
        (
          std::ifstream& file           ,
          const char* fileName          ,
          const char* fileSuffix = NULL ,
          const char* filePath   = NULL
        ) const;  
        
        // Write file  
        void openFile
        (
          std::ofstream& file           ,
          const char* fileName          ,
          const char* fileSuffix = NULL ,
          const char* filePath   = NULL
        ) const;
};
}
#include "parRun.C"

#endif
