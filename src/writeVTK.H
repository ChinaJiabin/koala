#ifndef writeVTK_H
#define writeVTK_H

#include <fstream>
namespace Koala
{

void writeToyMeshVTK
(
  const parRun& Run ,
  int dimension
)
{
  std::ifstream filePoints;
  std::ifstream fileCells;
  Run.openFile(filePoints, "points", NULL, "mesh");
  Run.openFile(fileCells , "cells" , NULL, "mesh"); 
  
  int sizePoints, sizeCells, sizePointsPerCell;
  filePoints >> sizePoints;
  fileCells  >> sizeCells;
  fileCells  >> sizePointsPerCell;
  
  std::ofstream file;
  Run.openFile(file, "mesh", "vtu", "writeFiles");
  
  double doubleBuffer, intBuffer;
  // Write header
  file << "<?xml version='1.0'?>\n";
  file << "<VTKFile type='UnstructuredGrid' version='0.1' byte_order='LittleEndian'>\n";
  file << "<UnstructuredGrid>\n";
  file << "<Piece NumberOfPoints='" << sizePoints
       << "' NumberOfCells='"       << sizeCells << "'>\n";
  
  // Write points
  file << "<Points>\n";
  file << "<DataArray type='Float32' NumberOfComponents='3' format='ascii'>\n";
  for (int i = 0; i < sizePoints; i++)
  {
    for (int dim = 0; dim < dimension; dim++)
    {
      filePoints >> doubleBuffer;
      file << doubleBuffer << " ";
    }
    
    if (dimension == 2)
      file << "0 ";
  }
  file << "\n</DataArray>\n";
  file << "</Points>\n";
  
  // Write cells
}
}

#endif
