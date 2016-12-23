#include "parRun.H"
#include "toyMesh2D.H"
#include "writeVTK.H"
using namespace Koala;

int main(int argc, char const *argv[])
{
  parRun Run;
  toyMesh2D mesh(Run);

  std::cout << "Write points" << std::endl;
  mesh.writePoints();

  std::cout << "Write cells" << std::endl;
  mesh.writeCells();

  std::cout << "Write faces" << std::endl;
  mesh.writeFaces();

  std::cout << "Write boundary name" << std::endl;
  mesh.writeBoundaryName();

  std::cout << "Write boundary points id" << std::endl;
  mesh.writeBoundaryPointsId();

  std::cout << "Write boundary faces" << std::endl;
  mesh.writeBoundaryFaces();

  std::cout << "Convert to vtk format" << std::endl;
  writeToyMeshVTK(Run, 2);

  return 0;
}
