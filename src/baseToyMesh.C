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
