/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESS.h
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
#ifndef essSystem_makeESS_h
#define essSystem_makeESS_h


namespace beamlineSystem
{
  class beamlineConstructor;
}

namespace constructSystem
{
  class ModBase;
  class SupplyPipe;

}

namespace moderatorSystem
{
  class FlightLine;
}

/*!
  \namespace essSystem
  \brief General ESS stuff
  \version 1.0
  \date January 2013
  \author S. Ansell
*/

namespace essSystem
{
  class WheelBase;
  class SegWheel;
  class Wheel;
  class BeRef;
  class essMod;
  class ConicModerator;
  class CylPreMod;
  class BulkModule;
  class ShutterBay;
  class ProtonTube;
  class GuideBay;
  class BeamMonitor;
  class DiskPreMod;

  /*!
    \class makeESS
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class makeESS
{
 private:
  
  std::shared_ptr<WheelBase> Target;   ///< target object
  std::shared_ptr<BeRef> Reflector;    ///< reflector object
  std::shared_ptr<ProtonTube> PBeam;   ///< Proton Void
  std::shared_ptr<BeamMonitor> BMon;   ///< Beam Monitor

  // Butterly
  /// Primary Lower Mod [
  std::shared_ptr<constructSystem::ModBase> LowMod;
  std::shared_ptr<DiskPreMod> LowPreMod;

  // Cylindrical [to be dumped]
  std::shared_ptr<moderatorSystem::FlightLine> LowAFL;  ///< Lower Mode FL
  std::shared_ptr<moderatorSystem::FlightLine> LowBFL;  ///< Lower Mode FL
  std::shared_ptr<CylPreMod> LowPre;          ///< Lower Mod (Pre)

  
  /// Lower supply 
  std::shared_ptr<constructSystem::SupplyPipe> LowSupplyPipe; 
  std::shared_ptr<constructSystem::SupplyPipe> LowReturnPipe;  ///< Lower supply 

  std::shared_ptr<constructSystem::ModBase> TopMod;   ///< Upper Mod
  std::shared_ptr<moderatorSystem::FlightLine> TopAFL;  ///< Upper Mode FL
  std::shared_ptr<moderatorSystem::FlightLine> TopBFL;  ///< Upper Mode FL
  std::shared_ptr<CylPreMod> TopPre;  ///< Upper Mod (Pre)

  std::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  std::shared_ptr<moderatorSystem::FlightLine> BulkLowAFL;  ///< Lower Mode FL

  /// Shutterbay objects
  std::shared_ptr<ShutterBay> ShutterBayObj;  
  /// Array of Guidies
  std::vector<std::shared_ptr<GuideBay> > GBArray;  

  /// Array of beamlines constructors:
  std::vector<std::shared_ptr<beamlineSystem::beamlineConstructor> > 
    BLArray;  

  void topFlightLines(Simulation&);
  void lowFlightLines(Simulation&);
  void createGuides(Simulation&);

  void buildLowMod(Simulation&);
  void buildLowButterfly(Simulation&);
  void buildLowPreMod(Simulation&);
  void buildLowCylMod(Simulation&);
  void buildLowConicMod(Simulation&);
  void buildLayerMod(Simulation&);
  void buildTopCylMod(Simulation&);
  void buildLowerPipe(Simulation&,const std::string&);

  void makeTarget(Simulation&,const std::string&);

  void makeBeamLine(Simulation&,
		    const mainSystem::inputParam&);
  void optionSummary(Simulation&);

 public:
  
  makeESS();
  makeESS(const makeESS&);
  makeESS& operator=(const makeESS&);
  ~makeESS();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
