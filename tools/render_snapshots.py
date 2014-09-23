import os
from paraview.simple import *

def SetUpView():
    view = GetActiveView()
    if not view:
        view = CreateRenderView()

    view.ViewSize = [1000, 1000]
    view.CameraViewUp = [0, 0, 1]
    view.CameraFocalPoint = [0, 0, 0]
    view.CameraViewAngle = 30
    view.CameraPosition = [100, 100, 100]
    view.Background = [0.3, 0.3, 0.4]

SetUpView()

readers = [XMLUnstructuredGridReader(FileName = [os.path.join(os.getcwd(), 'snap_volume_zone_%d_snap_0.vtu' % z)]) for z in range(4)]

gds = GroupDatasets(*readers)

s = Slice(gds)
s.SliceType.Normal = [0, 1, 0]

dp = GetDisplayProperties(s)
dp.ColorArrayName = 'szz'


Show(s)
Render()

WriteImage("sliced.png")

Hide(s)

SetUpView()

c = Clip(gds)
c.ClipType.Normal = [-1, -1, -1]

dp = GetDisplayProperties(c)
dp.ColorArrayName = 'velocity'

Show(c)
Render()

WriteImage("clipped.png")
