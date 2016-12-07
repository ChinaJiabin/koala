namespace Koala
{

baseToyMesh::baseToyMesh
(
  const parRun& Run_         ,
  const char* inputFileName_ ,
  const char* filesPath_
)
:
  Run(Run_)                     ,
  inputFileName(inputFileName_) ,
  filesPath(filesPath_) {}

void baseToyMesh::writeBoundaryName() const
{
  std::ofstream file;
  Run.openFile(file, "boundary", NULl, filesPath);

  file << sizePatches << "\n";
  file << patchesName;
}
  
void baseToyMesh::equalRatioDistribution1D
(
  double* map         ,
  const int& size     ,
  const double& ratio ,  
  const int& type
) const
{
  if (ratio > 0 && abs(ratio - 1.0) > 1e-5)
  {
    // Equal ratio distribution
    if (type == 0)
    {
      double q = pow(ratio, 1.0/size);
      
      map[0] = (q - 1.0)/(pow(q, size + 1) - 1);
      map[1] = map[0]*(1 + q);
      
      for (int i = 2; i < size; i++)
        map[i] = map[i - 1] + (map[i - 1] - map[i - 2])*q;
    }
    // Central equal ratio distribution
    else
    {
      if (size == 1)
        map[0] = 0.5;
      else
      {
        int sizeHalf = size/2;
        double q = pow(ratio, 1.0/sizeHalf);
        
        map[0] = 1.0/(
      }
    }
  }
  else
    // Uniform distribution
    for (int i = 0; i < size; i++)
      map[i] = (i + 1)/(size + 1.0);
}

void baseToyMesh::lineMap1D
(
  const double* startPoint ,
  const double* endPoint   ,
  double* points           ,
  const int& size          ,    
  const int& dim           ,
  const double& ratio      ,
  const int& type
) const
{
}

void baseToyMesh::arcMap1D
(
  const double* startPoint ,
  const double* endPoint   ,
  double* points           ,
  const int& size          ,    
  const double& angle      ,
  const double& ratio      ,
  const int& type
) const
{
}

void baseToyMesh::givenPointsMap1D
(
  double* points            ,
  const double* givenPoints ,
  const int& size
) const
{
}
}
