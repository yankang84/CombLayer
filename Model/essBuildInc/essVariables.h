/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/essVariables.h
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
#ifndef setVariable_essVariables_h
#define setVariable_essVariables_h


namespace mainSystem
{
  class inputParam;
}

namespace setVariable
{
  // Structure
  void EssBeamLinesVariables(FuncDataBase&);
  void EssBunkerVariables(FuncDataBase&);
  void EssButterflyModerator(FuncDataBase&);
  void EssWheel(FuncDataBase&);
  void EssProtonBeam(FuncDataBase&);
  void EssIradVariables(FuncDataBase&);
  void EssFlightLineVariables(FuncDataBase&);
  void F5Variables(FuncDataBase&);

  // ESS Beamlines
  void BEERvariables(FuncDataBase&);
  void BIFROSTvariables(FuncDataBase&);
  void CSPECvariables(FuncDataBase&);
  void DREAMvariables(FuncDataBase&);
  void ESTIAvariables(FuncDataBase&);
  void FREIAvariables(FuncDataBase&);
  void LOKIvariables(FuncDataBase&);
  void MAGICvariables(FuncDataBase&);
  void NMXvariables(FuncDataBase&);
  void ODINvariables(FuncDataBase&);
  void VORvariables(FuncDataBase&);
  void VESPAvariables(FuncDataBase&);

  // MODIFIED ESS Beamlines
  void shortDREAMvariables(FuncDataBase&);
  void shortDREAM2variables(FuncDataBase&);
  void shortODINvariables(FuncDataBase&);
  void longLOKIvariables(FuncDataBase&);

  // SIMPLE TEST ITEMS
  void simpleITEMvariables(FuncDataBase&);
}


#endif
