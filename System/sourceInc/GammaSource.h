/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   sourceInc/GammaSource.h
*
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef SDef_GammaSource_h
#define SDef_GammaSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class GammaSource
  \version 1.0
  \author S. Ansell
  \date November 2014
  \brief Adds gamma ray circular divergent source
*/

class GammaSource : 
  public attachSystem::FixedOffset
{
 private:
    
  int particleType;             ///< Particle Type
  double cutEnergy;             ///< Energy cut point

  size_t shape;                    ///< Shape type
  double width;                 ///< width
  double height;                ///< height
  double radius;                ///< radius
  double angleSpread;           ///< Angle spread
  
  Geometry::Vec3D FocusPoint;   ///< Focus point
  Geometry::Vec3D Direction;    ///< Beam direction

  double weight;
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
  
  void populate(const FuncDataBase& Control);
  int populateEnergy(std::string,std::string);
  int populateEFile(const std::string&,const int,const int);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void calcPosition();
  void createSource(SDef::Source&) const;
  void createRectangleSource(SDef::Source&) const;
  void createRadialSource(SDef::Source&) const;

 public:

  GammaSource(const std::string&);
  GammaSource(const GammaSource&);
  GammaSource& operator=(const GammaSource&);
  ~GammaSource();

  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }


  void createAll(const FuncDataBase&,SDef::Source&);
  void createAll(const FuncDataBase&,const attachSystem::FixedComp&,
		 const long int,SDef::Source&);
  
};

}

#endif
 
