#ifndef writeVTK_H
#define writeVTK_H

#include "baseMesh.H"
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
  file << "<Cells>\n";
  file << "<DataArray type='Int32' Name='connectivity' format='ascii'>\n";
  
  int cellVtkType = baseMesh::CellVtkType(sizePointsPerCell, dimension);
  for (int i = 0; i < sizeCells; i++)
    for (int j = 0; j < sizePointsPerCell; j++)
    {
      fileCells >> intBuffer;
      file << intBuffer << " ";
    }
  file << "\n</DataArray>\n";
  
  file << "<DataArray type='Int32' Name='offsets' format='ascii'>\n";
  for (int i = 0; i < sizeCells; i++)
    file << sizePointsPerCell*(i + 1) << " ";
  file << "\n</DataArray>\n";
  
  file << "<DataArray type='UInt8' Name='types' format='ascii'>\n";
    for (int i = 0; i < sizeCells; i++)
      file << cellVtkType << " ";
  file << "\n</DataArray>\n";
  
  file << "</Cells>\n";
  file << "</Piece>\n";
  file << "</UnstructuredGrid>\n";
  file << "</VTKFile>\n";
}
}

#endif
