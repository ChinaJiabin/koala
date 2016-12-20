#include <string>
#include <vector>
#include <fstream>
#include <iostream>
namespace Koala
{
// Constant
const int LINE_BOTTOM = 0;
const int LINE_RIGHT  = 1;
const int LINE_TOP    = 2;
const int LINE_LEFT   = 3;

const int LINE_POINT_FIRST_ID = 0;
const int LINE_POINT_END_ID   = 1;

// Swap two int number
void intSwap(int& a, int& b)
{
  a ^= b;
  b ^= a;
  a ^= b;
}

parBlock2D::parBlock2D()
{
  type = new int[4](); // Suggest: Type use char instead of int
  n    = new int[2]();
}

toyMesh2D::toyMesh2D
(
  const parRun& Run_ ,
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
  for (int i = 1; i < 4*sizeBlocks + 1; i++)
  {
    file >> parLinesIndex[i];
    parLinesIndex[i] += parLinesIndex[i - 1];
  }

  parLines = new double [parLinesIndex[4*sizeBlocks]];
  for (int i = 0; i < parLinesIndex[4*sizeBlocks]; i++)
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

  // Loop blocks
  sizePoints     = sizePointsOfBlocks;
  sizeCells      = 0;
  sizeInnerFaces = 0;
  sizeLines      = 0;

  cellsIndex    = new int[sizeBlocks + 1];
  cellsIndex[0] = 0;

  lines = new int[4*sizeBlocks][5];

  for (int blockId = 0; blockId < sizeBlocks; blockId++)
  {
    sizePoints     += parBlocks[blockId].SizePoints() - 4;
    sizeCells      += parBlocks[blockId].SizeCells();
    sizeInnerFaces += parBlocks[blockId].SizeInnerFaces();

    cellsIndex[blockId + 1] = sizeCells;

    int *blockPointsId     = &blocks[4*blockId];
    listLines2D blockLines = &lines[4*blockId];
    for (int lineIdInBlock = 0; lineIdInBlock < 4; lineIdInBlock++)
    {
      blockLines[lineIdInBlock][LINE_POINT_FIRST_ID] = blockPointsId[lineIdInBlock];         // First points Id of line
      blockLines[lineIdInBlock][LINE_POINT_END_ID]   = blockPointsId[(lineIdInBlock + 1)%4]; // Second point Id of line
      blockLines[lineIdInBlock][2]                   = ++sizeLines;                          // Id
      blockLines[lineIdInBlock][3]                   = -1;                                   // Line Id(0,1,2,3) in neighbour block
      blockLines[lineIdInBlock][4]                   = -1;                                   // Neighbour block Id

      for (int globalId = 0; globalId < 4*blockId; globalId++)
        if (lines[globalId][LINE_POINT_FIRST_ID] == blockLines[lineIdInBlock][LINE_POINT_END_ID] &&
            lines[globalId][LINE_POINT_END_ID]   == blockLines[lineIdInBlock][LINE_POINT_FIRST_ID])
        {
          lines[globalId][3] = lineIdInBlock;
          lines[globalId][4] = blockId;

          blockLines[lineIdInBlock][2] = -lines[globalId][2];
          blockLines[lineIdInBlock][3] = globalId%4;
          blockLines[lineIdInBlock][4] = globalId/4;

          int subscript = lineIdInBlock%2;
          sizePoints     -= (parBlocks[blockId].n[subscript] - 1);
          sizeInnerFaces += parBlocks[blockId].n[subscript];

          sizeLines--;
          break;
        }
    }
  }

  // Index of points on lines
  pointsOnLinesIndex    = new int[sizeLines + 1];
  pointsOnLinesIndex[0] = sizePointsOfBlocks;
  int trackingId        = sizePointsOfBlocks;
  for (int = 0; i < 4*sizeBlocks; i++)
  {
    if (lines[i][2] < 0)
      continue;

    int numPoints = parBlocks[i/4].n[i%2] - 1;
    if (!numPoints)
    {
      pointsOnLinesIndex[lines[i][2]] = pointsOnLinesIndex[lines[i][2] - 1];
      continue;
    }

    trackingId += numPoints;
    pointsOnLinesIndex[lines[i][2]] = trackingId;
  }

  // Index of points in blocks
  pointsInBlocksIndex = new int[sizeBlocks + 1];
  for (int = 0; i < sizeBlocks; i++)
  {
    pointsInBlocksIndex[i] = trackingId;
    trackingId += parBlocks[i].SizeInnerPoints();
  }
  pointsInBlocksIndex[sizeBlocks] = trackingId;
}

void toyMesh2D::smoothPoint
(
  double& point_x ,
  double& point_y ,
  const double& point_x_right     , const double& point_y_right     ,
  const double& point_x_up        , const double& point_y_up        ,
  const double& point_x_left      , const double& point_y_left      ,
  const double& point_x_down      , const double& point_y_down      ,
  const double& point_x_right_up  , const double& point_y_right_up  ,
  const double& point_x_up_left   , const double& point_y_up_left   ,
  const double& point_x_left_down , const double& point_y_left_down ,
  const double& point_x_down_right, const double& point_y_down_right
) const
{
  double delta_x_xi  = point_x_right - point_x_left;
  double delta_x_eta = point_x_up    - point_x_down;

  double delta_y_xi  = point_y_right - point_y_left;
  double delta_y_eta = point_y_up    - point_y_down;

  double alpha = (delta_x_eta * delta_x_eta  + delta_y_eta * delta_y_eta)/8.0;
  double beta  = (delta_x_xi  * delta_x_eta  + delta_y_xi  * delta_y_eta)/16.0;
  double gamma = (delta_x_xi  * delta_x_xi   + delta_y_xi  * delta_y_xi)/8.0;

  double sumAlphaGamma = alpha + gamma;

  point_x = (
    alpha*(point_x_left + point_x_right) +
    gamma*(point_x_down + point_x_up)    -
    beta*(
      (point_x_right_up   + point_x_left_down) -
      (point_x_down_right + point_x_up_left)
    )
  )/sumAlphaGamma;

  point_y = (
    alpha*(point_y_left + point_y_right) +
    gamma*(point_y_down + point_y_up)    -
    beta*(
      (point_y_right_up   + point_y_left_down) -
      (point_y_down_right + point_y_up_left)
    )
  )/sumAlphaGamma;
}

double toyMesh2D::smoothBlockPoints
(
  const int& nX       ,
  const int& nY       ,
  double* coordinates ,
  double* residual    ,
  const int& numGap
) const
{
  int numPointsX     = (nX + 1);
  int numGapPointsX  = numGap*numPointsX;
  int numTotalPoints = numPointsX*(nY + 1);

  double error = 0;
  for (int i = numGap; i + numGap <= nY; i += numGap)
    for (int j = numGap; j + numGap <= nX; j += numGap)
    {
      // 0. 9 around points address
      int id_x = j + i*numPointsX;
      int id_y = id_x + numTotalPoints;

      int id_x_right = id_x + numGap;
      int id_x_left  = id_x - numGap;
      int id_x_up    = id_x + numGapPointsX;
      int id_x_down  = id_x - numGapPointsX;

      int id_x_right_up   = id_x_up + numGap;
      int id_x_left_up    = id_x_up - numGap;
      int id_x_right_down = id_x_down + numGap;
      int id_x_left_down  = id_x_down - numGap;


      // Update
      double coordinateOld_x = coordinates[id_x];
      double coordinateOld_y = coordinates[id_y];

      smoothPoint
      (
        coordinates[id_x] ,
        coordinates[id_y] ,
        coordinates[id_x_right]     , coordinates[id_x_right + numTotalPoints] ,
        coordinates[id_x_up]        , coordinates[id_y_up    + numTotalPoints] ,
        coordinates[id_x_left]      , coordinates[id_y_left  + numTotalPoints] ,
        coordinates[id_x_down]      , coordinates[id_y_down  + numTotalPoints] ,
        coordinates[id_x_right_up]  , coordinates[id_x_right_up   + numTotalPoints] ,
        coordinates[id_x_up_left]   , coordinates[id_y_up_left    + numTotalPoints] ,
        coordinates[id_x_left_down] , coordinates[id_y_left_down  + numTotalPoints] ,
        coordinates[id_x_down_right], coordinates[id_y_down_right + numTotalPoints]
      );

      residual[id_x] = coordinates[id_x] - coordinateOld_x;
      residual[id_y] = coordinates[id_y] - coordinateOld_y;

      error += residual[id_x]*residual[id_x] + residual[id_y]*residual[id_y];
    }
  return error;
}

double toyMesh2D::restriction
(
  const int& nX    ,
  const int& nY    ,
  double* residual ,
  const int& numGap
) const
{
  int numPointsX     = (nX + 1);
  int numGapPointsX  = numGap*numPointsX;
  int numTotalPoints = numPointsX*(nY + 1);

  for (int i = numGap; i + numGap <= nY; i += numGap)
    for (int j = numGap; j + numGap <= nX; j += numGap)
    {
      // 0. 9 around points address
      int id_x = j + i*numPointsX;

      int id_x_right = id_x + numGap;
      int id_x_left  = id_x - numGap;
      int id_x_up    = id_x + numGapPointsX;
      int id_x_down  = id_x - numGapPointsX;

      int id_x_right_up   = id_x_up + numGap;
      int id_x_left_up    = id_x_up - numGap;
      int id_x_right_down = id_x_down + numGap;
      int id_x_left_down  = id_x_down - numGap;

      residual[id_x] = (
        residual[id_x] + (
          residual[id_x_right] +
          residual[id_x_left]  +
          residual[id_x_up]    +
          residual[id_x_down]
        )/2.0 + (
          residual[id_x_right_up]   +
          residual[id_x_left_up]    +
          residual[id_x_right_down] +
          residual[id_x_left_down]
        )/4.0
      )/4.0;

      residual[id_x + numTotalPoints] = (
        residual[id_x + numTotalPoints] + (
          residual[id_x_right + numTotalPoints] +
          residual[id_x_left  + numTotalPoints] +
          residual[id_x_up    + numTotalPoints] +
          residual[id_x_down  + numTotalPoints]
        )/2.0 + (
          residual[id_x_right_up   + numTotalPoints] +
          residual[id_x_left_up    + numTotalPoints] +
          residual[id_x_right_down + numTotalPoints] +
          residual[id_x_left_down  + numTotalPoints]
        )/4.0
      )/4.0;
    }
}

double toyMesh2D::prolongation
(
  const int& nX    ,
  const int& nY    ,
  double* residual ,
  const int& numGap
) const
{
  int halfNumGap     = numGap/2.0;
  int numPointsX     = (nX + 1);
  int numTotalPoints = numPointsX*(nY + 1);

  for (int i = halfNumGap; i + halfNumGap <= nY; i += halfNumGap)
    for (int j = halfNumGap; j + halfNumGap <= nX; j += halfNumGap)
    {
      int id_x = j + i*numPointsX;
      if (i%numGap && j%numGap)
      {
        int id_x_right_up   = id_x + halfNumGap*(1 + numPointsX);
        int id_x_left_up    = id_x_right_up - numGap;
        int id_x_right_down = id_x + halfNumGap*(1 - numPointsX);
        int id_x_left_down  = id_x_right_down - numGap;

        for (int dim = 0; dim < 2; dim++)
          residual[id_x + dim*numTotalPoints] = (
            residual[id_x_right_up   + dim*numTotalPoints] +
            residual[id_x_left_up    + dim*numTotalPoints] +
            residual[id_x_right_down + dim*numTotalPoints] +
            residual[id_x_left_down  + dim*numTotalPoints]
          )/4.0;
      }
      else if (i%numGap)
      {
        int id_x_up   = id_x + halfNumGap*numPointsX;
        int id_x_down = id_x - halfNumGap*numPointsX;

        for (int dim = 0; dim < 2; dim++)
          residual[id_x + dim*numTotalPoints] = (
            residual[id_x_up   + dim*numTotalPoints] +
            residual[id_x_down + dim*numTotalPoints]
          )/2.0;
      }
      else if (j%numGap)
      {
        int id_x_right = id_x + halfNumGap;
        int id_x_left  = id_x_right - numGap;

        for (int dim = 0; dim < 2; dim++)
          residual[id_x + dim*numTotalPoints] = (
            residual[id_x_right + dim*numTotalPoints] +
            residual[id_x_left  + dim*numTotalPoints]
          )/2.0;
      }
    }
}

void toyMesh2D::writePoints() const
{
  // Get innerLinesPointsId
  int numInnerLines       = 0;
  int numInnerLinesPoints = 0;
  for (int i = 0; i < 4*sizeBlocks; i++)
  {
    int lineId = lines[i][2] - 1;
    if (lineId < 0 || lines[i][3] == -1)
      continue;

    numInnerLines++;
    numInnerLinesPoints += pointsOnLinesIndex[lineId + 1] - pointsOnLinesIndex[lineId];
  }
  int innerLinesPointsId[numInnerLinesPoints + 2*numInnerLines][3];

  int trackingId = 0;
  for (int blockId = 0; blockId < sizeBlocks; blockId++)
    for (int lineIdInBlock = 0; lineIdInBlock < 4; lineIdInBlock++)
    {
      int globalId = lineIdInBlock + 4*blockId;

      // 0.
      int lineId = lines[globalId][2] - 1;
      if (lineId < 0 || lines[globalId][3] == -1)
        continue;

      int (*linePointsId)[3] = &innerLinesPointsId[trackingId];
      int numPoints = pointsOnLinesIndex[lineId + 1] - pointsOnLinesIndex[lineId];
      trackingId += numPoints + 2;

      linePointsId[0][0]             = lines[globalId][0];
      linePointsId[numPoints + 1][0] = lines[globalId][1];

      for (int offset = 1; offset <= numPoints; offset++)
        linePointsId[offset][0] = pointsOnLinesIndex[lineId] + (offset - 1);

      // 1. Case owner block
      {
        int idBefore     = globalId + (lineIdInBlock == 0 ? 3 : -1);
        int lineIdBefore = abs(lines[idBefore][2]) - 1;

        if (pointsOnLinesIndex[lineIdBefore] != pointsOnLinesIndex[lineIdBefore + 1])
        {
          if (lines[idBefore][2] > 0)
            linePointsId[0][1] = pointsOnLinesIndex[lineIdBefore + 1] - 1;
          else
            linePointsId[0][1] = pointsOnLinesIndex[lineIdBefore];

          int idAfter     = globalId + (lineIdInBlock == 3 ? -3 : 1);
          int lineIdAfter = abs(lines[idAfter][2]) - 1;

          if (lines[idAfter][2] > 0)
            linePointsId[numPoints + 1][1] = pointsOnLinesIndex[lineIdAfter];
          else
            linePointsId[numPoints + 1][1] = pointsOnLinesIndex[lineIdAfter + 1] - 1;

          switch (lineIdInBlock)
          {
            case LINE_BOTTOM:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][1] = pointsInBlocksIndex[blockId] + (offset - 1);
              break;

            case LINE_LEFT:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][1] = pointsInBlocksIndex[blockId] + (offset*(parBlocks[blockId].n[0] - 1) - 1);
              break;

            case LINE_TOP:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][1] = pointsInBlocksIndex[blockId + 1] - offset;
              break;

            case LINE_RIGHT:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][1] = pointsInBlocksIndex[blockId + 1] - offset*(parBlocks[blockId].n[0] - 1);
              break;
          }
        }
        else
        {
          int idOpposite     = globalId + (lineIdInBlock < 2 ? 2 : -2);
          int lineIdOpposite = abs(lines[idOpposite][2]) - 1;

          linePointsId[0][1]             = lines[idOpposite][1];
          linePointsId[numPoints + 1][1] = lines[idOpposite][0];

          if (lines[idOpposite][2] > 0)
            for (int offset = 1; offset <= numPoints; offset++)
              linePointsId[offset][1] = pointsOnLinesIndex[lineIdOpposite + 1] - offset;
          else
            for (int offset = 1; offset <= numPoints; offset++)
              linePointsId[offset][1] = pointsOnLinesIndex[lineIdOpposite] + (offset - 1);
        }
      }

      // 2. Case neighbour block
      {
        const int& lineIdInNeighbourBlock = lines[globalId][3];
        const int& neighbourBlockId       = lines[globalId][4];
        int numLine                       = 4*neighbourBlockId + lineIdInNeighbourBlock;


        int idBefore     = numLine + (lineIdInNeighbourBlock == 0 ? 3 : -1);
        int lineIdBefore = abs(lines[idBefore][2]) - 1;

        if (pointsOnLinesIndex[lineIdBefore] != pointsOnLinesIndex[lineIdBefore + 1])
        {
          if (lines[idBefore][2] > 0)
            linePointsId[numPoints + 1][2] = pointsOnLinesIndex[lineIdBefore + 1] - 1;
          else
            linePointsId[numPoints + 1][2] = pointsOnLinesIndex[lineIdBefore];

          int idAfter     = numLine + (lineIdInNeighbourBlock == 3 ? -3 : 1);
          int lineIdAfter = abs(lines[idAfter][2]) - 1;

          if (lines[idAfter][2] > 0)
            linePointsId[0][2] = pointsOnLinesIndex[lineIdAfter];
          else
            linePointsId[0][2] = pointsOnLinesIndex[lineIdAfter + 1] - 1;

          switch (lineIdInBlock)
          {
            case LINE_BOTTOM:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][2] = pointsInBlocksIndex[blockId] + (numPoints - offset);
              break;

            case LINE_LEFT:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][2] = pointsInBlocksIndex[blockId + 1] - ((offset - 1)*(parBlocks[blockId].n[0] - 1) + 1);
              break;

            case LINE_TOP:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][2] = pointsInBlocksIndex[blockId + 1] - (numPoints - offset + 1);
              break;

            case LINE_RIGHT:
              for (int offset = 1; offset <= numPoints; offset++)
                linePointsId[offset][2] = pointsInBlocksIndex[blockId] + (offset - 1)*(parBlocks[blockId].n[0] - 1);
              break;
          }
        }
        else
        {
          int idOpposite     = numLine + (lineIdInNeighbourBlock < 2 ? 2 : -2);
          int lineIdOpposite = abs(lines[idOpposite][2]) - 1;

          linePointsId[0][2]             = lines[idOpposite][0];
          linePointsId[numPoints + 1][2] = lines[idOpposite][1];

          if (lines[idOpposite][2] > 0)
            for (int offset = 1; offset <= numPoints; offset++)
              linePointsId[offset][2] = pointsOnLinesIndex[lineIdOpposite] + (offset - 1);
          else
            for (int offset = 1; offset <= numPoints; offset++)
              linePointsId[offset][2] = pointsOnLinesIndex[lineIdOpposite + 1] - offset;
        }
      }
    }

  // Inner block points Id
  int innerBlockPointsId[sizePointsOfBlocks];
  memset(innerBlockPointsId, 0, sizePointsOfBlocks*sizeof(int));

  for (int i = 0; i < 4*sizeBlocks; i++)
    if (lines[i][3] == -1)
    {
      innerBlockPointsId[lines[i][0]] = -1;
      innerBlockPointsId[lines[i][1]] = -1;
    }

  int sizeInnerBlockPoints = 0;
  for (int i = 0; i < sizePointsOfBlocks; i++)
    if (innerBlockPointsId[i] == 0)
      innerBlockPointsId[i] = sizeInnerBlockPoints++;

  trackingId = 0;
  std::vector<int> commonPointsId[sizeInnerBlockPoints];
  for (int i = 0; i < 4*sizeBlocks; i++)
  {
    int lineId = lines[i][2] - 1;
    if (lineId < 0 || lines[i][3] == -1)
      continue;

    int (*linePointsId)[3] = &innerLinesPointsId[trackingId];
    int numPoints = pointsOnLinesIndex[lineId + 1] - pointsOnLinesIndex[lineId];
    tracking += numPoints + 2;

    if (innerBlockPointsId[lines[i][0]] != -1)
    {
      if (commonPointsId[innerBlockPointsId[lines[i][0]]].empty())
        commonPointsId[innerBlockPointsId[lines[i][0]]].push_back(lines[i][0]);

      commonPointsId[innerBlockPointsId[lines[i][0]]].push_back(linePointsId[1][1]);
      commonPointsId[innerBlockPointsId[lines[i][0]]].push_back(linePointsId[1][0]);
      commonPointsId[innerBlockPointsId[lines[i][0]]].push_back(linePointsId[1][2]);
    }

    if (innerBlockPointsId[lines[i][1]] != -1)
    {
      if (commonPointsId[innerBlockPointsId[lines[i][1]]].empty())
        commonPointsId[innerBlockPointsId[lines[i][1]]].push_back(lines[i][1]);

      commonPointsId[innerBlockPointsId[lines[i][1]]].push_back(linePointsId[numPoints][1]);
      commonPointsId[innerBlockPointsId[lines[i][1]]].push_back(linePointsId[numPoints][0]);
      commonPointsId[innerBlockPointsId[lines[i][1]]].push_back(linePointsId[numPoints][2]);
    }
  }

  for (int i = 0; i < sizeInnerBlockPoints; i++)
  {
    // Reorder
    int startId = 3;
    while (startId < commonPointsId[i].size())
    {
      for (int j = startId + 1; j < commonPointsId[i].size(); j++)
        if (commonPointsId[i][startId] == commonPointsId[i][j])
        {
          if (j == startId + 3)
            intSwap(commonPointsId[i][startId + 1], commonPointsId[i][startId + 3]);

          else if (j > startId + 3 && j%3 == 1)
            for (int k = 1; k <= 3; k++)
              intSwap(commonPointsId[i][startId + k], commonPointsId[i][j + k - 1]);

          else if (j%3 == 0)
            for (int k = 1; k <= 3; k++)
              intSwap(commonPointsId[i][startId + k], commonPointsId[i][j + 1 - k]);

          break;
        }
      startId += 3;
    }

    if (commonPointsId[i][1] != commonPointsId[i][commonPointsId[i].size() - 1])
      intSwap(commonPoints[i][commonPointsId[i].size() - 1], commonPoints[i][commonPointsId[i].size() - 3]);
  }

  // New points array
  double points[sizePoints][2];

  // Block points
  for (int i = 0; i < sizePointsOfBlocks; i++)
    for (int j = 0; j < 2; j++)
      points[i][j] = pointsOfBlocks[i][j];

  // Points on lines
  for (int blockId = 0; blockId < sizeBlocks; blockId++)
    for (int lineIdInBlock = 0; lineIdInBlock < 4; lineIdInBlock++)
    {
      int globalId = lineIdInBlock + 4*blockId;
      int lineId   = lines[globalId][2] - 1;
      if (lineId < 0 || (pointsOnLinesIndex[lineId] == pointsOnLinesIndex[lineId + 1]))
        continue;

      switch (parBlocks[blockId].type[lineIdInBlock])
      {
        case 0:
          lineMap1D
          (
            &points[lines[i][0]][0]                ,
            &points[lines[i][1]][0]                ,
            &points[pointsOnLinesIndex[lineId]][0] ,
            numPoints                              ,
            2                                      ,
            parLines[parLinesIndex[i]]             ,
            parLines[parLinesIndex[i] + 1]
          );
          break;

        case 1:
          arcMap1D
          (
            &points[lines[i][0]][0]                ,
            &points[lines[i][1]][0]                ,
            &points[pointsOnLinesIndex[lineId]][0] ,
            numPoints                              ,
            parLines[parLinesIndex[i]]             ,
            parLines[parLinesIndex[i] + 1]         ,
            parLines[parLinesIndex[i] + 2]
          );
          break;

        case 2:
          givenPointsMap1D
          (
            &points[pointsOnLinesIndex[lineId]][0] ,
            &parLines[parLinesIndex[i]]            ,
            parLinesIndex[i + 1] - parLinesIndex[i]
          );
          break;
      }
    }

  //2. Create points in blocks
  double tolerance = 1e-5;
  double error     = 0;
  int numIter      = -1;
  while (1)
  {
    error      = 0;
    trackingId = 0;
    for (int blockId = 0; blockId < sizeBlocks; blockId++)
    {
      const int& nX = parBlocks[blockId].n[0];
      const int& nY = parBlocks[blockId].n[1];

      listLines2D blockLines   = &lines[4*blockId];
      listPoints2D blockPoints = &points[pointsInBlocksIndex[blockId]];
      double coordinates[2*(nY + 1)][(nX + 1)];

      // Set boundary points
      // Bottom
      const double* bottom = &points[pointsOnLinesIndex[abs(blockLines[LINE_BOTTOM][2]) - 1]][0];
      if (blockLines[LINE_BOTTOM][2] >= 0)
        for (int offset = 1; offset < nX; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[dim*(nY + 1)][offset] = bottom[dim + 2*(offset - 1)];
      else
        for (int offset = 1; offset < nX; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[dim*(nY + 1)][offset] = bottom[dim + 2*(nX - 1 - offset)];

      // Right
      const double* right = &points[pointsOnLinesIndex[abs(blockLines[LINE_RIGHT][2]) - 1]][0];
      if (blockLines[LINE_RIGHT][2] >= 0)
        for (int offset = 1; offset < nY; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[offset + dim*(nY + 1)][nX] = right[dim + 2*(offset - 1)];
      else
        for (int offset = 1; offset < nY; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[offset + dim*(nY + 1)][nX] = right[dim + 2*(nY - 1 - offset)];

      // Top
      const double* top = &points[pointsOnLinesIndex[abs(blockLines[LINE_TOP][2]) - 1]][0];
      if (blockLines[LINE_TOP][2] >= 0)
        for (int offset = 1; offset < nX; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[(dim + 1)*(nY + 1) - 1][offset] = top[dim + 2*(nX - 1 - offset)];
      else
        for (int offset = 1; offset < nX; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[(dim + 1)*(nY + 1) - 1][offset] = top[dim + 2*(offset - 1)];

      // Left
      const double* left = &points[pointsOnLinesIndex[abs(blockLines[LINE_LEFT][2]) - 1]][0];
      if (blockLines[LINE_LEFT][2] >= 0)
        for (int offset = 1; offset < nY; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[offset + dim*(nY + 1)][0] = left[dim + 2*(nY - 1 - offset)];
      else
        for (int offset = 1; offset < nY; offset++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[offset + dim*(nY + 1)][0] = left[dim + 2*(offset - 1)];

      // Four corner points
      for (int dim = 0; dim < 2; dim++)
      {
        coordinates[dim*(nY + 1)][0]       = points[blockLines[LINE_BOTTOM][LINE_POINT_FIRST_ID]][i];
        coordinates[dim*(nY + 1)][nX]      = points[blockLines[LINE_BOTTOM][LINE_POINT_END_ID]][i];
        coordinates[nY + dim*(nY + 1)][nX] = points[blockLines[LINE_TOP][LINE_POINT_FIRST_ID]][i];
        coordinates[nY + dim*(nY + 1)][0]  = points[blockLines[LINE_TOP][LINE_POINT_END_ID]][i];
      }

      // Set initial value by calculating the intersection point of two lines
      if (numIter == -1)
      {
        for (int offsetY = 1; offsetY < nY; offsetY++)
        {
          const double& x1 = coordinates[offsetY][0];
          const double& y1 = coordinates[offsetY + (nY + 1)][0];

          const double& X2 = coordinates[offsetY][nX];
          const double& y2 = coordinates[offsetY + (nY + 1)][nX];

          for (int offsetX = 1; offsetX < nX; offsetX++)
          {
            const double& x3 = coordinates[0][offsetY];
            const double& y3 = coordinates[nY + 1][offsetY];

            const double& x4 = coordinates[nY][offsetY];
            const double& y4 = coordinates[nY + (nY + 1)][offsetY];

            double denominator = (x1 -x2)*(y3 - y4) + (x3 -x4)*(y2 - y1);

            blockPoints[(offsetX - 1) + (offsetY - 1)*(nX - 1)][0] = (
              x1*x3*(y2 - y4) + x2*x3*(y4 - y1) +
              x1*x4*(y3 - y2) + x2*x4*(y1 - y3)
            )/denominator;

            blockPoints[(offsetX - 1) + (offsetY - 1)*(nX - 1)][1] = (
              y1*y3*(x4 - x2) + y2*y3*(x1 - x4) +
              y1*y4*(x2 - x3) + y2*y4*(x3 - x1)
            )/denominator;
          }
        }
        continue;
      }

      // Initialize coordinates
      for (int offsetY = 1; offsetY < nY; offsetY++)
        for (int offsetX = 1; offsetX < nX; offsetX++)
          for (int dim = 0; dim < 2; dim++)
            coordinates[offsetY + dim*(nY + 1)][offsetX] =
              blockPoints[(offsetX - 1) + (offsetY - 1)*(nX - 1)][dim];

      // Key part
      // :0. Initialize residual
      double residual[2*(nY + 1)][nX + 1];
      memset(residual, 0, 2*(nY + 1)*(nX + 1)*sizeof(double));

      // :1. Smooth on original grid
      error += smoothBlockPoints(nX, nY, &coordinates[0][0], &residual[0][0]);

      // :2. Restriction operation
      restriction(nX, nY, &residual[0][0]);

      // :3. Prolongation operation
      prolongation(nX, nY, &residual[0][0]);

      // :4. Update blockPoints
      for (int offsetY = 1; offsetY < nY; offsetY++)
        for (int offsetX = 1; offsetX < nX; offsetX++)
          for (int dim = 0; dim < 2; dim++)
            blockPoints[(offsetX - 1) + (offsetY - 1)*(nX - 1)][dim] =
              coordinates[offsetY + dim*(nY + 1)][offsetX] + residual[offsetY + dim*(nY + 1)][offsetX];

      // Smooth points on block lines that is not belong to boundary
      for (int lineIdInBlock = 0; lineIdInBlock < 4; lineIdInBlock++)
      {
        int lineId = blockLines[lineIdInBlock][2] - 1;
        if (lineId < 0 || blockLines[lineIdInBlock][3] == -1)
          continue;

        int (*linePointsId)[3] = &innerLinesPointsId[trackingId];
        int numPoints = pointsOnLinesIndex[lineId + 1] - pointsOnLinesIndex[lineId];
        trackingId   += numPoints + 2;

        for (int offset = 1; offset <= numPoints; offset++)
        {
          double pointOldX = points[linePointsId[offset][0]][0];
          double pointOldY = points[linePointsId[offset][0]][1];

          smoothPoint
          (
            points[linePointsId[offset][0]][0]    , points[linePointsId[offset][0]][1]     ,
            points[linePointsId[offset][2]][0]    , points[linePointsId[offset][2]][1]     ,
            points[linePointsId[offset + 1][0]][0], points[linePointsId[offset + 1][0]][1] ,
            points[linePointsId[offset][1]][0]    , points[linePointsId[offset][1]][1]     ,
            points[linePointsId[offset - 1][0]][0], points[linePointsId[offset - 1][0]][1] ,
            points[linePointsId[offset + 1][2]][0], points[linePointsId[offset + 1][2]][1] ,
            points[linePointsId[offset + 1][1]][0], points[linePointsId[offset + 1][1]][1] ,
            points[linePointsId[offset - 1][1]][0], points[linePointsId[offset - 1][1]][1] ,
            points[linePointsId[offset - 1][2]][0], points[linePointsId[offset - 1][2]][1]
          );

          double residualX = pointOldX - points[linePointsId[offset][0]][0];
          double residualY = pointOldY - points[linePointsId[offset][0]][1];

          error += residualX*residualX + residualY*residualY;
        }
      }
    }

    // Smooth end points of lines and the points is not belong to boundary
    for (int i = 0; i < sizeInnerBlockPoints; i++)
      if (commonPointsId[i].size() == 13)
      {
        double pointOldX = points[commonPointsId[i][0]][0];
        double pointOldY = points[commonPointsId[i][0]][1];

        smoothPoint
        (
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1] ,
          points[commonPointsId[i][0]][0], points[commonPointsId[i][0]][1]
        );

        double residualX = pointOldX - points[commonPointsId[i][0]][0];
        double residualY = pointOldY - points[commonPointsId[i][0]][1];

        error += residualX*residualX + residualY*residualY;
      }
      else
      {
      }

    if (++numIter > 1e4)
    {
      std::cout << "Reach the maximum number of iteration for map2D!" << std::endl;
      break;
    }

    if (numIter == 0)
      continue;

    if (sqrt(error) < tolerance)
      break;
  }

  std::cout << "Number of iteration = " << numIter
            << " Error = " << error << std::endl;

  // Write
  std::ofstream file;
  Run.openFile(file, "points", NULL, filesPath);

  file << sizePoints << "\n";
  for (int pointId = 0; pointId < sizePoints; pointId++)
    file << points[pointId][0] << " " << points[pointId][1] << "\n";
}

void toyMesh2D::writeCells() const
{
  std::ofstream file;
  Run.openFile(file, "cells", NULL, filesPath);

  file << sizeCells << " 4" << "\n";

  for (int blockId = 0; blockId < sizeBlocks; blockId++)
  {
    listLines2D blockLines = &lines[4*blockId];
    int idPointsOfBlock[parBlocks[blockId].n[1] + 1][parBlocks[blockId].n[0] + 1];
    
    // Corner points
    idPointsOfBlock[0][0]                                             = lines[LINE_BOTTOM][LINE_POINT_FIRST_ID];
    idPointsOfBlock[0][parBlocks[blockId].n[0]]                       = lines[LINE_BOTTOM][LINE_POINT_END_ID];
    idPointsOfBlock[parBlocks[blockId].n[1]][parBlocks[blockId].n[0]] = lines[LINE_TOP][LINE_POINT_FIRST_ID];
    idPointsOfBlock[parBlocks[blockId].n[1]][0]                       = lines[LINE_TOP][LINE_POINT_END_ID];
    
    // Lines points
    // Bottom
    int lineId = abs(lines[LINE_BOTTOM][2]) - 1;
    for (int i = pointsOnLinesIndex[lineId]; i < pointsOnLinesIndex[lineId + 1]; i++)
      if (lines[LINE_BOTTOM][2] < 0)
        idPointsOfBlock[0][i - pointsOnLinesIndex[lineId] + 1] =
          (pointsOnLinesIndex[lineId + 1] - 1) - (i - pointsOnLinesIndex[lineId]);
      else
        idPointsOfBlock[0][i - pointsOnLinesIndex[lineId] + 1] = i;
    
    // Right
    int lineId = abs(lines[LINE_RIGHT][2]) - 1;
    for (int i = pointsOnLinesIndex[lineId]; i < pointsOnLinesIndex[lineId + 1]; i++)
      if (lines[LINE_RIGHT][2] < 0)
        idPointsOfBlock[i - pointsOnLinesIndex[lineId] + 1][parBlocks[blockId].n[0]] =
          (pointsOnLinesIndex[lineId + 1] - 1) - (i - pointsOnLinesIndex[lineId]);
      else
        idPointsOfBlock[i - pointsOnLinesIndex[lineId] + 1][parBlocks[blockId].n[0]] = i;
    
    // Top
    int lineId = abs(lines[LINE_TOP][2]) - 1;
    for (int i = pointsOnLinesIndex[lineId]; i < pointsOnLinesIndex[lineId + 1]; i++)
      if (lines[LINE_TOP][2] > 0)
        idPointsOfBlock[parBlocks[blockId].n[1]][i - pointsOnLinesIndex[lineId] + 1] =
          (pointsOnLinesIndex[lineId + 1] - 1) - (i - pointsOnLinesIndex[lineId]);
      else
        idPointsOfBlock[parBlocks[blockId].n[1]][i - pointsOnLinesIndex[lineId] + 1] = i;
    
    // Left
    int lineId = abs(lines[LINE_LEFT][2]) - 1;
    for (int i = pointsOnLinesIndex[lineId]; i < pointsOnLinesIndex[lineId + 1]; i++)
      if (lines[LINE_LEFT][2] > 0)
        idPointsOfBlock[i - pointsOnLinesIndex[lineId] + 1][0] =
          (pointsOnLinesIndex[lineId + 1] - 1) - (i - pointsOnLinesIndex[lineId]);
      else
        idPointsOfBlock[i - pointsOnLinesIndex[lineId] + 1][0] = i;
                                                 
    // Internal points
    for (int offsetY = 1; offsetY < parBlocks[blockId].n[1]; offsetY++)
      for (int offsetX = 1; offsetX < parBlocks[blockId].n[0]; offsetX++)
        idPointsOfBlock[offsetY][offsetX] = 
          pointsInBlocksIndex[blockId] + (offsetX - 1) + (parBlocks[blockId].n[0] - 1)*(offsetY - 1);
    
    // Write cells
    for (int offsetY = 0; offsetY < parBlocks[blockId].n[1]; offsetY++)   
      for (int offsetX = 0; offsetX < parBlocks[blockId].n[0]; offsetX++)
      {
        file << idPointsOfBlock[offsetY][offsetX]         << " ";
        file << idPointsOfBlock[offsetY][offsetX + 1]     << " ";
        file << idPointsOfBlock[offsetY + 1][offsetX + 1] << " ";
        file << idPointsOfBlock[offsetY + 1][offsetX]     << "\n";
      }
  }
}

void toyMesh2D::writeFaces() const
{
}

void toyMesh2D::writeBoundaryPointsId() const
{
}

void toyMesh2D::writeBoundaryFacesId() const
{
}
