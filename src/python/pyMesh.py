#!/usr/bin/python
import os

'''
 Polynomial equation: x^n -k*n + (k - 1) = 0
 Find real root x0 using Newton iteration method
 Beautiful algorithm
'''
def solveRatioEquation(n, k):
  if k == n or n == 1:
    return 1

  x = (k/(n + 0.0))**(1/(n - 1.0))
  if k > n:
    x += 1
  else:
    x -= 1

  while 1:
    xOld = x
    x    = ((n - 1)*x**n + (1 - k))/(n*x**(n - 1) - k)

    if abs(xOld - x) < 1e-5:
      return x

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
        for i in range(5):
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
      file.write(str(len(self.patches)) + " ")
      for x in self.patches:
        file.write(str(len(x[1])/2) + " ")
      file.write("\n")

      for x in self.patches:
        file.write(x[0] + "\n")
        for value in x[1]:
          file.write(str(value) + " ")
        file.write("\n")
