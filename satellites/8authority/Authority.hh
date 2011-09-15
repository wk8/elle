//
// ---------- header ----------------------------------------------------------
//
// project       8authority
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef AUTHORITY_AUTHORITY_HH
#define AUTHORITY_AUTHORITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8authority application.
  ///
  class Authority
  {
  public:
    //
    // constants
    //
    static const elle::Natural32		Length;

    //
    // enumerations
    //
    enum Operation
      {
	OperationUnknown = 0,

	OperationCreate,
	OperationDestroy,
	OperationInformation
      };

    //
    // static methods
    //
    static elle::Status		Create();
    static elle::Status		Destroy();
    static elle::Status		Information();
  };

}

#endif
