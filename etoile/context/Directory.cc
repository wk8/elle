//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Directory.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [mon aug 24 14:22:16 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Directory.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      catalog(NULL)
    {
    }

    ///
    /// the copy constructor.
    ///
    Directory::Directory(const Directory&			context)
    {
      // XXX \todo XXX
      printf("NOT IMPLEMENTED YET\n");
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
    {
      // release the catalog.
      if (this->catalog != NULL)
	delete this->catalog;
    }

  }
}
