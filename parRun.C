#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
namespace Koala
{

parRun::parRun()
{
  if ( ( path = getcwd(NULL, 0) ) == NULL )
  {
    std::cout<< "Error: getcwd()" << std::endl;
    exit(1);
  }
}
}
