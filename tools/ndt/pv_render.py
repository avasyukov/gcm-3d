#!/bin/env python2

import sys
import libxml2
import json
import os
import argparse
import logging

from paraview.simple import GetParaViewVersion

USE_API_4_2 = [int(x) for x in str(GetParaViewVersion()).split('.')] >= [4, 2]

def render(step, output_pattern, out_dir):
    from paraview.simple import WriteImage

    output_file = os.path.join(out_dir, output_pattern % {'step': step})
    logger.debug('Saving plot to file: ' + output_file)
    WriteImage(output_file)

def plotOverLine(gds, desc, step, output_dir):
    import math
    from paraview.simple import CreateXYPlotView, SetActiveView
    from paraview.simple import SetActiveView, GetActiveSource, servermanager
    from paraview.simple import PlotOverLine, Calculator, GetDisplayProperties

    logger = logging.getLogger('gcm.pv_render.plotOverLine')

    title = desc.xpathEval('title/text()')
    if len(title) == 1:
        title = title[0].getContent()
    else:
        title = None
    output = desc.prop('output')

    logger.debug('Processing plotOverLine section')
    logger.debug('Title: ' + str(title))
    logger.debug('Output file name pattern: ' + output)

    quantities = [x.getContent() for x in desc.xpathEval('quantities/quantity/text()')]

    logger.debug('List of quantities to draw: ' + str(quantities))

    line_from = [float(x) for x in desc.prop('from').split(';')]
    line_to = [float(x) for x in desc.prop('to').split(';')]

    logger.debug('Line: %s -> %s' % (str(line_from), str(line_to)))

    view = CreateXYPlotView()
    view.ViewSize = [1000, 500]
    view.ChartTitle = title
    SetActiveView(view)

    leftAxis = desc.xpathEval('axes/left')
    if len(leftAxis) == 1:
        leftAxis = [float(leftAxis[0].prop('from')), float(leftAxis[0].prop('to'))]
        logger.debug('Using custom Y axis range: ' + str(leftAxis))
    else:
        logger.debug('Using auto Y axis range')
        leftAxis = None

    if not leftAxis is None:
        if USE_API_4_2:
            view.LeftAxisRangeMinimum = leftAxis[0]
            view.LeftAxisRangeMaximum = leftAxis[1]
            view.LeftAxisUseCustomRange = True
        else:
            view.LeftAxisRange = leftAxis
            view.AxisUseCustomRange = [1, 0, 0, 0]

    bottomAxis = desc.xpathEval('axes/bottom')
    if len(bottomAxis) == 1:
        bottomAxis = [float(bottomAxis[0].prop('from')), float(bottomAxis[0].prop('to'))]
        logger.debug('Using custom labels for X axis: ' + str(bottomAxis))
    else:
        logger.debug('Using default X axis labels')
        bottomAxis = None

    if bottomAxis:
        logger.debug('Creating custom calculator to draw proper X axis labels')
        calc = Calculator(gds)
        calc.ResultArrayName = 'xindex'
        calc.Function = '%(index_from)f+%(len)f*sqrt((coordsX-(%(from_x)f))^2 + (coordsY-(%(from_y)f))^2 + (coordsZ-(%(from_z)f))^2)/%(line_len)f' % {
            'index_from': bottomAxis[0],
            'len': bottomAxis[1]-bottomAxis[0],
            'from_x': line_from[0],
            'from_y': line_from[1],
            'from_z': line_from[2],
            'line_len': math.sqrt((line_from[0]-line_to[0])**2+(line_from[1]-line_to[1])**2+(line_from[2]-line_to[2])**2)
        }

        src = calc
    else:
        src = gds

    logger.debug('Creating POL object')
    pol = PlotOverLine(src)
    pol.Source.Point1 = line_from
    pol.Source.Point2 = line_to


    active_src = GetActiveSource()
    filter = servermanager.Fetch(active_src)
    pd = filter.GetPointData()

    if USE_API_4_2:
        _sv = quantities
    else:
        logger.debug('Filing default series visibility list (hide all)')

        sv = {
            'Points (0)': '0',
            'Points (1)': '0',
            'Points (2)': '0',
            'Points (Magnitude)': '0',
            'vtkOriginalIndices': '0',
        }

        for i in range(pd.GetNumberOfArrays()):
            arr = pd.GetArray(i)
            sz = arr.GetNumberOfComponents()
            name = arr.GetName()
            if sz == 1:
                sv[name] = '0'
            else:
                for j in range(sz):
                    sv[name + ' (' + str(j) + ')'] = '0'
                sv[name + ' (Magnitude)'] = '0'

        for q in quantities:
            logger.debug('Showing plot for ' + q)
            sv[q] = '1'

        _sv = []
        for v in sv:
            _sv.append(v)
        _sv.append(sv[v])

    logger.debug('Setting display properties for POL object')
    dp = GetDisplayProperties(pol)
    dp.SeriesVisibility = _sv

    if not bottomAxis is None:
        dp.UseIndexForXAxis = 0
        dp.XArrayName = 'xindex'


    render(step, output, output_dir)

def render3d(gds, desc, step, output_dir):
    from paraview.simple import CreateRenderView, SetActiveView, GetActiveView
    from paraview.simple import CreateScalarBar, GetLookupTableForArray
    from paraview.simple import Threshold, Clip, GetDisplayProperties

    def MakeCoolToWarmLT(name, min, max):
        #r=0.137255
        #g=0.239216
        #b=0.709804
        #r2=0.67451
        #g2=0.141176
        #b2=0.12549
        r=1
        g=1
        b=1
        r2=0
        g2=0
        b2=0
        lt = GetLookupTableForArray(name, 1, RGBPoints = [min, r, g, b, max, r2, g2, b2], ColorSpace = "RGB")
        lt.VectorMode = "Magnitude"
        lt.RGBPoints.SetData([min, r, g, b, max, r2, g2, b2])
        return lt

    logger = logging.getLogger('gcm.pv_render.render3d')

    title = desc.xpathEval('title/text()')
    if len(title) == 1:
        title = title[0].getContent()
    else:
        title = None
    output = desc.prop('output')

    logger.debug('Processing plotOverLine section')
    logger.debug('Title: ' + str(title))
    logger.debug('Output file name pattern: ' + output)

    q = desc.xpathEval('quantity')[0]
    quantity = q.getContent()
    qmin = float(q.prop('min'))
    qmax = float(q.prop('max'))

    camera = desc.xpathEval('camera')
    if len(camera) == 1:
        camera = camera[0]
    else:
        logger.fatal('Camera settings not provided')

    cpos = [float(x) for x in camera.prop('position').split(';')]
    cup = [float(x) for x in camera.prop('up').split(';')]
    cfocal = [float(x) for x in camera.prop('focal').split(';')]
    cangle = float(camera.prop('angle'))

    logger.debug('Camera position: ' + str(cpos))
    logger.debug('Camera up: ' + str(cup))
    logger.debug('Camera focal point: ' + str(cfocal))
    logger.debug('Camera view angle: ' + str(cangle))

    view = CreateRenderView()

    view.ViewSize = [1000, 500]
    view.Background = [1, 1, 1]
    view.CenterAxesVisibility = 0

    view.CameraViewUp = cup
    view.CameraFocalPoint = cfocal
    view.CameraViewAngle = cangle
    view.CameraPosition = cpos

    SetActiveView(view)

    logger.debug('Quantity to render: ' + quantity)

    src = gds

    logger.debug('Processing thresholds')

    for thr in desc.xpathEval('thresholds/threshold'):
        scalar = thr.prop('scalar')
        tmin = float(thr.prop('min'))
        tmax = float(thr.prop('max'))

        logger.debug('Creating threshold on array %s with range [%f, %f]' % (scalar, tmin, tmax))

        src = Threshold(src)
        src.Scalars = ['POINTS', scalar]
        src.ThresholdRange = [tmin, tmax]

    logger.debug('Processing clips')

    for cl in desc.xpathEval('clips/clip'):
        origin = [float(x) for x in cl.prop('origin').split(';')]
        normal = [float(x) for x in cl.prop('normal').split(';')]

        logger.debug('Creating clip with origin %s and normal %s' % (origin, normal))

        src = Clip(src)
        src.ClipType.Origin = origin
        src.ClipType.Normal = normal

    dp = GetDisplayProperties(src)
    dp.LookupTable = MakeCoolToWarmLT(quantity, qmin, qmax)

    if USE_API_4_2:
        dp.ColorArrayName = ('POINTS', quantity)
    else:
        dp.ColorArrayName = quantity
        dp.ColorAttributeType = 'POINT_DATA'

    bar = CreateScalarBar(LookupTable=dp.LookupTable, Title=quantity.capitalize(), ComponentTitle='', TitleFontSize = 10, LabelFontSize = 10, TitleColor = [0.0, 0.0, 0.0], LabelColor = [0.0, 0.0, 0.0])
    GetActiveView().Representations.append(bar)



    render(step, output, output_dir)


def render_all(args, task, snapshotsList):
    import subprocess

    logger = logging.getLogger('gcm.pv_render.render_all')
    for (idx, rc) in enumerate(task.xpathEval('/task/render/renderConfig')):
        logger.info('Processing render config (%s)' % rc.prop('id'))
        for snaps in snapshotsList:
            cmd = [
                '/bin/env',
                'pvbatch', '--use-offscreen-rendering',
                sys.argv[0],
                '--task', args.task,
                '--output-dir', args.output_dir,
                '--snap-list', args.snap_list
                ]
            if args.verbose:
                cmd.append('--verbose')
            cmd.extend([
                'render-one',
                '--render-config', str(idx+1),
                '--snap-index', snaps['index']
            ])

            logger.debug('Running command: ' + str(cmd))
            subprocess.call(cmd)

def render_one(args, task, snapshotsList):
        from paraview.simple import XMLUnstructuredGridReader, XMLStructuredGridReader
        from paraview.simple import GroupDatasets
	print('/task/render/renderConfig[%s]' % args.render_config)

        rc = task.xpathEval('/task/render/renderConfig[%s]' % args.render_config)[0]
        meshes = [x.getContent() for x in rc.xpathEval('meshes/mesh/text()')]
        logger.debug('List of meshes to load: ' + str(meshes))

        snaps = snapshotsList[int(args.snap_index)]
        logger.info('Processing snapshots group #%s (time: %s)' % (snaps['index'], snaps['time']))
        readers = []
        for snap in snaps['snaps']:
            if snap['mesh'] in meshes:
                snapFileName = snap['file']
                readerClass = XMLStructuredGridReader if snapFileName.endswith('.vts') else XMLUnstructuredGridReader
                logger.debug('Reading file: ' + snapFileName)
                readers.append(readerClass(FileName=[snapFileName]))

        logger.debug('Creating group data set')
        gds = GroupDatasets(*readers)

        logger.debug('Processing plotOverLine sections')
        for pol in rc.xpathEval('plotOverLine'):
            plotOverLine(gds, pol, int(snaps['index']), args.output_dir)

        logger.debug('Processing render3d sections')
        for r3d in rc.xpathEval('render3d'):
            render3d(gds, r3d, int(snaps['index']), args.output_dir)

parser = argparse.ArgumentParser(description='Render calculation results using ParaView')
parser.add_argument('--task', required=True, help='Task to read render config from')
parser.add_argument('--snap-list', required=True, help='List of snapshots to render')
parser.add_argument('--output-dir', required=True, help='Output directory')
parser.add_argument('--verbose', action="store_true", default=False, help='Be verbose')

subparsers = parser.add_subparsers()
parser_render_all = subparsers.add_parser('render-all', help='Render all snapshots')

parser_render_one = subparsers.add_parser('render-one', help='Render specified snapshot')
parser_render_one.add_argument('--render-config', required=True, help='Render config index')
parser_render_one.add_argument('--snap-index', required=True, help='Snapshot index to render')

parser_render_all.set_defaults(action=render_all)
parser_render_one.set_defaults(action=render_one)

args = parser.parse_args()

logging.basicConfig(level=(logging.DEBUG if args.verbose else logging.INFO))
logger = logging.getLogger('gcm.pv_render')

logger.debug('Task file: ' + args.task)
logger.debug('Snapshots list file: ' + args.snap_list)
logger.debug('Output directory: ' + args.output_dir)

if not os.path.isfile(args.task):
    logger.fatal('Task file does not exist')
    sys.exit(-1)

if not os.path.isfile(args.snap_list):
    logger.fatal('Snapshots list file does not exist')
    sys.exit(-1)

if not os.path.isdir(args.output_dir):
    logger.debug('Output directory does not exist, trying to create')
    os.makedirs(args.output_dir)

logger.info('Parsing task file')
task = libxml2.parseFile(args.task)
logger.info('Parsing snapshots list file')
snapshotsList = json.load(open(args.snap_list))['snapshots']

if not hasattr(args, 'action'):
    parser.print_usage()
    sys.exit(-1)
else:
    args.action(args, task, snapshotsList)
