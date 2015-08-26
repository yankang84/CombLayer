/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VOR.h
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
#ifndef essSystem_VOR_h
#define essSystem_VOR_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  class LokiHut;
  class VacTank;

  /*!
    \class VOR
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief VOR beamline constructor for the ESS
  */
  
class VOR
{
 private:

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedComp> vorAxis;

  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  /// Pipe between chopper 1 and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic forcus in bulkshield [m2.5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;

  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  VOR();
  VOR(const VOR&);
  VOR& operator=(const VOR&);
  ~VOR();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
