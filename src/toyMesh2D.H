#ifndef toyMesh2D_H
#define toyMesh2D_H

#include "baseToyMesh.H"
namespace Koala
{

class parBlock2D
:
  public parBlock
{
public:
    // Constructor

        parBlock2D();

    // inline
 
        inline int SizePoints()     const{ return (n[0] + 1)*(n[1] + 1) - 4 ; }
        inline int SizeCells()      const{ return n[0]*n[1]; }
        inline int SizeInnerFaces() const{ return n[0]*(n[1] - 1) + n[1]*(n[0] - 1); }
};

typedef int (*listLines2D)[5];

class toyMesh2D
: 
  public baseToyMesh
{
private:
        // Blocks parameters
        parBlock2D* parBlocks;
        listPoints2D pointsOfBlocks;
        listLines2D lines;

        // Smooth method: Gauss-Seidel method
        double smoothEllipticEquation
        (
          const int& nX       ,
          const int& nY       ,
          double* coordinates ,
          double* residual    ,
          const int& numGap = 1
        ) const;
        
        // Restriction operator
        double restriction
        (
          const int& nX    ,
          const int& nY    ,
          double* residual ,
          const int& numGap = 2
        ) const;

        // Prolongation operator
        double prolongation
        (
          const int& nX    ,
          const int& nY    ,
          double* residual ,
          const int& numGap = 2
        ) const;

public:
    // Constructor

        toyMesh2D
        (
          const parRun& Run ,
          const char* inputFileName_ = "toyMesh"
        );

    // Write

        virtual void writePoints() const; 
        virtual void writeCells()  const; 
        virtual void writeFaces()  const;  

        virtual void writeBoundaryPointsId() const;
        virtual void writeBoundaryFacesId()  const;
};
}

#endif
