/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VOR.cxx
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "debugMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"

#include "VOR.h"

namespace essSystem
{

VOR::VOR() :
  vorAxis(new attachSystem::FixedComp("vorAxis",4)),
  FocusA(new beamlineSystem::GuideLine("vorFA")),
  VacBoxA(new constructSystem::VacuumBox("vorVacA")),
  DDisk(new constructSystem::DiskChopper("vorDBlade")),
  DDiskHouse(new constructSystem::ChopperHousing("vorDBladeHouse")),
  VPipeB(new constructSystem::VacuumPipe("vorPipeB")),
  FocusB(new beamlineSystem::GuideLine("vorFB")),
  BInsert(new BunkerInsert("vorBInsert"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("VOR","VOR");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell("vorAxis");
  OR.addObject(vorAxis);

  OR.addObject(FocusA);
  OR.addObject(VacBoxA);
  OR.addObject(DDisk);
  OR.addObject(DDiskHouse);
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(BInsert);
}

VOR::~VOR()
  /*!
    Destructor
  */
{}

void
VOR::setBeamAxis(const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("VOR","setBeamAxis");

  vorAxis->createUnitVector(GItem);
  vorAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  vorAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  vorAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  vorAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    vorAxis->reverseZ();
  return;
}
  
void 
VOR::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("VOR","build");

  ELog::EM<<"\nBuilding VOR on : "<<GItem.getKeyName()<<ELog::endDiag;

  setBeamAxis(GItem,1);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addInsertCell(bunkerObj.getCell("MainVoid"));
  //  FocusA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);
  FocusA->getKey("Guide0").reverseZ();

  // First straight section
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,FocusA->getKey("Guide0"),2);
  FocusA->addInsertCell(VacBoxA->getCells("Void"));
  FocusA->insertObjects(System);
  
  // Double disk chopper
  DDisk->addInsertCell(VacBoxA->getCell("Void",0));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->createAll(System,FocusA->getKey("Guide0"),2);

  // Double disk chopper housing
  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  DDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  DDiskHouse->createAll(System,DDisk->getKey("Main"),0);
  DDiskHouse->insertComponent(System,"Void",*DDisk);

  //  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  //FocusB->insertObjects(System);

  // PIPE
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VacBoxA,2);
  VPipeB->setBack(bunkerObj,1);
  VPipeB->createAll(System,*VacBoxA,2);

  FocusB->addInsertCell(VPipeB->getCell("Void"));
  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  FocusB->createAll(System,DDisk->getKey("Beam"),2,
		    DDisk->getKey("Beam"),2);

  // Make bunker insert
  const attachSystem::FixedComp& GFC(FocusB->getKey("Guide0"));
  const std::string BSector=
    bunkerObj.calcSegment(System,GFC.getSignedLinkPt(-1),
			  GFC.getSignedLinkAxis(-1));  
  BInsert->setInsertCell(bunkerObj.getCells(BSector));
  BInsert->createAll(System,FocusB->getKey("Guide0"),-1,bunkerObj);

  //  FocusB->addInsertCell(BInsert->getCell("Void"));
  BInsert->insertComponent(System,"Void",*FocusB);
  return;
}


}   // NAMESPACE essSystem

