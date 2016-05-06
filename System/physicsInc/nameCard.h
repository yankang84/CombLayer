/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/nameCard.h
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
#ifndef physicsSystem_nameCard_h
#define physicsSystem_nameCard_h

namespace physicsSystem
{

/*!
  \class nameCard
  \version 1.0
  \date May 2016
  \author S.Ansell
  \brief Named card of form name keyword = value ..
  
  
*/

class nameCard 
{
 private:

  /// enumeration for type
  enum class MData { DBL = 1, INT = 2, STR = 3 };
  
  std::string keyName;                     ///< Main name
  bool active;                             ///< Active

  
  std::vector<std::string> nameOrder;      ///< Ordered list of units
  std::map<std::string,MData> regNames;      ///< Registered names / type
  
  std::map<std::string,double> DUnit;      ///< Double units
  std::map<std::string,long int> IUnit;    ///< int units
  std::map<std::string,std::string> SUnit; ///< string units

  static const std::string& getTypeName(const MData&);
  static MData convertName(const std::string&);
  
 public:
   
  nameCard(const std::string&);
  nameCard(const nameCard&);
  nameCard& operator=(const nameCard&);
  virtual ~nameCard();

  void registerItems(const std::vector<std::string>&);
  
  template<typename T>
  void setItem(const std::string&,const T&);

  template<typename T>
  const T& getItem(const std::string&) const;


  template<typename T>
  void setRegItem(const std::string&,const T&);

  void reset();
  void write(std::ostream&) const;   
};

}

#endif
