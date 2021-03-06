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

        inline int SizePoints()      const{ return (n[0] + 1)*(n[1] + 1); }

        inline int SizeCells()       const{ return n[0]*n[1]; }

        inline int SizeInnerPoints() const{ return (n[0] - 1)*(n[1] - 1); }

        inline int SizeInnerFaces()  const{ return n[0]*(n[1] - 1) + n[1]*(n[0] - 1); }
};

typedef double (*listPoints2D)[2];
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

        virtual void getPointsIdOfBlock(const int& blockId , int* pointsIdOfBlock) const;

        /*
          Gauss-Seidel smooth method
          Format from elliptic equation
        */
        void smoothPoint
        (
          double* point                  ,                 
          const double* point_right      ,
          const double* point_up         ,
          const double* point_left       ,
          const double* point_down       ,
          const double* point_right_up   ,
          const double* point_up_left    ,
          const double* point_left_down  ,
          const double* point_down_right
        ) const;

        // Smooth all points in the block
        double smoothBlockPoints
        (
          const int& blockId   ,
          listPoints2D points  ,
          int* pointsIdOfBlock ,
          double* residual     ,
          const int& numGap = 1
        ) const;

        // Restriction operator
        double restriction
        (
          const int& blockId   ,
          double* residual     ,
          const int& numGap = 2
        ) const;

        // Prolongation operator
        double prolongation
        (
          const int& blockId   ,
          double* residual     ,
          const int& numGap = 2
        ) const;

public:
    // Constructor

        toyMesh2D
        (
          const parRun& Run ,
          const char* inputFileName_ = "toyMesh2D"
        );

    // Write

        virtual void writePoints() const;
        virtual void writeCells()  const;
        virtual void writeFaces()  const;

        virtual void writeBoundaryPointsId() const;
        virtual void writeBoundaryFaces()  const;
};
}
#include "toyMesh2D.C"

#endif
