#!pvbatch

import os
from paraview.simple import *

def SetUpView():
	view = GetActiveView()
	if not view:
		view = CreateRenderView()

	# the size of the picture in pixels
	view.ViewSize = [2000, 750]
	# where is the camera placed itself
	view.CameraPosition = [-10, 0, 1.25]
	# the point the camera is looking at from its position
	view.CameraFocalPoint = [0, 0, 1.25]
	# the direction in space that is up at the picture
	view.CameraViewUp = [0, 0, 1]
	# zoom - angle of view
	view.CameraViewAngle = 45
	# coloring of the background, [1, 1, 1] is white, 
	# [0.3, 0.3, 0.4] is like usually in Paraview
	view.Background = [0.3, 0.3, 0.4]
	# replace center axes from picture
	view.CenterAxesVisibility = False

def MakeCoolToWarmLT(name, min, max):
	r=0.137255
	g=0.239216
	b=0.709804
	r2=0.67451
	g2=0.141176
	b2=0.12549
	lt = GetLookupTableForArray(name, 1, RGBPoints = [min, r, g, b, max, r2, g2, b2], ColorSpace = "Diverging")
	lt.VectorMode = "Magnitude"
	lt.RGBPoints.SetData([min, r, g, b, max, r2, g2, b2])
	return lt

def makeSnapshot():
	view = CreateRenderView()

	readers = [XMLStructuredGridReader(FileName = [os.path.join(os.getcwd(), 'snap_mesh_layer%d_cpu_0_step_20.vts' % z)]) for z in [1, 2, 3]]

	gds = GroupDatasets(*readers)

	s = Slice(gds)
	s.SliceType.Normal = [1, 0, 0]
	dp = GetDisplayProperties(s)
	dp.Representation = 'Surface'
	dp.ColorArrayName = 'szz'
	dp.LookupTable = MakeCoolToWarmLT('s', -1, 1)
	bar = CreateScalarBar(LookupTable=dp.LookupTable, Title='szz', TitleFontSize = 10, LabelFontSize = 10)
	view.Representations.append(bar)

	#g = Glyph(gds)
	#dp = GetDisplayProperties(g)
	#dp.Representation = 'Surface'
	#dp.ColorArrayName = 'velocity'

	Show(s)
	Render()
	SetUpView()

	WriteImage("sliced.png")


makeSnapshot()
