#!/usr/bin/python

import os
import sys
from paraview.simple import *

def SetUpView():
    view = GetActiveView()
    if not view:
        view = CreateRenderView()

    view.ViewSize = [1000, 1000]
    view.CameraViewUp = [-1, 0, 0]
    view.CameraFocalPoint = [-0.5, 0, 0]
    view.CameraViewAngle = 30
    view.CameraPosition = [-0.5, -200, 0]
    view.Background = [0.3, 0.3, 0.4]
    view.CenterAxesVisibility = 0

def MakeCoolToWarmLT(name, label, min, max):
    r=0.137255
    g=0.239216
    b=0.709804
    r2=0.67451
    g2=0.141176
    b2=0.12549
    lt = GetLookupTableForArray(name, 1, RGBPoints = [min, r, g, b, max, r2, g2, b2], ColorSpace = "Diverging")
    lt.VectorMode = "Magnitude"
    lt.RGBPoints.SetData( [min, r, g, b, max, r2, g2, b2] )
    return lt

def CreateImage(obj, field, label, limit, step):
    dp = GetDisplayProperties(obj)
    dp.LookupTable = MakeCoolToWarmLT(field, label, 0, limit)
    dp.ColorAttributeType = 'POINT_DATA'
    dp.ColorArrayName = field
    bar = CreateScalarBar(LookupTable=dp.LookupTable, Title=label, TitleFontSize = 10, LabelFontSize = 10)
    GetActiveView().Representations.append(bar)
    Show(obj)
    Render()
    SetUpView()
    WriteImage('res-%(field)s-%(step)d.png' % {"field":label, "step":step})
    GetActiveView().Representations.remove(bar)

def CreateMatView():
    readers = [XMLUnstructuredGridReader(FileName = [os.path.join(os.getcwd(), 'snap_mesh_main_cpu_0_step_0.vtu')])]
    gds = GroupDatasets(*readers)
    s = Slice(gds)
    s.SliceType.Origin = [-0.5, 0, 0]
    s.SliceType.Normal = [0, 1, 0]

    CreateImage(s, "materialID", "materialID", 13, step)
    

def CreateSnap(step):
    readers = [XMLUnstructuredGridReader(FileName = [os.path.join(os.getcwd(), 'snap_mesh_main_cpu_0_step_%(step)d.vtu' % {"step":step})])]
    gds = GroupDatasets(*readers)
    s = Slice(gds)
    s.SliceType.Origin = [-0.5, 0, 0]
    s.SliceType.Normal = [0, 1, 0]

    CreateImage(s, "velocity", "velocity", 30, step)

step = int(sys.argv[1])

SetUpView()
CreateMatView()

for i in range(step+1):
	CreateSnap(i)	

