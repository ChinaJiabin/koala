#include <string>
#include <fstream>
#include <iostream>
namespace Koala
{

// Constructor
parRun::parRun()
{
  if ( ( path = getcwd(NULL, 0) ) == NULL )
  {
    std::cout<< "Error: getcwd()" << std::endl;
    exit(1);
  }
}

// IO
std::string parRun::getFileFullPath
(
  const char* fileName   ,
  const char* fileSuffix ,
  const char* filePath
) const
{
  std::string fileFullPath = path;

  fileFullPath.append("/");
  if (filePath)
  {
    fileFullPath.append("filePath");
    fileFullPath.append("/");
  }

  if (fileName)
  {
    fileFullPath.append("fileName");
    
    if (fileSuffix)
      fileFullPath.append(fileSuffix);
  }

  return fileFullPath;
}
}
