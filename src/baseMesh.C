#include <stdlib.h>
namespace Koala
{

// Static
int baseMesh::CellVtkType(const int& size, const int& dim)
{
  switch (size)
  {
    case 3:
      return 5;
      
    case 4:
      return dim == 2 ? 9 : 10;
    
    case 8:
      return 12;
      
    default:
      std::cout << "Can't find vtk type" << std::endl;
      exit(1);
  }
}
}
