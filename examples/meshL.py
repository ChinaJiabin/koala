#!/usr/bin/python
from pyMesh import pyMesh2D

mesh = pyMesh2D()

mesh.points     = [ [0, 0], [-1, 0], [-1, 1], [1, 1], [1, -1], [0, -1] ]
mesh.blocks     = [ [0, 3, 2, 1], [0, 5, 4, 3] ]
mesh.parameters = [
                    [4, 4, 0, 0, 0, 0, [0, 0], [0, 0], [0, 0], [0, 0]] ,
                    [4, 4, 0, 0, 0, 0, [0, 0], [0, 0], [0, 0], [0, 0]]
                  ]
mesh.patches    = [
                    ["wall", [0, 0, 0, 2, 0, 3, 1, 0, 1, 1, 1, 3]]
                  ]
mesh.write()
