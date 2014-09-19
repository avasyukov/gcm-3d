#!/bin/env pvbatch

import sys
import libxml2
import json
import os
import math
import argparse
import logging

from paraview.simple import *

def plotOverLine(desc, step, output_dir):
    
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

    
    logger.debug('Rendering')
    UpdatePipeline()
    Render()
    
    output_file = os.path.join(output_dir, output % {'step': step})
    logger.debug('Saving plot to file: ' + output_file)
    WriteImage(output_file, view)

parser = argparse.ArgumentParser(description='Render calculation results using ParaView')
parser.add_argument('--task', required=True, help='Task to read render config from')
parser.add_argument('--snap-list', required=True, help='List of snapshots to render')
parser.add_argument('--output-dir', required=True, help='Output directory')
parser.add_argument('--verbose', action="store_true", default=False, help='Be verbose')

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


for rc in task.xpathEval('/task/render/renderConfig'):
    logger.info('Processing render config (%s)' % rc.prop('id'))
    meshes = [x.getContent() for x in rc.xpathEval('meshes/mesh/text()')]
    logger.debug('List of meshes to load: ' + str(meshes))
    
    for snaps in snapshotsList:
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
            plotOverLine(pol, int(snaps['index']), args.output_dir)