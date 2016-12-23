#!/usr/bin/python
from pyMesh import pyMesh2D
import numpy as np

mesh = pyMesh2D()

mesh.points     = [ [0, 0], [1, 0], [1, 1], [0, 1] ]
mesh.blocks     = [ [0, 1, 2, 3] ]
mesh.parameters = [
                    [4, 4, 1, 0, 0, 0, [np.pi/6, 0, 0], [0, 0], [0, 0], [0, 0]]
                  ]
mesh.patches    = [
                    ["bottom", [0, 0]] ,
                    ["top"   , [0, 2]] ,
                    ["left"  , [0, 3]] ,
                    ["right" , [0, 1]]
                  ]
mesh.write()
