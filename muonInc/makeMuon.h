/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/makeMuon.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_makeMuon_h
#define muSystem_makeMuon_h


/*!
  \namespace muSystem
  \brief Muon beamline model
  \version 1.0
  \author G. Skoro
  \date October 2013
*/

namespace muSystem
{
  class targSimpleShield;
  class targetVesselBox; 
  class muonCarbonTarget;
  class muonTube;
  class coneColl;  
  class cShieldLayer; 
  class cShieldLayer; 
  class profileMon;                 
  class muonQ1;
  class muBeamWindow;  

  /*!
    \class makeMuon
    \version 1.0
    \author G. Skoro
    \date October 2013
    \brief General constructor for muon beamline
  */

class makeMuon
{
 private:

  boost::shared_ptr<targSimpleShield> TargShieldObj;   // Target vessel 
  boost::shared_ptr<targetVesselBox> VessBoxObj;   // Target vessel 
  boost::shared_ptr<muonCarbonTarget> TargetObj;   // Target 
  boost::shared_ptr<muonTube> EpbInTubeObj;   // Tube in - epb
  boost::shared_ptr<muonTube> EpbOutTubeObj;   // Tube out - epb
  boost::shared_ptr<coneColl> FirstCollObj;   // 1st Collimator        
  boost::shared_ptr<coneColl> ThirdCollObj;   // 3rd Collimator        
  boost::shared_ptr<cShieldLayer> FirstShieldObj;   // Muon collimator
  boost::shared_ptr<muonTube> FirstTubeObj;   // Tube in 1st shield  
  boost::shared_ptr<cShieldLayer> ThirdShieldObj;   // Shield layered 
  boost::shared_ptr<muonTube> ThirdTubeObj;   // Tube in 3rd layered shield  
  boost::shared_ptr<profileMon> ProfMonObj;   // Profile monitor   
  boost::shared_ptr<muonQ1> Q44Obj;   // quadropoles 
  boost::shared_ptr<muonTube> FinalTubeObj;   // final tube            
  boost::shared_ptr<targSimpleShield> MuRoomObj;   // room
  boost::shared_ptr<muonQ1> Q1Obj;   // quadropoles 
  boost::shared_ptr<muonQ1> Q2Obj;   // quadropoles
  boost::shared_ptr<targSimpleShield> CernFront;   // cern quad magnet front "plate"    
  boost::shared_ptr<targSimpleShield> CernBack;   // cern quad magnet back "plate"
  boost::shared_ptr<muBeamWindow> BWin1Obj;   // beam window 
  boost::shared_ptr<muBeamWindow> BWin2Obj;   // beam window 
  boost::shared_ptr<muonTube> PosRing1Obj;   // position ring 1
  boost::shared_ptr<muonTube> MuTube1Obj;   // muon vacuum tube 1
  boost::shared_ptr<muonTube> MuTube2Obj;   // muon vacuum tube 2      
  boost::shared_ptr<muonTube> PosRing2Obj;   // position ring 2
  boost::shared_ptr<muonTube> MuTube3Obj;   // muon vacuum tube 3       
  boost::shared_ptr<muonTube> PosRing3Obj;   // position ring 3    
  boost::shared_ptr<muonTube> MuTube4Obj;   // muon vacuum tube 4   
  boost::shared_ptr<muonTube> MuTube5Obj;   // muon vacuum tube 5   
  boost::shared_ptr<muonTube> PosRing4Obj;   // position ring 4
  boost::shared_ptr<cShieldLayer> ShieldPlugObj;   // Shield plug layered 
            
 public:
  
  makeMuon();
  makeMuon(const makeMuon&);
  makeMuon& operator=(const makeMuon&);
  ~makeMuon();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
