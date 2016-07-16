/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essInc/RotaryHoleGenerator.h
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
#ifndef setVariable_RotaryHoleGenerator_h
#define setVariable_RotaryHoleGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class RotaryHoleGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief RotaryHoleGenerator for variables
*/

struct holeInfo
{
  size_t shape;         ///< Shape [number until enum]
  double radius;     ///< Radius
  double xradius;    ///< Radius [if needed]
  double angCent;    ///< Angle centre
  double radStep;    ///< out setp
};
 
  
class RotaryHoleGenerator
{
 private:


  double mainRadius;    ///< Main radius
  double mainThick;     ///< Main thickness
  double innerWall;     ///< inner wall thickness
  std::string innerMat; ///< Inner wall material
  std::string defMat;   ///< Default material

  size_t posIndex;      ///< Position index
  double angOffset;      ///< angle relative to posIndex
  /// Hole definitions
  std::vector<holeInfo> HData;

  
 public:

  RotaryHoleGenerator();
  RotaryHoleGenerator(const RotaryHoleGenerator&);
  RotaryHoleGenerator& operator=(const RotaryHoleGenerator&);
  ~RotaryHoleGenerator();


  void setPosition(const size_t,const double);
  /// access to size
  void setMain(const double R,const double T)
  { mainRadius=R; mainThick=T; }
  void setWall(const double T,const std::string& M)
  { innerWall=T; innerMat=M; }
  void resetHoles() { HData.clear(); }
  void addHole(const std::string&,const double,const double,
	       const double,const double);
  
  
  void generatePinHole(FuncDataBase&,const std::string&,
		       const double,const double)  const;
};

}

#endif
 
