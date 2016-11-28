#include <string>
#include <vector>
#include <fstream>
#include <upstream>
namespace Koala
{

parBlock2D::parBlock2D()
{
  // Suggest: Type use char instead of int
  type = new int[4];
  n    = new int[2];

  memset(type, 0, 4*sizeof(int));
  memset(n   , 0, 2*sizeof(int));
}

toyMesh2D::toyMesh2D
(
  const parRun& Run ,
  const char* inputFileName_
)
:
  baseToyMesh(Run_, inputFileName_)
{
  // Open file for read
  std::ifstream file;
  Run.openFile(file, inputFileName, NULL, filesPath);
 
  // Read Points
  file >> sizePointsOfBlocks;
  pointsOfBlocks = new double[sizePointsOfBlocks][2];
  for (int i = 0; i < sizePointsOfBlocks; i++)
    for (int j = 0; j < 2; j++)
      file >> pointsOfBlocks[i][j];

  // Read blocks
  file >> sizeBlocks;
  int blocks[4*sizeBlocks];  
  for (int i = 0; i < 4*sizeBlocks; i++)
    file >> blocks[i];

  parBlocks = new parBlock2D[sizeBlocks];
  for (int i = 0; i < sizeBlocks; i++)
  {
    for (int j = 0; j < 2; j++)
      file >> parBlocks[i].n[j];

    for (int j = 0; j < 4; j++)
      file >> parBlocks[i].type[j];
  }

  // Read parameters of lines
  parLinesIndex    = new int[4*sizeBlocks + 1];
  parLinesIndex[0] = 0;
  for（int i = 0; i < 4*sizeBlocks + 1; i++)
  {
    file >> parLinesIndex[i];
    parLinesIndex[i] += parLinesIndex[i - 1];
  }
 
  parLines = new double [parLinesIndex[4*sizeBlocks]];
  for（int i = 0; i < parLinesIndex[4*sizeBlocks]; i++)
    file >> parLines[i];

}
}
