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

const int LINE_POINT_FIRST_ID = 0;
const int LINE_POINT_END_ID   = 1;

const int LINE_BOTTOM_0 = 0;
const int LINE_BOTTOM_1 = 1; 
const int LINE_BOTTOM_2 = 2;  
const int LINE_BOTTOM_3 = 3;   
  
const int LINE_MIDDLE_0 = 4;
const int LINE_MIDDLE_1 = 5; 
const int LINE_MIDDLE_2 = 6;  
const int LINE_MIDDLE_3 = 7;   
  
const int LINE_TOP_0 = 8;
const int LINE_TOP_1 = 9; 
const int LINE_TOP_2 = 10;  
const int LINE_TOP_3 = 11;    
  
const int faceLineId[6][4] =
{
  {LINE_BOTTOM_0, LINE_BOTTOM_3,  LINE_BOTTOM_2,  LINE_BOTTOM_1} ,
  {LINE_TOP_0   , LINE_TOP_1   ,  LINE_TOP_2   ,  LINE_TOP_3   } ,
  {LINE_BOTTOM_1, LINE_MIDDLE_2,  LINE_TOP_1   ,  LINE_MIDDLE_1} ,
  {LINE_BOTTOM_3, LINE_MIDDLE_0,  LINE_TOP_3   ,  LINE_MIDDLE_3} ,
  {LINE_BOTTOM_0, LINE_MIDDLE_1,  LINE_TOP_0   ,  LINE_MIDDLE_0} ,
  {LINE_BOTTOM_2, LINE_MIDDLE_3,  LINE_TOP_2   ,  LINE_MIDDLE_2}
};

const int FACE_BOTTOM = 0;
const int FACE_TOP    = 1;
const int FACE_RIGHT  = 2;
const int FACE_LEFT   = 3;
const int FACE_FRONT  = 4;
const int FACE_BEHIND = 5;
  
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
  for (int patchId = 1; patchId < sizePatches + 1; patchId++)
  {
    file >> boundaryFacesIndex[patchId];
    boundaryFacesIndex[patchId] += boundaryFacesIndex[patchId - 1];
  }

  boundaryFaces = new int[boundaryFacesIndex[sizePatches]][2];
  for (int patchId = 0; patchId < sizePatches; patchId++)
  {
    std::string patchName;
    file >> patchName;
    patchesName += patchName;
    patchesName.append("\n");

    for (int faceIdInPatch = boundaryFacesIndex[patchId]; faceIdInPatch < boundaryFacesIndex[patchId + 1]; faceIdInPatch++)
    {
      file >> boundaryFaces[faceIdInPatch][0];
      file >> boundaryFaces[faceIdInPatch][1];
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

          if (lineIdInBlock < 4 || lineIdInBlock >= 8)
            sizePoints -= (parBlocks[blockId].n[lineIdInBlock%2] - 1);
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
          if (blockFaces[faceIdInBlock][3] == faces[globalFaceId][(pointIdInFace + 1)%4])
            {
              blockFaces[faceIdInBlock][4] = -faces[globalFaceId][4];
              blockFaces[faceIfInBlock][5] = pointIdInFace;

              if (faceIdInBlock < 2)
                sizePoints -= (parBlocks[blockId].n[0] - 1)*(parBlocks[blockId].n[1] - 1);
              else if (faceIdInBlock < 4)
                sizePoints -= (parBlocks[blockId].n[1] - 1)*(parBlocks[blockId].n[2] - 1);
              else
                sizePoints -= (parBlocks[blockId].n[2] - 1)*(parBlocks[blockId].n[0] - 1);

              sizeFaces--;
              break;
            }
        }
      }
    }

    int trackingId = sizePointsOfBlocks;
    // Index of points on lines
    pointsOnLinesIndex    = new int[sizeLines + 1];
    pointsOnLinesIndex[0] = trackingId;
    for (int blockId = 0; blockId < sizeBlocks; blockId++)
      for (int lineIdInBlock = 0; lineIdInBlock < 12; lineIdInBlock++)
      {
        int globalLineId = lineIdInBlock + 12*blockId;
        if (lines[globalLineId][2] < 0)
          continue;

        int numPoints;
        if (lineIdInBlock < 4 || lineIdInBlock >= 8)
          numPoints = (parBlocks[blockId].n[lineIdInBlock%2] - 1);
        else
          numPoints = (parBlocks[blockId].n[2] - 1);

        trackingId += numPoints;
        pointsOnLinesIndex[lines[globalLineId][2]] = trackingId;
      }

    // Index of points on faces
    pointsOnFacesIndex    = new int[sizeFaces + 1];
    pointsOnFacesIndex[0] = trackingId;
    for (int blockId = 0; blockId < sizeBlocks; blockId++)
      for (int faceIdInBlock = 0; faceIdInBlock < 6; faceIdInBlock++)
      {
        int globalFaceId = faceIdInBlock + 6*blockId;
        if (faces[globalFaceId][4] < 0)
          continue;

        int numPoints;
        if (faceIdInBlock < 2)
          numPoints = (parBlocks[blockId].n[0] - 1)*(parBlocks[blockId].n[1] - 1);
        else if (faceIdInBlock < 4)
          numPoints = (parBlocks[blockId].n[1] - 1)*(parBlocks[blockId].n[2] - 1);
        else
          numPoints = (parBlocks[blockId].n[2] - 1)*(parBlocks[blockId].n[0] - 1);

        trackingId += numPoints;
        pointsOnFacesIndex[lines[globalFaceId][4]] = trackingId;
      }

    // Index of points in blocks
    pointsInBlocksIndex = new int[sizeBlocks];
    for (int blockId = 0; blockId < sizeBlocks; blockId++)
    {
      pointsInBlocksIndex[blockId] =trackingId;
      trackingId += parBlocks[blockId].SizeInnerPoints();
    }
}

void toyMesh3D::getPointsIdOfBlock(const int& blockId, int* pointsIdOfBlock) const
{
  const int& nX = parBlocks[blockId].n[0];
  const int& nY = parBlocks[blockId].n[1];
  const int& nZ = parBlocks[blockId].n[2];
  
  listLines3D blockLines = &lines[12*blockId];
  
  bool blockLinesSign[12];
  for (int lineIdInBlock = 0; lineIdInBlock < 4; lineIdInBlock++)
    blockLinesSign[lineIdInBlock] = 
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[(lineIdInBlock + 1)%4][LINE_POINT_FIRST_ID] ||
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[(lineIdInBlock + 1)%4][LINE_POINT_END_ID];
  
  for (int lineIdInBlock = 4; lineIdInBlock < 8; lineIdInBlock++)
    blockLinesSign[lineIdInBlock] = 
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[(lineIdInBlock + 4)][LINE_POINT_FIRST_ID] ||
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[(lineIdInBlock + 4)][LINE_POINT_END_ID];
  
  for (int lineIdInBlock = 8; lineIdInBlock < 12; lineIdInBlock++)
    blockLinesSign[lineIdInBlock] = 
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[8 + (lineIdInBlock + 1)%4][LINE_POINT_FIRST_ID] ||
      blockLines[lineIdInBlock][LINE_POINT_END_ID] == blockLines[8 + (lineIdInBlock + 1)%4][LINE_POINT_END_ID];
 
  // Corners points id
  if (blockLinesSign[LINE_BOTTOM_0])
  { 
    pointsIdOfBlock[0]  = blockLines[LINE_BOTTOM_0][LINE_POINT_FIRST_ID];
    pointsIdOfBlock[nX] = blockLines[LINE_BOTTOM_0][LINE_POINT_END_ID];    
  }
  else
  {
    pointsIdOfBlock[0]  = blockLines[LINE_BOTTOM_0][LINE_POINT_END_ID];
    pointsIdOfBlock[nX] = blockLines[LINE_BOTTOM_0][LINE_POINT_FIRST_ID];    
  }
  
  if (blockLinesSign[LINE_BOTTOM_2])
  { 
    pointsIdOfBlock[nY*(nX + 1)]      = blockLines[LINE_BOTTOM_2][LINE_POINT_END_ID];
    pointsIdOfBlock[nY*(nX + 1) + nX] = blockLines[LINE_BOTTOM_2][LINE_POINT_FIRST_ID];    
  }
  else
  {
    pointsIdOfBlock[nY*(nX + 1)]      = blockLines[LINE_BOTTOM_2][LINE_POINT_FIRST_ID];
    pointsIdOfBlock[nY*(nX + 1) + nX] = blockLines[LINE_BOTTOM_2][LINE_POINT_END_ID];    
  }
  
  if (blockLinesSign[LINE_TOP_0])
  { 
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1)]      = blockLines[LINE_TOP_0][LINE_POINT_FIRST_ID];
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nX] = blockLines[LINE_TOP_0][LINE_POINT_END_ID];    
  }
  else
  {
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1)]      = blockLines[LINE_TOP_0][LINE_POINT_END_ID];
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nX] = blockLines[LINE_TOP_0][LINE_POINT_FIRST_ID];    
  }
  
  if (blockLinesSign[LINE_TOP_2])
  { 
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1)]      = blockLines[LINE_TOP_2][LINE_POINT_END_ID];
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1) + nX] = blockLines[LINE_TOP_2][LINE_POINT_FIRST_ID];    
  }
  else
  {
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1)]      = blockLines[LINE_TOP_2][LINE_POINT_FIRST_ID];
    pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1) + nX] = blockLines[LINE_TOP_2][LINE_POINT_END_ID];    
  }
  
  // Lines points id
  int idLine = abs(blockLines[LINE_BOTTOM_0][2]) - 1;
  if (blockLinesSign[LINE_BOTTOM_0])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[linePointId - pointsOnLinesIndex[idLine] + 1] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[offset] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_BOTTOM_1][2]) - 1;
  if (blockLinesSign[LINE_BOTTOM_1])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[(linePointId - pointsOnLinesIndex[idLine] + 1)*(nX + 1) + nX] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[offset*(nX + 1) + nX] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_BOTTOM_2][2]) - 1;
  if (blockLinesSign[LINE_BOTTOM_2])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      {
        int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
        pointsIdOfBlock[nY*(nX + 1) + offset] = pointsOnLinesIndex[idLine + 1] - offset;
      }
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nY*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)] = linePointId;
  
  idLine = abs(blockLines[LINE_BOTTOM_3][2]) - 1;
  if (blockLinesSign[LINE_BOTTOM_3])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      {
        int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
        pointsIdOfBlock[offset*(nX + 1)] = pointsOnLinesIndex[idLine + 1] - offset;
      }
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[(linePointId - pointsOnLinesIndex + 1)*(nX + 1)] = linePointId; 
  
  idLine = abs(blockLines[LINE_MIDDLE_0][2]) - 1;
  if (blockLinesSign[LINE_MIDDLE_0])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[(linePointId - pointsOnLinesIndex[idLine] + 1)*(nY + 1)*(nX + 1) + nX] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[offset*(nY + 1)*(nX + 1) + nX] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_MIDDLE_1][2]) - 1;
  if (blockLinesSign[LINE_MIDDLE_1])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[(linePointId - pointsOnLinesIndex[idLine] + 1)*(nY + 1)*(nX + 1) + nY*(nX + 1) + nX] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[offset*(nY + 1)*(nX + 1) + nY*(nX + 1) + nX] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_MIDDLE_2][2]) - 1;
  if (blockLinesSign[LINE_MIDDLE_2])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[(linePointId - pointsOnLinesIndex[idLine] + 1)*(nY + 1)*(nX + 1) + nY*(nX + 1)] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[offset*(nY + 1)*(nX + 1) + nY*(nX + 1)] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_MIDDLE_3][2]) - 1;
  if (blockLinesSign[LINE_MIDDLE_3])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + offset] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_TOP_0][2]) - 1;
  if (blockLinesSign[LINE_TOP_0])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + offset] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_TOP_1][2]) - 1;
  if (blockLinesSign[LINE_TOP_1])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)*(nX + 1) + nX] = linePointId;
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
    {
      int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + offset*(nX + 1) + nX] = pointsOnLinesIndex[idLine + 1] - offset;
    }
  
  idLine = abs(blockLines[LINE_TOP_2][2]) - 1;
  if (blockLinesSign[LINE_TOP_2])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      {
        int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
        pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1) + offset] = pointsOnLinesIndex[idLine + 1] - offset;
      }
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + nY*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)] = linePointId;
  
  idLine = abs(blockLines[LINE_TOP_3][2]) - 1;
  if (blockLinesSign[LINE_TOP_3])
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      {
        int offset = linePointId - pointsOnLinesIndex[idLine] + 1;
        pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + offset*(nX + 1)] = pointsOnLinesIndex[idLine + 1] - offset;
      }
  else
    for (int linePointId = pointsOnLinesIndex[idLine]; linePointId < pointsOnLinesIndex[idLine + 1]; linePointId++)
      pointsIdOfBlock[nZ*(nY + 1)*(nX + 1) + (linePointId - pointsOnLinesIndex[idLine] + 1)*(nX + 1)] = linePointId;
                                                              
  // Faces points id
  
  // Internal points id
  for (int offsetZ = 1; offsetZ < nZ; offsetZ++)
    for (int offsetY = 1; offsetY < nY; offsetY++)
      for (int offsetX = 1; offsetX < nX; offsetX++)
        pointsIdOfBlock[offsetZ][offsetY][offsetX] = pointsInBlocksIndex[blockId] +
                                                     (offsetX - 1)                +       
                                                     (offsetY - 1)*(nX - 1)       +
                                                     (offsetZ - 1)*(nX - 1)*(nY - 1);
  
  // Write cells id
  for (int offsetZ = 0; offsetZ < nZ; offsetZ++)
    for (int offsetY = 0; offsetY < nY; offsetY++)
      for (int offsetX = 0; offsetX < nX; offsetX++)
      {
        file << pointsIdOfBlock[offsetZ][offsetY][offsetX]             << " ";
        file << pointsIdOfBlock[offsetZ][offsetY][offsetX + 1]         << " ";
        file << pointsIdOfBlock[offsetZ][offsetY + 1][offsetX + 1]     << " ";
        file << pointsIdOfBlock[offsetZ][offsetY + 1][offsetX]         << " ";
        file << pointsIdOfBlock[offsetZ + 1][offsetY][offsetX]         << " ";
        file << pointsIdOfBlock[offsetZ + 1][offsetY][offsetX + 1]     << " ";
        file << pointsIdOfBlock[offsetZ + 1][offsetY + 1][offsetX + 1] << " ";
        file << pointsIdOfBlock[offsetZ + 1][offsetY + 1][offsetX]     << "\n";
      }
}
  
void toyMesh3D::writePoints() const
{
}
}
