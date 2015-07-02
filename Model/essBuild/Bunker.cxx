/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "mergeTemplate.h"

#include "World.h"
#include "Bunker.h"

namespace essSystem
{

Bunker::Bunker(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12),
  attachSystem::CellMap(),
  bnkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bnkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bunker::~Bunker() 
  /*!
    Destructor
  */
{}

void
Bunker::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Bunker","populate");

  leftPhase=Control.EvalVar<double>(keyName+"LeftPhase");
  rightPhase=Control.EvalVar<double>(keyName+"RightPhase");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");

  nSectors=Control.EvalVar<size_t>(keyName+"NSectors");
  
  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  ModelSupport::populateDivide(Control,nLayers,keyName+"WallMat",
			       wallMat,wallMatVec);
  ModelSupport::populateDivideLen(Control,nLayers,keyName+"WallLen",
				  wallThick,wallFrac);
  return;
}
  
void
Bunker::createUnitVector(const attachSystem::FixedComp& MainCentre,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex,
			 const bool reverseZ)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: Linked object
    \param sideIndex :: Side for linkage centre
    \param reverseZ :: reverse Z direction
  */
{
  ELog::RegMethod RegA("Bunker","createUnitVector");

  rotCentre=MainCentre.getCentre();
  FixedComp::createUnitVector(FC,sideIndex);
  if (reverseZ)
    {
      X*=-1;
      Z*=-1;
    }
      
  return;
}
  
void
Bunker::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bunker","createSurface");

  innerRadius=rotCentre.Distance(Origin);
  
  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftAngle+leftPhase,Z).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(-(rightAngle+rightPhase),Z).rotate(BWallDir);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Origin-rotCentre);
  Geometry::Vec3D BWall(Origin-rotCentre);
  Geometry::Quaternion::calcQRotDeg(leftPhase,Z).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(-rightPhase,Z).rotate(BWall);

  // Divider
  ModelSupport::buildPlane(SMap,bnkIndex+1,rotCentre,Y);
  ModelSupport::buildCylinder(SMap,bnkIndex+7,rotCentre,Z,wallRadius);
    
  ModelSupport::buildPlane(SMap,bnkIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+4,BWall,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,bnkIndex+6,Origin+Z*roofHeight,Z);

  // Walls
  ModelSupport::buildCylinder(SMap,bnkIndex+17,rotCentre,
			      Z,wallRadius+wallThick);
    
  ModelSupport::buildPlane(SMap,bnkIndex+13,
			   AWall-AWallDir*sideThick,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+14,
			   BWall+BWallDir*sideThick,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+15,
			   Origin-Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,bnkIndex+16,
			   Origin+Z*(roofHeight+roofThick),Z);


  // CREATE Sector boundary lines
  // Note negative subtraction as moving +ve to -ve
  double phaseAngle(leftPhase);
  const double phaseStep((leftPhase+rightPhase)/nSectors);
  double normAngle(leftAngle+leftPhase);
  const double normStep((leftAngle+leftPhase+rightAngle+rightPhase)/nSectors);
  int divIndex(bnkIndex+500);
      
  for(size_t i=1;i<nSectors;i++)
    {
      divIndex++;
      phaseAngle-=phaseStep;
      normAngle-=normStep;
      Geometry::Vec3D DPosition(Origin-rotCentre);
      Geometry::Quaternion::calcQRotDeg(phaseAngle,Z).rotate(DPosition);
      Geometry::Vec3D DNorm(X);
      Geometry::Quaternion::calcQRotDeg(normAngle,Z).rotate(DNorm);
      ModelSupport::buildPlane(SMap,divIndex,DPosition,DNorm);
    }
      
  return;
}

void
Bunker::createSideLinks(const Geometry::Vec3D& AWall,
			const Geometry::Vec3D& BWall,
			const Geometry::Vec3D& AWallDir,
			const Geometry::Vec3D& BWallDir)
  /*!
    Ugly function to create side wall linkes
    \param AWall :: Left wall point
    \param BWall :: Left wall point
   */
{
  ELog::RegMethod RegA("Bunker","createSideLinks");
		      
  // Construct links on side walls:
  Geometry::Vec3D AWallY(AWallDir*Z);
  Geometry::Vec3D BWallY(BWallDir*Z);

  if (AWallY.dotProd(Y)<0.0)
    AWallY*=-1;
  if (BWallY.dotProd(Y)<0.0)
    BWallY*=-1;

  // Outer 
  FixedComp::setConnect(2,AWall+AWallY*wallRadius/2.0,AWallDir);
  FixedComp::setConnect(3,BWall+BWallY*wallRadius/2.0,BWallDir);

  return;
}

  
void
Bunker::createObjects(Simulation& System,
		      const attachSystem::FixedComp& FC,  
		      const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Side of bulk shield + divider(?)
    \param sideIndex :: side of link point
  */
{
  ELog::RegMethod RegA("Bunker","createObjects");
  
  std::string Out;
  const std::string Inner=FC.getSignedLinkString(sideIndex);
  
  Out=ModelSupport::getComposite(SMap,bnkIndex,"1 -7 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Inner));
  setCell("MainVoid",cellIndex-1);

  // left:right:floor:roof:Outer
  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 -3 13 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 4 -14 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 13 -14 -5 15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 13 -14 6 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));

  int divIndex(bnkIndex+500);
  for(size_t i=0;i<nSectors;i++)
    {
      
      const std::string ACut=(!i) ?
	ModelSupport::getComposite(SMap,bnkIndex," 13 ") :
	ModelSupport::getComposite(SMap,divIndex-1," 1M ");
      const std::string BCut=(i+1 == nSectors) ?
	ModelSupport::getComposite(SMap,bnkIndex," -14 ") :
	ModelSupport::getComposite(SMap,divIndex," -1M ");
        Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 7  15 -16 ");
      Out+=ACut+BCut;
	  
      divIndex++;
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      setCell("MainWall"+StrFunc::makeString(i),cellIndex-1);
    }


  // External
  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 13 -14 15 -16 ");
  addOuterSurf(Out);
      
  
  return;
}

void 
Bunker::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("Bunker","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(wallFrac[i-1]);
	  DA.addMaterial(wallMatVec[i-1]);
	}
      DA.addMaterial(wallMatVec.back());

      for(size_t i=0;i<nSectors;i++)
	{
	  // Cell Specific:
	  const int firstCell(cellIndex);
	  const std::string cellName("MainWall"+StrFunc::makeString(i));
	  DA.setCellN(getCell(cellName));
	  DA.setOutNum(cellIndex,bnkIndex+1001);

	  ModelSupport::mergeTemplate<Geometry::Cylinder,
				      Geometry::Cylinder> surroundRule;
	  
	  surroundRule.setSurfPair(SMap.realSurf(bnkIndex+7),
				   SMap.realSurf(bnkIndex+17));
	  
	  OutA=ModelSupport::getComposite(SMap,bnkIndex," 7 ");
	  OutB=ModelSupport::getComposite(SMap,bnkIndex," -17 ");
	  
	  surroundRule.setInnerRule(OutA);
	  surroundRule.setOuterRule(OutB);
	  
	  DA.addRule(&surroundRule);
	  DA.activeDivideTemplate(System);
	  
	  cellIndex=DA.getCellNum();
	  removeCell(cellName);
	  setCells(cellName,firstCell,cellIndex-1);
	}
    }
  return;
}

  
void
Bunker::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Bunker","createLinks");

  FixedComp::setConnect(0,rotCentre+Y*(wallRadius),Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(bnkIndex+7));
  FixedComp::setBridgeSurf(0,SMap.realSurf(bnkIndex+1));
  
  // Outer
  FixedComp::setConnect(1,rotCentre+Y*(wallRadius+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(bnkIndex+17));
  FixedComp::setBridgeSurf(1,SMap.realSurf(bnkIndex+1));
  
  FixedComp::setConnect(4,Origin-Z*(floorDepth+floorThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(bnkIndex+15));
  FixedComp::setConnect(5,Origin+Z*(roofHeight+roofThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(bnkIndex+16));

  // Inner
  FixedComp::setConnect(7,rotCentre+Y*wallRadius,-Y);
  FixedComp::setLinkSurf(7,-SMap.realSurf(bnkIndex+7));

  FixedComp::setConnect(10,Origin-Z*floorDepth,Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(bnkIndex+5));
  FixedComp::setConnect(11,Origin+Z*roofHeight,-Z);
  FixedComp::setLinkSurf(11,-SMap.realSurf(bnkIndex+6));

  return;
}


void
Bunker::createAll(Simulation& System,
		  const attachSystem::FixedComp& MainCentre,
		  const attachSystem::FixedComp& FC,
		  const long int linkIndex,
		  const bool reverseZ)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param MainCentre :: Rotatioin Centre
    \param reverseZ :: Reverse Z direction
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("Bunker","createAll");

  populate(System.getDataBase());
  createUnitVector(MainCentre,FC,linkIndex,reverseZ);
  createSurfaces();
  createLinks();
  createObjects(System,FC,linkIndex);
  layerProcess(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem