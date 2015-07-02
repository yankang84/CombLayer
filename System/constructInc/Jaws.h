/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Jaws.h
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
#ifndef constructSystem_Jaws_h
#define constructSystem_Jaws_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class Jaws
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief Jaws unit  
*/

class Jaws : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int jawIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< step in X 
  double yStep;                 ///< step in X 
  double zStep;                 ///< step in X 
  double xAngle;                ///< rotation about X [last]
  double yAngle;                ///< rotation about Y [mid]
  double zAngle;                ///< rotation about Z [first]
  
  double zOpen;                 ///< Z-opening
  double zThick;                ///< Blade thickness
  double zCross;                ///< Z-cross distance [height]
  double zLen;                  ///< Z-length [away from beam]
  
  double xOpen;                 ///< X-opening
  double xThick;                ///< X-thickness
  double xCross;                ///< X-cross distance [height]
  double xLen;                  ///< X-length [away from beam]

  double jawGap;                ///< Gap of the jaws

  double XHeight;               ///< Global box space [X]
  double YHeight;               ///< Global box space [Y]
  double ZHeight;               ///< Global box space [Z]

  double wallThick;             ///< Simple X/Z walls
  
  int zJawMat;                  ///< Z material
  int xJawMat;                  ///< X material

  int wallMat;                  ///< Wall material

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Jaws(const std::string&);
  Jaws(const Jaws&);
  Jaws& operator=(const Jaws&);
  virtual ~Jaws();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 