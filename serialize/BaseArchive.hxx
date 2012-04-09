#ifndef ELLE_SERIALIZE_BASEARCHIVE_HXX
# define ELLE_SERIALIZE_BASEARCHIVE_HXX

# include <boost/detail/endian.hpp>

# include "BaseArchive.hh"
# include "ArchiveSerializer.hxx"

namespace elle { namespace serialize {

    template<ArchiveMode mode_, typename Archive, typename CharType_>
    template<typename T, ArchiveMode _mode>
      struct BaseArchive<mode_, Archive, CharType_>::_EnableFor
      {
        template<typename _T> struct _IsNamedValue
          { static bool const value = false; };
        template<typename _T> struct _IsNamedValue<NamedValue<_T>>
          { static bool const value = true; };

        typedef typename std::enable_if<
                 (
                      std::is_same<
                          typename boost::call_traits<T>::param_type,
                          typename std::add_lvalue_reference<
                              typename std::add_const<T>::type
                          >::type
                      >::value
                  ||  (!std::is_pointer<T>::value && std::is_array<T>::value)
                )
            &&  mode == _mode
            &&  !_IsNamedValue<T>::value
          , Archive&
        > Ref;

        typedef typename std::enable_if<
                std::is_same<
                    typename boost::call_traits<T>::param_type,
                    typename std::add_const<T>::type
                >::value
            &&  mode == _mode
            &&  !_IsNamedValue<T>::value
            && !std::is_pointer<T>::value
          , Archive&
        > Val;

        typedef typename std::enable_if<
                (!std::is_pointer<T>::value || std::is_array<T>::value)
            &&  !std::is_const<T>::value
            &&  mode == _mode
          , Archive&
        > NotPointer;

        typedef typename std::enable_if<
            !std::is_pointer<T>::value && mode == _mode,
            std::unique_ptr<T, ConstructDeleter<T>>
        > ConstructPtr;

        typedef typename std::enable_if<
          _IsNamedValue<T>::value && mode == _mode,
          Archive&
        > NamedValue;
      };

    /// Generic save method. Uses an explicit specialization of ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CharType_>
    template<typename T>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(T const& val)
      {
        if (StoreClassVersion<T>::value == true)
            Access::Save(this->self(), ClassVersionType(ArchivableClass<T>::version));

        typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
        // this const_cast is safe since the archive is in output mode
        Serializer::Serialize(
            this->self(),
            const_cast<T&>(val),
            ArchivableClass<T>::version
        );
      }

    /// Generic load method. Uses an explicit specialization of ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CharType_>
    template<typename T>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(T& val)
      {
        ClassVersionType classVersion(0);
        if (StoreClassVersion<T>::value == true)
          Access::Load(this->self(), classVersion);

        typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
        Serializer::Serialize(this->self(), val, classVersion.version);
      }

    /// Generic load construct. Uses an explicit specialization of ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CharType_>
    template<typename T>
      inline void BaseArchive<mode_, Archive, CharType_>::LoadConstruct(T* ptr)
      {
        assert(ptr != nullptr);
        ClassVersionType classVersion(0);
        Access::Load(this->self(), classVersion);
        ArchiveSerializer<T>::LoadConstruct(this->self(), ptr, classVersion.version);
      }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(int16_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x00ff)      ),
              static_cast<char>((val & 0xff00) >> 8 ),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), tab, sizeof(tab));
# endif
        }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(int16_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int16_t>(tab[0])      ) +
              (static_cast<int16_t>(tab[1]) << 8 )
            ;
# endif
        }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(int32_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x000000ff)      ),
              static_cast<char>((val & 0x0000ff00) >> 8 ),
              static_cast<char>((val & 0x00ff0000) >> 16),
              static_cast<char>((val & 0xff000000) >> 24),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), &tab, sizeof(tab));
# endif
        }

      /// Load int32_t
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(int32_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int32_t>(tab[0])      ) +
              (static_cast<int32_t>(tab[1]) << 8 ) +
              (static_cast<int32_t>(tab[2]) << 16) +
              (static_cast<int32_t>(tab[3]) << 24)
            ;
# endif
        }

      /// Save int64_t
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(int64_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x00000000000000ff)      ),
              static_cast<char>((val & 0x000000000000ff00) >> 8 ),
              static_cast<char>((val & 0x0000000000ff0000) >> 16),
              static_cast<char>((val & 0x00000000ff000000) >> 24),
              static_cast<char>((val & 0x000000ff00000000) >> 32),
              static_cast<char>((val & 0x0000ff0000000000) >> 40),
              static_cast<char>((val & 0x00ff000000000000) >> 48),
              static_cast<char>((val & 0xff00000000000000) >> 56),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), tab, sizeof(tab));
# endif
        }

      /// Load int64_t
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(int64_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int64_t>(tab[0])      ) +
              (static_cast<int64_t>(tab[1]) << 8 ) +
              (static_cast<int64_t>(tab[2]) << 16) +
              (static_cast<int64_t>(tab[3]) << 24) +
              (static_cast<int64_t>(tab[4]) << 32) +
              (static_cast<int64_t>(tab[5]) << 40) +
              (static_cast<int64_t>(tab[6]) << 48) +
              (static_cast<int64_t>(tab[7]) << 56)
            ;
# endif
        }

      /// Save float
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(float val)
        {
          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

      /// Load float
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(float& val)
        {
          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

      /// Save double
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(double val)
        {
          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

      /// Load double
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(double& val)
        {
          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

      /// Save std::string
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(std::string const& val)
        {
          typedef typename Archive::StringSizeType SizeType;
          static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
          size_t sz = val.size();
          if (static_cast<size_t>(static_cast<SizeType>(-1)) < sz)
            throw std::runtime_error("String size too big");
          this->self().Save(static_cast<SizeType>(sz));
          Access::SaveBinary(this->self(), val.data(), sz);
        }

      /// Load std::string
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(std::string& val)
        {
          typedef typename Archive::StringSizeType SizeType;
          static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
          SizeType size;
          this->self().Load(size);
          val.resize(size);
          char tab[256];
          size_t idx = 0;
          while (size > 256)
            {
              Access::LoadBinary(this->self(), tab, 256);
              val.replace(idx, 256, tab, 256);
              size -= 256;
              idx += 256;
            }
          if (size > 0)
            {
              Access::LoadBinary(this->self(), tab, size);
              val.replace(idx, size, tab, size);
            }
        }

      /// Save class version
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(ClassVersionType const& classVersion)
      {
        Access::Save(this->self(), classVersion.version);
      }

      /// Load class version
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(ClassVersionType& classVersion)
      {
        Access::Load(this->self(), classVersion.version);
      }

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::SaveBinary(void const* data, std::streamsize size)
        {
          this->stream().write(static_cast<char const*>(data), size);
        }

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::LoadBinary(void* data, std::streamsize size)
        {
          this->stream().read(static_cast<char*>(data), size);
        }



    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(int8_t val)
        { Access::SaveBinary(this->self(), &val, sizeof(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(int8_t& val)
        { Access::LoadBinary(this->self(), &val, sizeof(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(char val)
        { Access::Save(this->self(), static_cast<int8_t>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(uint8_t val)
        { Access::Save(this->self(), static_cast<int8_t>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(uint16_t val)
        { Access::Save(this->self(), static_cast<int16_t>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(uint32_t val)
        { Access::Save(this->self(), static_cast<int32_t>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Save(uint64_t val)
        { Access::Save(this->self(), static_cast<int64_t>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(char& val)
        { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(uint8_t& val)
        { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(uint16_t& val)
        { Access::Load(this->self(), reinterpret_cast<int16_t&>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(uint32_t& val)
        { Access::Load(this->self(), reinterpret_cast<int32_t&>(val)); }

    template<ArchiveMode mode_, typename Archive, typename CharType_>
      inline void BaseArchive<mode_, Archive, CharType_>::Load(uint64_t& val)
        { Access::Load(this->self(), reinterpret_cast<int64_t&>(val)); }


    template<ArchiveMode mode_, typename Archive, typename CharType_>
      class BaseArchive<mode_, Archive, CharType_>::Access
      {
      public:
        static inline void SaveBinary(Archive& ar, void const* data, size_t size)
        {
          ar.SaveBinary(data, size);
        }
        static inline void LoadBinary(Archive& ar, void* data, size_t size)
        {
          ar.LoadBinary(data, size);
        }

        static inline void Save(Archive& ar, char val)      { ar.Save(val); }
        static inline void Save(Archive& ar, int8_t val)    { ar.Save(val); }
        static inline void Save(Archive& ar, uint8_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, int16_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, uint16_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, uint32_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, int32_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, uint64_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, int64_t val)   { ar.Save(val); }

        static inline void Load(Archive& ar, char& val)     { ar.Load(val); }
        static inline void Load(Archive& ar, int8_t& val)   { ar.Load(val); }
        static inline void Load(Archive& ar, uint8_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, int16_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint16_t& val) { ar.Load(val); }
        static inline void Load(Archive& ar, int32_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint32_t& val) { ar.Load(val); }
        static inline void Load(Archive& ar, int64_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint64_t& val) { ar.Load(val); }

        static inline void Save(Archive& ar, ClassVersionType const& classVersion)
          { ar.Save(classVersion); }
        static inline void Load(Archive& ar, ClassVersionType& classVersion)
          { ar.Load(classVersion); }


        template<typename T> static inline void Save(Archive& ar, T const& val)
          { ar.Save(val); }
        template<typename T> static inline void Load(Archive& ar, T& val)
          { ar.Load(val); }
        template<typename T> static inline void LoadConstruct(Archive& ar, T* ptr)
          { ar.LoadConstruct(ptr); }
        template<typename T>
          static inline void SaveNamed(Archive& ar,
                                       std::string const& name,
                                       T const& value)
          { ar.SaveNamed(name, value); }
        template<typename T>
          static inline void LoadNamed(Archive& ar, std::string const& name, T& value)
          { ar.LoadNamed(name, value); }
      };

}}

#endif
