#ifndef toyMesh3D_H
#define toyMesh3D_H

#include "baseToyMesh.H"
namespace Koala
{

class parBlock3D
:
  public parBlock
{
public:
    // Constructor

        parBlock3D();

    // inline
 
        inline int SizePoints()      const{ return (n[0] + 1)*(n[1] + 1)*(n[2] + 1); }

        inline int SizeCells()       const{ return n[0]*n[1]*n[2]; }
        
        inline int SizeInnerPoints() const{ return (n[0] - 1)*(n[1] - 1)*(n[2] - 1); }

        inline int SizeInnerFaces()  const{ return n[0]*n[1]*(n[2] - 1) + n[0]*(n[1] - 1)*n[2] + (n[0] - 1)*n[1]*n[2]; }
};

typedef double (*listPoints3D)[3];
typedef int (*listLines3D)[3]
typedef int (*listFaces3D)[6];

class toyMesh3D
: 
  public baseToyMesh
{
private:
        // Blocks parameters
        parBlock3D* parBlocks;
        listPoints3D pointsOfBlocks;
        
        listLines3D lines;

        int sizeFaces;
        listFaces3D faces;
        int* pointsOnFacesIndex;
         
public:
    // Constructor

        toyMesh3D
        (
          const parRun& Run_                       ,
          const char* inputFileName_ = "toyMesh3D" ,
          const char* filePath_      = "mesh"
        );

    // Write

        virtual void writePoints() const; 
        virtual void writeCells()  const; 
        virtual void writeFaces()  const;  

        virtual void writeBoundaryPointsId() const;
        virtual void writeBoundaryFaces()  const;
};
}
#include "toyMesh3D.C"

#endif
