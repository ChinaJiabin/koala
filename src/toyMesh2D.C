#include <string>
#include <vector>
#include <fstream>
#include <upstream>
namespace Koala
{

parBlock2D::parBlock2D()
{
  type = new int[4]; // Suggest: Type use char instead of int
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
  for（int i = 1; i < 4*sizeBlocks + 1; i++)
  {
    file >> parLinesIndex[i];
    parLinesIndex[i] += parLinesIndex[i - 1];
  }
 
  parLines = new double [parLinesIndex[4*sizeBlocks]];
  for（int i = 0; i < parLinesIndex[4*sizeBlocks]; i++)
    file >> parLines[i];

  // Boundary
  file >> sizePatches;
  boundaryFacesIndex    = new int[sizePatches + 1];
  boundaryFacesIndex[0] = 0;
  for (int i = 1; i < sizePatches + 1; i++)
  {
    file >> boundaryFacesIndex[i];
    boundaryFacesIndex[i] += boundaryFacesIndex[i - 1];
  }

  boundaryFaces = new int[2*boundaryFacesIndex[sizePatches]][2];
  for (int i = 0; i < sizePatches; i++)
  { 
    std::string patchName;
    file >> patchName;
    patchesName += patchName;
    patchesName.append("\n");
  }

  for (int j = boundaryFacesIndex[i]; j < boundaryFacesIndex[i + 1]; j++)
  {
    file >> boundaryFaces[j][0];
    file >> boundaryFaces[j][1];
  }

  // Loop blocks
  sizePoints     = sizePointsOfBlocks;
  sizeCells      = 0;
  sizeInnerFaces = 0;
  sizeLines      = 0;  

  cellsIndex    = new int[sizeBlocks + 1];
  cellsIndex[0] = 0;

  lines = new int[4*sizeBlocks][5];
M
  for (int = 0; i < sizeBlocks; i++)
  {
    sizePoints     += parBlocks[i].SizePoints();
    sizeCells      += parBlocks[i].SizeCells();
    sizeInnerFaces += parBlocks[i].SizeInnerFaces();

    cellsIndex[i + 1] = sizeCells;
 
    int*        blockPointsId = &blocks[4*i];
    listLines2D blockLines    = &lines[4*i];
    for ( j = 0; j < 4; j++)
    {
      blockLines[j][0] = blockPointsId[j];         // First points Id of line
      blockLines[j][1] = blockPointsId[(j + 1)%4]; // Second point Id of line
      blockLines[j][2] = ++sizeLines;              // Id
      blockLines[j][3] = -1;                       // Line Id(0,1,2,3) in neighbor block
      blockLines[j][4] = -1;                       // Neighbour block Id
    }
  }
}
}
