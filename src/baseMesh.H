#ifndef baseMesh_H
#define baseMesh_H

#include "parRun.H"
namespace Koala
{

class baseMesh
{
public:
    // static
        
        static inline int CellVtkType(const int& size, const int& dim);
};
}
#include "baseMesh.C"

#endif
