#ifndef baseToyMesh_H
#define baseToyMesh_H

#include "parRun.H"
namespace Koala
{

class parBlock
{
public:
        /*
          Type line:
            0: Straight line
            1: Circular
            2: Line determined by points given
        */
        int* type;
        int* n;
};

typedef int (*listBoundaryFaces)[2];

class baseToyMesh
{
protected:
        // Path parameters
        const parRun& Run;
        const char* inputFileName;
        const char* filesPath;

        // Blocks parameters
        int sizePointsOfBlocks;
        int sizeBlocks;
        
        double* parLines;
        int* parLinesIndex;
        
        // Mesh parameters
        int sizePoints;
        int sizeCells;
        int sizeLines;
        int sizeInnerFaces;
        
        int* pointsOnLinesIndex;
        int* pointsInBlocksIndex;
        int* cellsIndex;
        
        int sizePatches;
        std::string patchesName;
 
        listBoundaryFaces boundaryFaces;
        int* boundaryFacesIndex;

public:
    // Constructor

        baseToyMesh
        (
          const parRun& Run_         ,
          const char* inputFileName_ ,  
          const char* filesPath_ = "mesh"
        );  
      
    // Write

        virtual void writePoints() const = 0;  
        virtual void writeCells()  const = 0; 
        virtual void writeFaces()  const = 0;  

        virtual void writeBoundary()         const;
        virtual void writeBoundaryPointsId() const = 0;
        virtual void writeBoundaryFacesId()  const = 0;

protected:
    // Functions used to create points
};
}

#endif
