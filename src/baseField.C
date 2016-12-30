#include <cstring>
namespace Koala
{

// Constructor
baseField::baseField(const int& size_)
:
  size(size_)
 {
   value = new double[size];
   memset(value, 0, size*sizeof(double));
 }
 
baseField::baseField(const char* name_, const int& size_)
:
  name(name_) ,
  size(size_)
 {
   value = new double[size];
   memset(value, 0, size*sizeof(double));
 }
 
baseField::baseField(const int& size_, double* value_)
:
  size(size_) ,
  value(value_) {}
  
baseField::baseField(const char* name_, const int& size_, double* value_)
:
  name(name_) ,
  size(size_) ,
  value(value_) {}
  
// Member operators
void baseField::operator=(const baseField& varField)
{
  for (int i = 0; i < size; i++)
    value[i] = varField.value[i];
}

void baseField::operator=(const double& var)
{
  for (int i = 0; i < size; i++)
    value[i] = var;
}

void baseField::operator+=(const baseField& varField)
{
  for (int i = 0; i < size; i++)
    value[i] += varField.value[i];
}

void baseField::operator+=(const double& var)
{
  for (int i = 0; i < size; i++)
    value[i] += var;
}

void baseField::operator-=(const baseField& varField)
{
  for (int i = 0; i < size; i++)
    value[i] -= varField.value[i];
}

void baseField::operator-=(const double& var)
{
  for (int i = 0; i < size; i++)
    value[i] -= var;
}

void baseField::operator*=(const baseField& varField)
{
  for (int i = 0; i < size; i++)
    value[i] *= varField.value[i];
}

void baseField::operator*=(const double& var)
{
  for (int i = 0; i < size; i++)
    value[i] *= var;
}

void baseField::operator*=(const double* var)
{
  for (int i = 0; i < size; i++)
    value[i] *= var[i];
}

void baseField::operator/=(const baseField& varField)
{
  for (int i = 0; i < size; i++)
    value[i] /= varField.value[i];
}

void baseField::operator/=(const double& var)
{
  for (int i = 0; i < size; i++)
    value[i] /= var;
}

void baseField::operator/=(const double* var)
{
  for (int i = 0; i < size; i++)
    value[i] /= var[i];
}
}
