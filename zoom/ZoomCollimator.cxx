/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomCollimator.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "shutterBlock.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "BulkShield.h"
#include "ScatterPlate.h"
#include "ChipIRFilter.h"
#include "ZoomChopper.h"
#include "ZoomStack.h"
#include "ZoomCollimator.h"

namespace zoomSystem
{

ZoomCollimator::ZoomCollimator(const std::string& Key) : 
  attachSystem::TwinComp(Key,6),attachSystem::ContainedComp(),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),populated(0),
  cStack("zoomColStack"),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomCollimator::ZoomCollimator(const ZoomCollimator& A) : 
  attachSystem::TwinComp(A),attachSystem::ContainedComp(A),
  colIndex(A.colIndex),
  cellIndex(A.cellIndex),populated(A.populated),
  cStack(A.cStack),length(A.length),height(A.height),
  depth(A.depth),leftWidth(A.leftWidth),
  rightWidth(A.rightWidth),stackFullWidth(A.stackFullWidth),
  stackLength(A.stackLength),nLayers(A.nLayers),
  feMat(A.feMat),innerVoid(A.innerVoid),cFrac(A.cFrac),
  cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: ZoomCollimator to copy
  */
{}

ZoomCollimator&
ZoomCollimator::operator=(const ZoomCollimator& A)
  /*!
    Assignment operator
    \param A :: ZoomCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::TwinComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      cStack=A.cStack;
      length=A.length;
      height=A.height;
      depth=A.depth;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      stackFullWidth=A.stackFullWidth;
      stackLength=A.stackLength;
      nLayers=A.nLayers;
      feMat=A.feMat;
      innerVoid=A.innerVoid;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}


ZoomCollimator::~ZoomCollimator() 
  /*!
    Destructor
  */
{}

void
ZoomCollimator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomCollimator","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  length=Control.EvalVar<double>(keyName+"Length");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
  rightWidth=Control.EvalVar<double>(keyName+"RightWidth");

  stackFullWidth=Control.EvalVar<double>(keyName+"StackFullWidth");

  feMat=Control.EvalVar<int>(keyName+"FeMat");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  // Need widthh/eight from the divide [if used]
  const double minDist(std::min(leftWidth,rightWidth));
  ModelSupport::populateDivide(Control,nLayers,keyName+"Mat_",
   			       feMat,cMat);
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",minDist,cFrac);

  populated=1;
  return;
}

void
ZoomCollimator::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
    \param TC :: TwinComp to attach to
  */
{
  ELog::RegMethod RegA("ZoomCollimator","createUnitVector");
  
  TwinComp::createUnitVector(TC);
  Origin+=X*xStep+Z*zStep;
  return;
}

void
ZoomCollimator::createSurfaces(const attachSystem::FixedComp& LC)
  /*!
    Create All the surfaces
    \param LC :: Linear Component [to get outer surface]
  */
{
  ELog::RegMethod RegA("ZoomCollimator","createSurface");

  SMap.addMatch(colIndex+1,LC.getLinkSurf(1));   // back plane
  //  SMap.addMatch(colIndex+14,LC.getLinkSurf(3));   // right plane

  ModelSupport::buildPlane(SMap,colIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,colIndex+3,Origin-X*leftWidth,X);
  ModelSupport::buildPlane(SMap,colIndex+4,Origin+X*rightWidth,X);
  ModelSupport::buildPlane(SMap,colIndex+5,Origin-Z*depth,Z);
  // Should be the height of the chopper:
  SMap.addMatch(colIndex+6,LC.getLinkSurf(5));   // right plane
  //  ModelSupport::buildPlane(SMap,colIndex+6,Origin+Z*height,Z);

  // inner void
  ModelSupport::buildPlane(SMap,colIndex+23,
			   bEnter-X*stackFullWidth/2.0,X);
  ModelSupport::buildPlane(SMap,colIndex+24,
			   bEnter+X*stackFullWidth/2.0,X);
    
  return;
}

void
ZoomCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomCollimator","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      
  Out+=ModelSupport::getComposite(SMap,colIndex," (-23:24) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));

  // Inner void:
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 23 -24 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  innerVoid=cellIndex-1;

  return;
}

void 
ZoomCollimator::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("ZoomCollimator","createLinks");

  FixedComp::setConnect(0,Origin,-Y);       
  FixedComp::setConnect(1,Origin+Y*length,Y);     
  FixedComp::setConnect(2,Origin-X*leftWidth/2.0,-X);     
  FixedComp::setConnect(3,Origin+X*rightWidth/2.0,X);     
  FixedComp::setConnect(4,Origin-Z*depth,-Z);     
  FixedComp::setConnect(5,Origin+Z*height,Z);     
  
  for(size_t i=0;i<6;i++)
    FixedComp::setLinkSurf
      (i,SMap.realSurf(colIndex+static_cast<int>(i)+1));

  setBeamExit(colIndex+2,bEnter,bY);

  return;
}

void 
ZoomCollimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that split cells:
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ZoomCollimator","LayerProcess");

  if (!nLayers) return;
  ModelSupport::surfDivide DA;
  // Generic [nLayers > 0 ]
  for(size_t i=0;i<nLayers-1;i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[nLayers-1]);

  const size_t CSize=CDivideList.size();
  for(size_t i=0;i<CSize;i++)
    {
      DA.init(); 
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,colIndex+201+100*static_cast<int>(i));

      DA.makePair<Geometry::Plane>(SMap.realSurf(colIndex+23),
				   -SMap.realSurf(colIndex+3));
      DA.makePair<Geometry::Plane>(SMap.realSurf(colIndex+24),
       				   SMap.realSurf(colIndex+4));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}


int
ZoomCollimator::exitWindow(const double Dist,
			   std::vector<int>& window,
			   Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Distance from window
    \param window :: window vector of paired planes
    \param Pt :: Point for 
    \return Point at exit + Dist
  */
{
  window.clear();
  // Not valid numbers:
  window.push_back(SMap.realSurf(colIndex+23));
  window.push_back(SMap.realSurf(colIndex+24));
  window.push_back(SMap.realSurf(colIndex+25));
  window.push_back(SMap.realSurf(colIndex+26));
  // Note cant rely on exit point because that is the 
  // virtual 46 degree exit point.
  Pt=Origin+Y*(length+Dist); 
  return SMap.realSurf(colIndex+2);
}

void
ZoomCollimator::createAll(Simulation& System,
			  const zoomSystem::ZoomChopper& ZC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZC :: Zoom chopper
  */
{
  ELog::RegMethod RegA("ZoomCollimator","createAll");
  
  populate(System);
  createUnitVector(ZC);
  createSurfaces(ZC);
  createObjects(System);
  createLinks();
  layerProcess(System);
  insertObjects(System);   

  cStack.addInsertCell(innerVoid);
  cStack.createAll(System,ZC);
  
  return;
}
  
}  // NAMESPACE shutterSystem
