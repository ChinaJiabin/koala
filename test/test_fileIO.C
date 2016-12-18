#include "parRun.H"
using namespace Koala
int main(int argc, char* argv[])
{
  parRun Run;

  // File testInput only contains 1
  std::ifstream file;
  Run.openfile(file, "testInput", NULL, NULL);

  int anIntBuffer;
  file >> anIntBuffer;
  if (anIntBuffer != 1)
  {
    std::cout << "Test fileIO not pass!" << std::endl;
    exit(1);
  }

  std::cout << "Test fileIO pass!" << std::endl;
  return 0;
}
