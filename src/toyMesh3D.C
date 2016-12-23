#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
namespace Koala
{

parBlock3D::parBlock3D()
{
  type = new int[12](); // Suggest: Type use char instead of int
  n    = new int[3]();
}

const int LINE_POINT_FIRST _ID = 0;
const int LINE_POINT_END_ID    = 1;
  
const int faceLineId[6][4] =
{
  {0, 3,  2,  1} ,
  {8, 9, 10, 11} ,
  {1, 6,  9,  5} ,
  {3, 4, 11,  7} ,
  {0, 5,  8,  4} ,
  {2, 7, 10,  6}
};
  
toyMesh2D::toyMesh2D
(
  const parRun& Run_         ,
  const char* inputFileName_ ,
  const char* filesPath_
)
:
  baseToyMesh(Run_, inputFileName_, filesPath_)
{
  // Open file for read
  std::ifstream file;
  Run.openFile(file, inputFileName, NULL, filesPath);

  // Read Points
  file >> sizePointsOfBlocks;
  pointsOfBlocks = new double[sizePointsOfBlocks][3];
  for (int i = 0; i < sizePointsOfBlocks; i++)
    for (int j = 0; j < 3; j++)
      file >> pointsOfBlocks[i][j];

  // Read blocks
  file >> sizeBlocks;
  int blocks[8*sizeBlocks];
  for (int i = 0; i < 8*sizeBlocks; i++)
    file >> blocks[i];

  parBlocks = new parBlock2D[sizeBlocks];
  for (int i = 0; i < sizeBlocks; i++)
  {
    for (int j = 0; j < 3; j++)
      file >> parBlocks[i].n[j];

    for (int j = 0; j < 12; j++)
      file >> parBlocks[i].type[j];
  }

  // Read parameters of lines
  parLinesIndex    = new int[12*sizeBlocks + 1];
  parLinesIndex[0] = 0;
  for (int i = 1; i < 12*sizeBlocks + 1; i++)
  {
    file >> parLinesIndex[i];
    parLinesIndex[i] += parLinesIndex[i - 1];
  }

  parLines = new double [parLinesIndex[12*sizeBlocks]];
  for (int i = 0; i < parLinesIndex[12*sizeBlocks]; i++)
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

    for (int j = boundaryFacesIndex[i]; j < boundaryFacesIndex[i + 1]; j++)
    {
      file >> boundaryFaces[j][0];
      file >> boundaryFaces[j][1];
    }
  }
  
  // Lines and faces
  lines = new int[12*sizeBlocks][3];
  faces = new int[6*sizeBlocks][6];
  
  sizePoints = sizePointsOfBlocks;
  sizeCells  = 0;
  sizeLines  = 0;
  sizeFaces  = 0;
  for (int blockId = 0; blockId < sizeBlocks; blockId++)
  {
    sizePoints += parBlocks[blockId].SizePoints() - 8;
    sizeCells  += parBlocks[blockId].SizeCells();

    listLines3D blockLines = &lines[12*blockId];
    int* blockPointsId     = &blocks[8*blockId];
    for (int lineIdInBlock = 0; lineIdInBlock < 12; lineIdInBlock++)
    {
      if (lineIdInBlock < 4)
      {
        blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] = blockPoints[lineIdInBlock];
        blockLines[lineIdInBlock][LINE_POINT_END_ID]   = blockPoints[(lineIdInBlock + 1)%4];        
      }
      else if (lineIdInBlock < 8)
      {
        blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] = blockPoints[(lineIdInBlock - 4)];
        blockLines[lineIdInBlock][LINE_POINT_END_ID]   = blockPoints[lineIdInBlock];
      }
      else
      {
        blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] = blockPoints[(lineIdInBlock - 4)];
        blockLines[lineIdInBlock][LINE_POINT_END_ID]   = blockPoints[4 + (lineIdInBlock - 3)%4];
      }
      blockLines[lineIdInBlock][2] = ++sizeLines;
      
      for (int globalLineId = 0; globalLineId < 12*blockId; globalLineId++)
        if (
             (lines[globalLineId][LINE_POINT_FIRST_ID] == blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] &&
              lines[globalLineId][LINE_POINT_END_ID]   == blockLines[lineIdInBlock][LINE_POINT_END_ID])
             ||
             (lines[globalLineId][LINE_POINT_FIRST_ID] == blockLines[lineIdInBlock][LINE_POINT_END_ID]   &&
              lines[globalLineId][LINE_POINT_END_ID]   == blockLines[lineIdInBlock][LINE_POINT_FIRST_ID])
           )
        {
          blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] = lines[globalLineId][LINE_POINT_FIRST_ID];
          blockLines[lineIdInBlock][LINE_POINT_END_ID]   = lines[globalLineId][LINE_POINT_END_ID];
          blockLines[lineIdInBlock][2]                   = lines[globalLineId][2];
          
          if (j < 4 || j >= 8)
            sizePoints -= (parBlocks[blockId].n[j%2] - 1);
          else
            sizePoints -= (parBlocks[blockId].n[2] - 1);
          
          sizeLines--;
          break;
        }
    }
    
    listFaces3D blockFaces = &faces[6*blockId];
    for (int faceIdInBlock = 0; faceIdInBlock < 6; faceIdInBlock++)
    {
      for (int lineIdInFace = 0; lineIdInFace < 4; lineIdInFace++)
        blockFaces[faceIdInBlock][lineIdInFace] = abs(blockLines[faceLineId[faceIdInBlock][lineIdInFace]][2]) - 1;
      
      blockFaces[faceIdInBlock][4] = ++sizeFaces;
      blockFaces[faceIdInBlock][5] = -1;
      for (int globalFaceId = 0; globalFaceId < 6*blockId; globalFaceId++)
      { 
        int pointIdInFace = 0;
        if (
             blockFaces[faceIdInBlock][0] == faces[globalFaceId][pointIdInFace]   ||
             blockFaces[faceIdInBlock][0] == faces[globalFaceId][++pointIdInFace] ||
             blockFaces[faceIdInBlock][0] == faces[globalFaceId][++pointIdInFace] ||
             blockFaces[faceIdInBlock][0] == faces[globalFaceId][++pointIdInFace] 
           )
          if (
             )
            {
              blockFaces[faceIdInBlock][4] = -faces[globalFaceId][4];
              blockFaces[faceIfInBlock][5] = pointIdInFace;
            
              if (faceIdInBlock < 2)
                sizePoints -=(parBlocks[blockId].n[0] - 1)*(parBlocks[blockId].n[1] - 1);
              else if (faceIdInBlock < 4)
                sizePoints -=(parBlocks[blockId].n[1] - 1)*(parBlocks[blockId].n[2] - 1);               
              else
                sizePoints -=(parBlocks[blockId].n[2] - 1)*(parBlocks[blockId].n[0] - 1);                
            }
        }
      }
    }
    
    // Index of points on lines
    int trackingId     = sizePointsOfBlocks;
    pointsOnLinesIndex = new int[sizeLines + 1];
    pointsOnLinesIndex = trackingId;
    
    // Index of points on faces
    
    // Index of points in blocks

}
  
void toyMesh3D::writePoints() const
{
}
}
