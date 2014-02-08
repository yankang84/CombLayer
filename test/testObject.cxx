/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testObject.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Debug.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "neutron.h"

#include "testFunc.h"
#include "testObject.h"

using namespace MonteCarlo;

testObject::testObject() 
  /*!
    Constructor
  */
{}

testObject::~testObject() 
  /*!
    Destructor
  */
{
  deleteObject();
}

void 
testObject::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObject","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  // Second box :
  SurI.createSurface(11,"px -3");
  SurI.createSurface(12,"px 3");
  SurI.createSurface(13,"py -3");
  SurI.createSurface(14,"py 3");
  SurI.createSurface(15,"pz -3");
  SurI.createSurface(16,"pz 3");

  // Far box :
  SurI.createSurface(21,"px 10");
  SurI.createSurface(22,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
  return;
}

void
testObject::deleteObject()
  /*!
    Delete all the objects
  */
{
  ELog::RegMethod RegA("testObject","deleteObject");

  OTYPE::iterator mc;
  for(mc=MObj.begin();mc!=MObj.end();mc++)
    delete mc->second;
  MObj.erase(MObj.begin(),MObj.end());
  return;
}

void
testObject::populateMObj()
  /*!
    Populate with simple object
    the MLis component
  */
{
  ELog::RegMethod RegA("testObject","populateMObj");
  // Define entries
  typedef std::map<int,std::string> MTYPE;
  MTYPE MString;
  MString.insert(MTYPE::value_type(3,"3 2  0.062 60001 -60002 60003 -60004 60005 -60006"));
  MString.insert(MTYPE::value_type(2,"2 12 0.099 -4  5  60  -61  62  -63"));
  MString.insert(MTYPE::value_type(8,"8 12 0.099  -12 13 60  -61  62  -63"));
  MString.insert(MTYPE::value_type(10,"10 2 0.062 "
				   "80001 ((-80002 80003) : -80004 ) "
				   "80005 -80006"));
  MString.insert(MTYPE::value_type(12,"12 0 63 64 (-61 : -62)")); 

  // Clear map
  deleteObject();

  // Insert:
  MTYPE::const_iterator mc;
  for(mc=MString.begin();mc!=MString.end();mc++)
    {
      MObj[mc->first]=new Qhull();
      MObj[mc->first]->setObject(mc->second);
    }
  return;
}

int 
testObject::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testObject","applyTest");
  TestFunc::regSector("testObject");

  typedef int (testObject::*testPtr)();
  testPtr TPtr[]=
    {
      &testObject::testCellStr,
      &testObject::testComplement,
      &testObject::testIsValid,
      &testObject::testIsOnSide,
      &testObject::testMakeComplement,
      &testObject::testRemoveComplement,
      &testObject::testSetObject,
      &testObject::testTrackCell
    };
  const std::string TestName[]=
    {
      "CellStr",
      "Complement",
      "IsValid",
      "IsOnSide",
      "MakeComplement",
      "RemoveComplement",
      "SetObject",
      "TrackCell"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(int i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
      return 0;
    }
  for(int i=0;i<TSize;i++)
    {
      if (extra<0 || extra==i+1)
        {
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}


int
testObject::testCellStr()
  /*!
    Test the CellSTr method including the 
    complementary of both \#(XXX) and \#Cell.
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testCellStr");

  populateMObj();

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("4 10 0.0552 -5 8 60 (-61:62) -63 #3",
			"#(-60006 60005 -60004 60003 -60002 60001)"
			"  -63 ( -61 : 62 ) 60 8 -5"));
  Tests.push_back(TTYPE("5 1 0.05524655 18 45 #(45 (57 : 56))",
			"#( ( 57 : 56 ) 45 ) 45 18"));

  Tests.push_back(TTYPE("5 1 0.05524655 18 45 #(45 (57 : 56))",
			"#( ( 57 : 56 ) 45 ) 45 18"));

  // quick way to make a long long list:
  std::ostringstream cx;
  std::ostringstream ox;
  const int nTestSurf(100);
  for(int i=1;i<nTestSurf;i++)
    {
      cx<<i<<" ";
      ox<<nTestSurf-i<<" ";
    }
  Tests.push_back(TTYPE("5 0 #("+cx.str()+")",
			"#( "+ox.str()+")"));

  std::vector<TTYPE>::const_iterator tc;
  Qhull A;  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setObject(tc->get<0>());
      const std::string Xstr =A.cellStr(MObj);
      if (Xstr!=tc->get<1>())
	{
	  ELog::EM<<"Init Obj:"<<tc->get<0>()<<ELog::endDiag;
	  ELog::EM<<"Out Obj:"<<Xstr<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}


int
testObject::testSetObject() 
  /*!
    Test the processing of a line
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testSetObject");

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(" 4 10 0.05524655  -5  8  60  -61  62  -63 #3",
			"4 10 0.0552465 #3 -63 62 -61 60 8 -5"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::ostringstream cx;
      Qhull A;
      A.setObject(tc->get<0>());
      A.write(cx);
      if (StrFunc::fullBlock(cx.str())!=tc->get<1>())
	{
	  ELog::EM<<"Failed on test "<<(tc-Tests.begin())+1
		  <<ELog::endTrace;
	  ELog::EM<<"Input == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"Expect == "<<tc->get<1>()<<ELog::endTrace;
	  ELog::EM<<"Result == "<<cx.str()<<ELog::endTrace;
	  return -1;
	}
    }

  return 0;
}

int
testObject::testComplement() 
  /*!
    Test the removal of a complement
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testComplement");

  populateMObj();

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6  #12",
			"i'g'e'fhj(d'+c'+ab)"));
  Tests.push_back(TTYPE("5 10 0.05524655  #(1 2 3 4)","d'+c'+b'+a'"));
  Tests.push_back(TTYPE("5 10 0.05524655  #(34 44 (-84 : 82))","d'+c'+b'a"));
  Tests.push_back(TTYPE("5 10 0.05524655  #(-34 -44)","a+b"));
  Tests.push_back(TTYPE("5 10 0.05524655  #(-8 (-9 : 19) 3 -4)",
			"b'+a+e+d'c"));
  
  std::vector<TTYPE>::const_iterator vc;
  for(vc=Tests.begin();vc!=Tests.end();vc++)
    {
      Qhull A;
      A.setObject(vc->get<0>());
      if (!A.hasComplement())
	{
	  ELog::EM<<"Has no complements "<<A.hasComplement()<<ELog::endDiag;
	  return -1;
	}

      Algebra AX;

      AX.setFunctionObjStr(A.cellStr(MObj));
      if (!A.procString(AX.writeMCNPX()))
	{
	  ELog::EM<<"Failed to set string "
		  <<AX.writeMCNPX()<<ELog::endDiag;
	  return -2;
	}

      AX.setFunctionObjStr(A.cellStr(MObj));
      std::ostringstream cx;
      cx<<AX;
      if (cx.str()!=vc->get<1>())
	{
	  ELog::EM<<"Obj == :"<<A.cellStr(MObj)<<ELog::endDiag;
	  ELog::EM<<"AX  == :"<<AX<<ELog::endDiag;
	  ELog::EM<<"Exp == :"<<vc->get<1>()<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}

int
testObject::testIsValid() 
  /*!
    Test the object being valid both on and off surface
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testIsValid");

  createSurfaces();
  Qhull A;

  // Object : Point : SN : Result
  typedef boost::tuple<std::string,int,Geometry::Vec3D,int> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",
			0,Geometry::Vec3D(0,0,0),1));
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",
			0,Geometry::Vec3D(0,0,1),1));
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",
			0,Geometry::Vec3D(0,0,1.01),0));
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",
			-6,Geometry::Vec3D(0,0,1.01),1));
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",
			6,Geometry::Vec3D(0,0,1.01),0));
  // Test to exit sphere
  Tests.push_back(TTYPE("5 10 0.05524655 -100",
			-100,Geometry::Vec3D(0,0,25.01),1));
  Tests.push_back(TTYPE("5 10 0.05524655 -100",
			-100,Geometry::Vec3D(0,0,24.01),1));
  Tests.push_back(TTYPE("5 10 0.05524655 -100",
			100,Geometry::Vec3D(0,0,26.01),0));


  
  int res;

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setObject(tc->get<0>());
      A.populate();
      const int SN=tc->get<1>();
      if (SN)
	res=A.isDirectionValid(tc->get<2>(),SN);
      else
	res=A.isValid(tc->get<2>());
      if (res!=tc->get<3>())
	{
	  ELog::EM<<"Failed on test "<<(tc-Tests.begin())+1<<ELog::endDebug;
	  ELog::EM<<"Result= "<<res<<" ["<<tc->get<3>()<<"]"<<ELog::endDebug;
	  ELog::EM<<"Point= "<<tc->get<2>()<<ELog::endDebug;

	  const Rule* TR=A.topRule();
	  ELog::EM<<"Display= "<<TR->display(tc->get<2>())<<ELog::endDebug;
	}
    }

  return 0;
}

int
testObject::testIsOnSide() 
  /*!
    Test the object being valid both on and off surface
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testIsOnSide");

  createSurfaces();
  Qhull A;

  // Object : Point : SN : Result
  typedef boost::tuple<std::string,Geometry::Vec3D,int> TTYPE;
  std::vector<TTYPE> Tests;
  


  Tests.push_back(TTYPE("4 10 0.05 1 -2 3 -4 5 -6",
			Geometry::Vec3D(0,0,0),0));
  Tests.push_back(TTYPE("4 10 0.05 1 -2 3 -4 5 -6",
			Geometry::Vec3D(0.5,-1,0.5),3));
  Tests.push_back(TTYPE("4 10 0.05 1 -2 3 -4 5 -6",
			Geometry::Vec3D(0,-1,1),3));
  Tests.push_back(TTYPE("4 10 0.05 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)",
			Geometry::Vec3D(0,1,3),-16));
  Tests.push_back(TTYPE("4 10 0.05 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)",
			Geometry::Vec3D(0,1,0.5),4));
  Tests.push_back(TTYPE("4 10 0.05 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)",
			Geometry::Vec3D(0,-1,1),0));
  Tests.push_back(TTYPE("4 10 0.05 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)",
			Geometry::Vec3D(2,-1,2),0));
  Tests.push_back(TTYPE("4 10 0.05 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)",
			Geometry::Vec3D(4,-1,2),0));
  
  std::vector<TTYPE>::const_iterator tc;
  //  for(tc=Tests.begin();tc!=Tests.end();tc++)
  tc=Tests.begin()+5;
    {
      A.setObject(tc->get<0>());
      A.populate();
      A.createSurfaceList();
      const int res=A.isOnSide(tc->get<1>());
      if (res!=tc->get<2>())
	{
	  ELog::EM<<"Failed on test "<<(tc-Tests.begin())+1<<ELog::endDebug;
	  ELog::EM<<"Result= "<<res<<" ["<<tc->get<2>()<<"]"<<ELog::endDebug;
	  ELog::EM<<"Point= "<<tc->get<1>()<<ELog::endDebug;
	  const Rule* TR=A.topRule();
	  ELog::EM<<"Display= "<<TR->display(tc->get<1>())<<ELog::endDebug;
	  return -1;
	}
    }

  return 0;
}

int
testObject::testTrackCell() 
  /*!
    Test the track through a cell
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObject","testTrackCell");

  createSurfaces();
  Qhull A;

  // Object : startSurf : Neut : ResultPos
  typedef boost::tuple<std::string,int,Geometry::Vec3D,
		       Geometry::Vec3D,Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",2,
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0),
			Geometry::Vec3D(1,0,0)));

  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",-1,
			Geometry::Vec3D(-1.01,0,0),Geometry::Vec3D(1,0,0),
			Geometry::Vec3D(1,0,0)));

  Tests.push_back(TTYPE("4 10 0.05524655  1 -2 3 -4 5 -6",2,
			Geometry::Vec3D(3.01,0,0),Geometry::Vec3D(-1,0,0),
			Geometry::Vec3D(-1,0,0)));
  
  double aDist;
  const Geometry::Surface* SPtr;          // Output surface
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setObject(tc->get<0>());
      A.populate();
      A.createSurfaceList();
      ELog::EM<<"Test "<<tc-Tests.begin()<<ELog::endDiag;

      neutron TNeut(1,tc->get<2>(),tc->get<3>());

      const int outFaceSurf(tc->get<1>());
      ELog::EM<<"HASDFASDF"<<ELog::endDiag;

      const int SN= -A.trackOutCell(TNeut,aDist,SPtr,outFaceSurf);
      TNeut.moveForward(aDist);
      if (TNeut.Pos!=tc->get<4>())
	{
	  ELog::EM<<"Failed on test "<<(tc-Tests.begin())+1<<ELog::endDiag;
	  ELog::EM<<"Result= "<<TNeut.Pos<<" ["<<tc->get<4>()<<"]"<<ELog::endDebug;
	  ELog::EM<<"SN= "<<SN<<" "<<outFaceSurf<<ELog::endDiag;

	  const Rule* TR=A.topRule();
	  ELog::EM<<"Display= "<<TR->display(TNeut.Pos)<<ELog::endDiag;
	}
    }
  return 0;
}

int
testObject::testMakeComplement()
  /*!
    Test the making of a given object complementary
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testObject","testMakeComplement");

  populateMObj();

  typedef boost::tuple<int,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE(2,"2 12 0.099 (-5 : -60 : -62 : 63 : 61 : 4)"));
  std::vector<TTYPE>::const_iterator vc;
  for(vc=Tests.begin();vc!=Tests.end();vc++)
    {
      std::ostringstream ocx;
      std::ostringstream cx;
      const int index(vc->get<0>());
      MObj[index]->write(ocx);
      MObj[index]->makeComplement();
      MObj[index]->write(cx);
      if (StrFunc::fullBlock(cx.str())!=vc->get<1>())
	{
	  ELog::EM<<"Original "<<ocx.str()<<":"<<ELog::endDebug;
	  ELog::EM<<"Expected "<<vc->get<1>()<<":"<<ELog::endDebug;
	  ELog::EM<<"Aquired  "<<cx.str()<<":"<<ELog::endDebug;
	  return -1;
	}
    }      
  return 0;  
}

int
testObject::testRemoveComplement()
  /*!
    Tests the removal of a complement
    \return -ve on error
   */
{
  ELog::RegMethod RegA("testObject","removeComplement");
  
  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("3 0 5 ","3 0 -5"));
  Tests.push_back(TTYPE("4 0 5 ((1 -2 ) : (13 -14))",
			"4 0 (-5 : ( ( -1 : 2 ) ( -13 : 14 ) ))"));

  // quick way to make a long long list:
  std::ostringstream cx;
  std::ostringstream ox;
  const int N(73);
  for(int i=1;i<N;i++)
    cx<<(i+10)<<" ";
  for(int i=N-1;i>0;i--)
    {
      if (i!=N-1)
	ox<<" : ";
      ox<<-(i+10);

    }

  Tests.push_back(TTYPE("5 0 "+cx.str(),
			"5 0 ("+ox.str()+")"));

  // Null list since we are not using #CellNum
  std::map<int,MonteCarlo::Qhull*> OList;

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::ostringstream cx;
      Qhull A;
      MonteCarlo::Algebra AX;
      A.setObject(tc->get<0>());
      A.makeComplement();
      AX.setFunctionObjStr(A.cellStr(OList));
      A.procString(AX.writeMCNPX());
      cx<<A;
      if (StrFunc::singleLine(cx.str())!=tc->get<1>())
	{
	  ELog::EM<<"Failed on test "<<(tc-Tests.begin()+1)<<ELog::endTrace;
	  ELog::EM<<"Ax == "<<AX<<ELog::endTrace;	  
	  ELog::EM<<"Amc == "<<AX.writeMCNPX()<<ELog::endTrace;	  
	  ELog::EM<<"A == "<<A.cellStr(OList)<<ELog::endTrace;
	  ELog::EM<<"A(complement)  = "<<A;
	  ELog::EM<<"A(expect)      = "<<tc->get<1>()<<ELog::endTrace;
	  ELog::EM<<"Str compared)  = "<<StrFunc::singleLine(cx.str())
		  <<ELog::endTrace;
	  ELog::EM<<"A(original)    = "<<tc->get<0>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

