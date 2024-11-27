#!/usr/bin/python3

import argparse
import os
import re
import vtk

parser = argparse.ArgumentParser(description='Converts sensor data from CSV to VTS')

parser.add_argument(
    '-fname',
    type=str,
    metavar='FNAME',
    help='A name of csv series.'
)

parser.add_argument(
    '-nx',
    default=9,
    type=int,
    metavar='NX',
    help='Amount of sensors along X axis.'
)

parser.add_argument(
    '-ny',
    default=9,
    type=int,
    metavar='NY',
    help='Amount of sensors along Y axis.'
)

parser.add_argument(
    '-dx',
    default=1.0,
    type=float,
    metavar='DX',
    help='Distance between sensors along X axis.'
)

parser.add_argument(
    '-dy',
    default=1.0,
    type=float,
    metavar='DY',
    help='Distance between sensors along Y axis.'
)

parser.add_argument(
    '-dz',
    default=0.1,
    type=float,
    metavar='DX',
    help='Now, that is a tricky one. It is a Z-axis step, which depends on p-wave speed and time step.'
)

parser.add_argument(
    '-offset',
    default=10,
    type=int,
    metavar='OFFSET',
    help='Amount of time steps between emitters start times.'
)

parser.add_argument(
    '-thickness',
    default=10,
    type=int,
    metavar='THICKNESS',
    help='Amount of time steps to draw.'
)
args = parser.parse_args()

values = []
offset = 0
for i in range(0, args.nx):
    sheet = []
    sname = args.fname + "_" + str(i)
    for j in range(0, args.ny):
        lname = sname + str(j) + ".csv"
        with open(lname) as f:
            sheet.append([[float(x) for x in l.split()] for l in f.readlines()][offset : offset + args.thickness])
            offset += args.offset
    values.append(sheet)


structuredGrid = vtk.vtkStructuredGrid()
points = vtk.vtkPoints()

vx = vtk.vtkDoubleArray()
vx.SetName("vx")
vy = vtk.vtkDoubleArray()
vy.SetName("vy")
vz = vtk.vtkDoubleArray()
vz.SetName("vz")

for i in range(0, args.nx):
    for j in range(0, args.ny):
        for k in range(0, args.thickness):
            points.InsertNextPoint(float(i) * args.dx, float(j) * args.dy, float(k) * args.dz)
            vx.InsertNextValue(values[i][j][k][1])
            vy.InsertNextValue(values[i][j][k][2])
            vz.InsertNextValue(values[i][j][k][3])

structuredGrid.SetDimensions(args.thickness, args.ny, args.nx)
structuredGrid.SetPoints(points)
structuredGrid.GetPointData().AddArray(vx)
structuredGrid.GetPointData().AddArray(vy)
structuredGrid.GetPointData().AddArray(vz)
writer = vtk.vtkXMLStructuredGridWriter()
writer.SetInputDataObject(structuredGrid)
writer.SetFileName(args.fname + ".vts")
writer.Write()


0.00538482