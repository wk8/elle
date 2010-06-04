//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Token.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [fri may 28 17:48:43 2010]
//

#ifndef ETOILE_KERNEL_TOKEN_HH
#define ETOILE_KERNEL_TOKEN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Agent;

  }

  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a token is a secret key enabling a user to access encrypted
    /// data. however, in order to allow only the proper user to
    /// use this key, it is encrypted with the user's public key.
    ///
    class Token:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Token		Null;

      //
      // constructors & destructors
      //
      Token();
      Token(const Token&);
      ~Token();

      //
      // methods
      //
      elle::Status	Update(const elle::SecretKey&,
			       const elle::PublicKey&);
      elle::Status	Extract(const user::Agent&,
				elle::SecretKey&) const;

      //
      // interfaces
      //

      // object
      declare(Token);
      elle::Boolean	operator==(const Token&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      elle::Code*	code;
    };

  }
}

#endif
