//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Object.cc
//
// created       julien quintard   [fri mar  6 11:37:13 2009]
// updated       julien quintard   [sat may 21 23:49:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Object.hh>

#include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Object::Object():
      proton::ImprintBlock(ComponentObject)
    {
      //
      // the attributes below are initialized in the constructor body
      // because they belong to a sub-structure.
      //
      this->meta.state = StateClean;
      this->meta.owner.permissions = PermissionNone;
      this->meta.genre = GenreUnknown;
      this->meta.version = 0;

      this->data.state = StateClean;
      this->data.size = 0;
      this->data.version = 0;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the object given the owner public and the
    /// genre of the object to create.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// (ii) sets the meta data, and finally (iv) /// initializes the data
    /// part by setting the owner as the author.
    ///
    elle::Status	Object::Create(const Genre		genre,
				       const elle::PublicKey&	owner)
    {
      enter();

      // (i)
      {
	// create the underlying owner key block.
	if (proton::ImprintBlock::Create(owner) == elle::StatusError)
	  escape("unable to create the underlying owner key block");
      }

      // (ii)
      {
	// set the meta genre.
	this->meta.genre = genre;

	// set the initial owner permissions to all with an empty key.
	if (this->Administrate(this->meta.attributes,
			       proton::Address::Null,
			       PermissionReadWrite,
			       this->meta.owner.token) == elle::StatusError)
	  escape("unable to set the initial meta data");
      }

      // (iii)
      {
	elle::Digest	fingerprint;
	Author		author;

	// set an owner author.
	if (author.Create() == elle::StatusError)
	  escape("unable to create an author");

	// set the initial data with no contents, an empty fingerprint and
	// the owner as the author.
	if (this->Update(author,
			 proton::Address::Null,
			 0,
			 fingerprint) == elle::StatusError)
	  escape("unable to set the initial data");
      }

      leave();
    }

    ///
    /// this method updates the data section.
    ///
    elle::Status	Object::Update(const Author&		author,
				       const proton::Address&	contents,
				       const Size&		size,
				       const elle::Digest&	fingerprint)
    {
      enter();

      // set the last update time.
      if (this->data.stamp.Current() == elle::StatusError)
	escape("unable to set the last update time");

      // set the author.
      this->author = author;

      // set the address.
      this->data.contents = contents;

      // set the size.
      this->data.size = size;

      // set the fingerprint.
      this->data.fingerprint = fingerprint;

      // mark the section as dirty.
      this->data.state = StateDirty;

      leave();
    }

    ///
    /// this method updates the meta section.
    ///
    elle::Status	Object::Administrate(const Attributes&	attributes,
					     const proton::Address& access,
					     const Permissions&	permissions,
					     const Token&	token)
    {
      enter();

      // check if the access block address of the owner's token have changed.
      if ((this->meta.access != access) ||
	  (this->meta.owner.token != token))
	{
	  //
	  // in this case, the data section will have to be re-signed.
	  //
	  // indeed, the access address and owner token are actually
	  // included in the data section.
	  //
	  // for more information, please refer to the research publications.
	  //

	  // set the address.
	  this->meta.access = access;

	  // set the owner token.
	  this->meta.owner.token = token;

	  // set the data section as dirty.
	  this->data.state = StateDirty;
	}

      //
      // in any case, since the object has been administered, the management
      // time is going to be re-calculated, hence the meta section will
      // change, therefore it will need to be re-signed.
      //
      // the following simply assign the meta attributes and set the
      // meta section as dirty.
      //

      // set the last management time.
      if (this->meta.stamp.Current() == elle::StatusError)
	escape("unable to set the last management time");

      // set the attributes.
      this->meta.attributes = attributes;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // mark the section as dirty.
      this->meta.state = StateDirty;

      // re-compute the owner's access record. just like this->owner.subject,
      // this attribute is not mandatory but has been introduced in order
      // to simplify access control management.
      if (this->meta.owner.record.Update(this->owner.subject,
					 this->meta.owner.permissions,
					 this->meta.owner.token) ==
	  elle::StatusError)
	escape("unable to create the owner access record");

      leave();
    }

    ///
    /// this method verifies that the object has been properly created
    /// i.e that every signature has been produced by legitimate users.
    ///
    /// the method (i) calls the parent class for validation (iii) verifies
    /// the meta part's signature (iv) retrieves the author's public key
    /// (v) verifies the data signature and (vi) verify that the mutable
    /// block's general version number matches the object's versions.
    ///
    elle::Status	Object::Validate(const proton::Address&	address,
					 const Access*		access)
      const
    {
      const elle::PublicKey*	author;

      enter();

      // (i)
      {
	// call the parent class.
	if (proton::ImprintBlock::Validate(address) == elle::StatusFalse)
	  flee("unable to verify the underlying physical block");
      }

      // (ii)
      {
	if (this->meta.access != proton::Address::Null)
	  {
	    elle::Digest	fingerprint;

	    // test if there is an access block.
	    if (access == NULL)
	      escape("the Validate() method must take the object's "
		     "access block");

	    // compute the fingerprint of the access (subject, permissions)
	    // tuples.
	    if (access->Fingerprint(fingerprint) == elle::StatusError)
	      escape("unable to compute the access block fingerprint");

	    // verify the meta part, including the access fingerprint.
	    if (this->owner.K.Verify(this->meta.signature,

				     this->meta.owner.permissions,
				     this->meta.genre,
				     this->meta.stamp,
				     this->meta.attributes,
				     this->meta.version,

				     fingerprint) == elle::StatusError)
	      flee("unable to verify the meta's signature");
	  }
	else
	  {
	    // verify the meta part.
	    if (this->owner.K.Verify(this->meta.signature,

				     this->meta.owner.permissions,
				     this->meta.genre,
				     this->meta.stamp,
				     this->meta.attributes,
				     this->meta.version) == elle::StatusError)
	      flee("unable to verify the meta's signature");
	  }
      }

      // (iii)
      {
	switch (this->author.role)
	  {
	  case RoleOwner:
	    {
	      author = &this->owner.K;

	      break;
	    }
	  default:
	    {
	      // XXX
	      printf("UNIMPLEMENTED\n");	
	    }
	  }
      }

      // (iv)
      {
	// verify the signature.
	if (author->Verify(this->data.signature,

			   this->data.contents,
			   this->data.size,
			   this->data.stamp,
			   this->data.fingerprint,
			   this->data.version,

			   this->meta.owner.token,
			   this->meta.access) != elle::StatusTrue)
	  flee("unable to verify the data signature");
      }

      // (v)
      {
	// check the mutable block's general version.
	if (this->version != (this->data.version + this->meta.version))
	  flee("invalid version number");
      }

      true();
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Boolean	Object::operator<(const Block&		block) const
    {
      const Object*	object =
	dynamic_cast<const Object*>(&block);

      enter();

      // check the cast.
      if (object == NULL)
	flee("unable to cast the block into an Object");

      // check the versions.
      if ((object->meta.version >= this->meta.version) ||
	  (object->data.version >= this->data.version))
	false();

      // XXX check the data/meta base.

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Object, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object.
    ///
    elle::Status	Object::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent class.
      if (proton::ImprintBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying owner key block");

      // dump the author part.
      if (this->author.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the author");

      // dump the meta part.
      std::cout << alignment << elle::Dumpable::Shift << "[Meta]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Owner] " << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< elle::Dumpable::Shift << "[Permissions] "
		<< (elle::Natural32)this->meta.owner.permissions << std::endl;

      if (this->meta.owner.token.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta owner's token");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Genre] " << (elle::Natural32)this->meta.genre
		<< std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Stamp] " << std::endl;
      if (this->meta.stamp.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta stamp");

      if (this->meta.attributes.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the meta attributess");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Access]" << std::endl;
      if (this->meta.access.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta access address");

      if (this->meta.version.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the meta version");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;
      if (this->meta.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the meta signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[State] " << this->meta.state << std::endl;

      // dump the data part.
      std::cout << alignment << elle::Dumpable::Shift << "[Data]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Contents]" << std::endl;
      if (this->data.contents.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the contents' address");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Size] " << this->data.size << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Stamp]" << std::endl;
      if (this->data.stamp.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the data stamp");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Fingerprint]" << std::endl;
      if (this->data.fingerprint.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the fingerprint");

      if (this->data.version.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the data version");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;
      if (this->data.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the data signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[State] " << this->data.state << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status	Object::Serialize(elle::Archive&	archive) const
    {
      enter();

      // call the parent class.
      if (proton::ImprintBlock::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying physical block");

      // serialize the author part.
      if (archive.Serialize(this->author) == elle::StatusError)
	escape("unable to serialize the author");

      // serialize the meta part.
      if (archive.Serialize(this->meta.owner.permissions,
			    this->meta.owner.token,
			    this->meta.genre,
			    this->meta.stamp,
			    this->meta.attributes,
			    this->meta.access,
			    this->meta.version,
			    this->meta.signature) == elle::StatusError)
	escape("unable to serialize the meta part");

      // serialize the data part.
      if (archive.Serialize(this->data.contents,
			    this->data.size,
			    this->data.stamp,
			    this->data.fingerprint,
			    this->data.version,
			    this->data.signature) == elle::StatusError)
	escape("unable to serialize the data part");

      leave();
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status	Object::Extract(elle::Archive&		archive)
    {
      enter();

      // call the parent class.
      if (proton::ImprintBlock::Extract(archive) == elle::StatusError)
	escape("unable to extract the underyling physical block");

      // compare the component.
      if (this->component != ComponentObject)
	escape("the archive does not seem to contain an object");

      // extract the author part.
      if (archive.Extract(this->author) == elle::StatusError)
	escape("unable to extract the author");

      // extract the meta part.
      if (archive.Extract(this->meta.owner.permissions,
			  this->meta.owner.token,
			  this->meta.genre,
			  this->meta.stamp,
			  this->meta.attributes,
			  this->meta.access,
			  this->meta.version,
			  this->meta.signature) == elle::StatusError)
	escape("unable to extract the meta part");

      // extract the data part.
      if (archive.Extract(this->data.contents,
			  this->data.size,
			  this->data.stamp,
			  this->data.fingerprint,
			  this->data.version,
			  this->data.signature) == elle::StatusError)
	escape("unable to extract the data part");

      // compute the owner record.
      if (this->meta.owner.record.Update(this->owner.subject,
					 this->meta.owner.permissions,
					 this->meta.owner.token) ==
	  elle::StatusError)
	escape("unable to create the owner access record");

      leave();
    }

  }
}
