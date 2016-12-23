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

        virtual void writeBoundaryName()     const;
        virtual void writeBoundaryPointsId() const = 0;
        virtual void writeBoundaryFaces()  const = 0;

protected:
    // Functions used to create points
    // size: Number of points inside line

        /*
          Points distribution on [0, 1] interval
          Type:
                0: Equal ratio distribution
                1: Central equal ratio distribution
        */
        void equalRatioDistribution1D
        (
          double* map         ,
          const int& size     ,
          const double& ratio ,
          const int& type = 0
        ) const;

        void lineMap1D
        (
          const double* startPoint ,
          const double* endPoint   ,
          double* points           ,
          const int& size          ,
          const int& dim           ,
          const double& ratio = 0  ,
          const int& type = 0
        ) const;

        void arcMap1D
        (
          const double* startPoint ,
          const double* endPoint   ,
          double* points           ,
          const int& size          ,
          const double& angle      ,
          const double& ratio = 0  ,
          const int& type = 0
        ) const;

        void givenPointsMap1D
        (
          double* points            ,
          const double* givenPoints ,
          const int& size           ,
          const int& dim = 2
        ) const;
};
}
#include "baseToyMesh.C"

#endif
