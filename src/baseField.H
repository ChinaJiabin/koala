#ifndef baseField_H
#define baseField_H

namespace Koala
{

class baseField
{
protected:
        const char* name;
        int size;
        double* value;
        
public:
    // Constructor
        
        baseField() {};
        
        baseField(const int& size_);
        
        baseField(const char* name_, const int& size_);
        
        baseField(const int& size_, double* value_);
        
        baseField(const char* name_, const int& size_, double* value_);
    
    // Member operators
    
        void operator=(const baseField& varField);
        void operator=(const double& var);
        
        void operator+=(const baseField& varField);
        void operator+=(const double& var);
        
        void operator-=(const baseField& varField);
        void operator-=(const double& var);
        
        void operator*=(const baseField& varField);
        void operator*=(const double& var);
        void operator*=(const double* var);
        
        void operator/=(const baseField& varField);
        void operator/=(const double& var);
        void operator/=(const double* var);
        
    // inline
    
        inline const char* Name()    const{return name;}
        inline const int& Size()     const{return size;}
        inline const double* Value() const{return value;}
        
        inline double& operator[](const int& id) const{return value[id];}
};
}
#include "baseField.C"

#endif
