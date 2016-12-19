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
    std::cout << "Error: getcwd()" << std::endl;
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

// IO: read file
void parRun::openFile
(
  std::ifstream& file    ,
  const char* fileName   ,
  const char* fileSuffix ,
  const char* filePath
) const
{
  if (file.is_open())
    file.close();

  file.open(getFileFullPath(fileName, fileSuffix, filePath).data());
  if (file.is_open())
  {
    std::cout << "Can't not find "
              << getFileFullPath(fileName, fileSuffix, filePath).data()
              << std::endl;
    return 0;
  }
}

// IO: write file
void parRun::openFile
(
  std::ofstream& file    ,
  const char* fileName   ,
  const char* fileSuffix ,
  const char* filePath
) const
{
  if (file.is_open())
    file.close();

  file.open(getFileFullPath(fileName, fileSuffix, filePath).data());
  if (file.is_open())
  {
    std::cout << "File path "
              << getFileFullPath(NULL, NULL, filePath).data()
              << std::endl;
    return 0;
  }
}
}
