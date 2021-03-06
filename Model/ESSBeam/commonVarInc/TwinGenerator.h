/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/TwinGenerator.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_TwinGenerator_h
#define essSystem_TwinGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class TwinGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief TwinGenerator for variables
*/

class TwinGenerator
{
 private:

  double stepHeight;       ///< Height to move main cylinder apart
  double mainRadius;       ///< outer radius of disks
  double innerRadius;      ///< radius of disk voids
  double innerTopStep;     ///< Step above centre for top disk
  double innerLowStep;     ///< Step below centre for lower disk

  double portRadius;       ///< Main Port radius
  double portOuter;        ///< Port outer boundary
  size_t portNBolt;        ///< Number of bolts
  double portBoltRadius;   ///< Port bolt size [M8 currently]

  double viewWidth;         ///< View square width
  double viewHeight;        ///< View square height
  double viewLength;        ///< View thickness
  double viewWindowThick;   ///< View thickness
  double viewBoltStep;      ///< View port bolt step
  size_t viewNBolt;         ///< Number of bolts in view port
  double viewBoltRadius;    ///< View bolt radius
  std::string viewWindowMat;  ///< Neutron beam window
  std::string viewMat;       ///< Main port window
  std::string viewBoltMat;    ///< viewi bolt mater

  
  double motorRadius;      ///< Motor radius
  double motorOuter;       ///< Motor outer
  size_t motorNBolt;       ///< Number of bolts
  double motorBoltRadius;  ///< Number of bolts
  double motorSealThick;   ///< Seal thickness
  std::string motorSealMat;     ///< Seal material

  size_t ringNBolt;         ///< Bolts in outer ring
  size_t lineNBolt;         ///< Bolts in outer line
  double outerStep;         ///< Bolts step from outer edge
  double outerBoltRadius;   ///< Radius of outer bolts
  std::string outerBoltMat;     ///< Main wall material
  
  std::string wallMat;     ///< Main wall material
  
 public:

  TwinGenerator();
  TwinGenerator(const TwinGenerator&);
  TwinGenerator& operator=(const TwinGenerator&);
  ~TwinGenerator();


  void setMaterial(const std::string&,const std::string&);
  void setFrame(const double,const double);  
  void setMainRadius(const double);


  void generateChopper(FuncDataBase&,
		       const std::string&,
		       const double,const double,
		       const double);
};

}

#endif
 
