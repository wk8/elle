//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infi...hole/implementations/cirkle/Neighbour.hh
//
// created       julien quintard   [wed aug 24 10:33:01 2011]
// updated       julien quintard   [sat aug 27 20:46:03 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOUR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/Label.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Neighbour:
	public elle::Object
      {
      public:
	//
	// enumerations
	//
	enum State
	  {
	    StateUnauthenticated,
	    StateAuthenticated
	  };

	//
	// constructors & destructors
	//
	Neighbour();
	~Neighbour();

	//
	// methods
	//
	elle::Status	Create(const elle::Point&);
	elle::Status	Create(elle::Gate*);

	elle::Status	Connect();

	//
	// callbacks
	//
	elle::Status	Monitor();

	//
	// interfaces
	//

	// object
	declare(Neighbour);

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;

	// archivable
	elle::Status	Serialize(elle::Archive&) const;
	elle::Status	Extract(elle::Archive&);

	//
	// attributes
	//
	State		state;

	elle::Point	point;

	Label		label;
	elle::Gate*	gate;
      };

    }
  }
}

#endif
