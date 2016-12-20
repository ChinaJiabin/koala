#include <iostream>
#include "parRun.H"
using namespace Koala;

int main(int argc, char const *argv[])
{
  parRun Run;

  // File testInput only contains 1
  std::ifstream file;
  Run.openFile(file, "testInput", NULL, NULL);

  int anIntBuffer;
  file >> anIntBuffer;
  if (anIntBuffer != 1)
  {
    std::cout << "Test fileIO not pass!" << std::endl;
    return 0;
  }

  std::cout << "Test fileIO pass!" << std::endl;
  return 0;
}
