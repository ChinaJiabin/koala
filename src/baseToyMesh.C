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
}
