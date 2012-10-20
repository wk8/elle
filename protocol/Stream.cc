#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/Stream.hh>

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    Stream::Stream(reactor::Scheduler& scheduler)
      : _scheduler(scheduler)
    {}

    Stream::~Stream()
    {}

    /*-----------.
    | Properties |
    `-----------*/

    reactor::Scheduler&
    Stream::scheduler()
    {
      return _scheduler;
    }

    /*--------.
    | Sending |
    `--------*/

    void
    Stream::write(Packet& packet)
    {
      packet.flush();
      this->_write(packet);
    }

    /*------------------.
    | Int serialization |
    `------------------*/

    void
    Stream::_uint32_put(std::ostream& s, uint32_t  i)
    {
      i = htonl(i);
      s.write(reinterpret_cast<char*>(&i), sizeof(i));
    }

    uint32_t
    Stream::_uint32_get(std::istream& s)
    {
      uint32_t res;
      s.read(reinterpret_cast<char*>(&res), sizeof(res));
      return ntohl(res);
    }
  }
}
