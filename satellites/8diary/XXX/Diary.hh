//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [sun jun 26 22:32:34 2011]
//

#ifndef PIG_DIARY_DIARY_HH
#define PIG_DIARY_DIARY_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION		26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <pig/diary/Upcall.hh>

#include <elle/idiom/Close.hh>
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace pig
{
  ///
  /// XXX
  ///
  namespace diary
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Diary:
      public elle::Object,
      public virtual elle::Fileable<elle::FormatRaw>
    {
    public:
      //
      // enumerations
      //
      enum Mode
	{
	  ModeUnknown,
	  ModeRecord,
	  ModeReplay
	};

      //
      // constructors & destructors
      //
      Diary();
      ~Diary();

      //
      // methods
      //
      elle::Status	Setup(const ::fuse_operations&);

      elle::Status	Record(elle::String&);
      elle::Status	Replay(const elle::Natural32,
			       const elle::Natural32);

      elle::Status	Write(const Upcall&);
      elle::Status	Read(Upcall&);

      elle::Status	End() const;

      //
      // interfaces
      //

      // object
      declare(Diary);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // fileable
      elle::Status	Load(const elle::Path&);
      elle::Status	Store(const elle::Path&) const;

      //
      // attributes
      //
      Mode		mode;

      ::fuse_operations	fuse;

      elle::Archive	archive;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Live.hh>
#include <pig/diary/Record.hh>
#include <pig/diary/Replay.hh>
#include <pig/diary/Upcall.hh>

#endif
