#ifndef ELLE_SERIALIZATION_ERROR_HH
# define ELLE_SERIALIZATION_ERROR_HH

# include <elle/Error.hh>

namespace elle
{
  namespace serialization
  {
    class Error:
      public elle::Error
    {
    public:
      Error(std::string const& message);
    };
  }
}

#endif
