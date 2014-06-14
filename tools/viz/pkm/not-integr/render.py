import os
import sys
from paraview.simple import *

def MakeCoolToWarmLT(name, label, min, max):
    r=0.137255
    g=0.239216
    b=0.709804
    r2=0.67451
    g2=0.141176
    b2=0.12549
    lt = GetLookupTableForArray(name, 1, RGBPoints = [min, r, g, b, max, r2, g2, b2], ColorSpace = "Diverging")
    lt.RGBPoints.SetData( [min, r, g, b, max, r2, g2, b2] )
    return lt

def SetUpView():
    view = GetActiveView()
    if not view:
        view = CreateRenderView()

    view.ViewSize = [1000, 500]
    view.CameraViewUp = [0, 1, 0]
    view.CameraFocalPoint = [140, 73.25, 150]
    view.CameraViewAngle = 30
    view.CameraPosition = [140, 73.25, 500]
    view.Background = [0.3, 0.3, 0.4]
    view.CenterAxesVisibility = 0

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

def CreateSnap(step):
    readers = [XMLUnstructuredGridReader(FileName = [os.path.join(os.getcwd(), 'cpu_%(cpu)d_step_%(step)d.vtu' % {"cpu":z, "step":step})]) for z in range(8)]
    gds = GroupDatasets(*readers)
    s = Slice(gds)
    s.SliceType.Origin = [140, 73.25, 150]
    s.SliceType.Normal = [0, 0, 1]

    CreateImage(s, "compression", "compression", 2.13e8, step)
    CreateImage(s, "deviator", "deviator", 1.12e8, step)
    CreateImage(s, "shear", "shear", 1.12e8, step)
    CreateImage(s, "tension", "tension", 8.6e7, step)
    CreateImage(s, "tension", "delamination", 4.2e7, step)

def CreateStat(step):
    readers = [XMLUnstructuredGridReader(FileName = [os.path.join(os.getcwd(), 'cpu_%(cpu)d_step_%(step)d.vtu' % {"cpu":z, "step": (x+1)}) for x in range(step)]) for z in range(8)]
    gds = GroupDatasets(*readers)
    ts = TemporalStatistics(gds)
    s = Slice(ts)
    s.SliceType.Origin = [140, 73.25, 150]
    s.SliceType.Normal = [0, 0, 1]

    CreateImage(s, "compression_maximum", "compression_maximum", 2.13e8, step)
    CreateImage(s, "deviator_maximum", "deviator_maximum", 1.12e8, step)
    CreateImage(s, "shear_maximum", "shear_maximum", 1.12e8, step)
    CreateImage(s, "tension_maximum", "tension_maximum", 8.6e7, step)
    CreateImage(s, "tension_maximum", "delamination_maximum", 4.2e7, step)

SetUpView()

action = sys.argv[1]
step = int(sys.argv[2])

if action == "snap":
    CreateSnap(step)
elif action == "stat":
    CreateStat(step)
