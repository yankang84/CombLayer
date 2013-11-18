/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomCollimator.h
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
#ifndef zoomSystem_ZoomCollimator_h
#define zoomSystem_ZoomCollimator_h

class Simulation;


namespace zoomSystem
{
  class ZoomChopper;

/*!
  \class ZoomCollimator
  \version 1.0
  \author S. Ansell
  \date September 2011

  \brief Zoom Collimator unit
  
  The guide is defined along a line that leads from the centre line
  of the shutter. The origin is the point that the shutter centre 
  line impacts the 40000 outer surface
*/

class ZoomCollimator : public attachSystem::TwinComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int colIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not

  ZoomStack cStack;            ///< Zoom stack

  double xStep;                ///< Master offset 
  double zStep;                ///< Master offset 

  double length;               ///< Full length
  double height;               ///< Full height
  double depth;                ///< Full height
  double leftWidth;            ///< Full width
  double rightWidth;           ///< Right width



  double stackFullWidth;        ///< Inner void width
  double stackLength;           ///< Stack length

  size_t nLayers;               ///< number of layers
  int feMat;                    ///< Iron material
  int innerVoid;                ///< Inner void cell

  std::vector<double> cFrac;      ///< Layer thicknesss (fractions)
  std::vector<int> cMat;          ///< Layer materials
  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  ZoomCollimator(const std::string&);
  ZoomCollimator(const ZoomCollimator&);
  ZoomCollimator& operator=(const ZoomCollimator&);
  virtual ~ZoomCollimator();

  void createAll(Simulation&,const zoomSystem::ZoomChopper&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

};

}

#endif
 
