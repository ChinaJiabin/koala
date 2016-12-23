#!/usr/bin/python
import os

class pyMesh2D:
  def __init__(self, fileName = "mesh/toyMesh2D"):
    self.points     = []
    self.blocks     = []
    self.parameters = []
    self.patches    = []
    
    self.path         = os.getcwd()
    self.fileName     = fileName
    self.fileFullPath = os.path.join(self.path, self.fileName)
    
  def write(self):
    with open(self.fileFullPath, "w") as file:
      # Points
      file.write(str(len(self.points)) + "\n")
      for x in self.points:
        file.write(str(x[0]) + " " + str(x[1]) + "\n")
        
      # Blocks
      file.write(str(len(self.blocks)) + "\n")
      for x in self.blocks:
        for i in range(3):
          file.write(str(x[i]) + " ")
        file.write(str(x[3]) + "\n") 
      
      for x in self.parameters:
        for x in range(5):
          file.write(str(x[i]) + " ")
        file.write(str(x[5]) + "\n")
        
      # Parameters of lines
      for x in self.parameters:
        for i in range(6, 10):
          file.write(str(len(x[i])) + " ")
      file.write("\n")
      
      for x in self.parameters:
        for i in range(6, 10):
          for value in x[i]:
            file.write(str(value) + " ")
      file.write("\n")
      
      # Boundary
      file.write(str(len(self.patches)) + "\n")
      for x in self.patches:
        file.write(str(len(x[i])/2) + " ")
      file.write("\n")
      
      for x in self.patches:
        file.write(x[0] + "\n")
        for value in x[1]:
          file.write(str(value) + " ")
        file.write("\n")
        
