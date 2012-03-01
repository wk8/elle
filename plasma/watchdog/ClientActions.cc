//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 01 Mar 2012 12:55:56 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Client.hh"
#include "ClientActions.hh"
#include "Manager.hh"
#include "Connection.hh"

using namespace plasma::watchdog;


//
// ---------- helper macros ---------------------------------------------------
//

#define REGISTER(name, method)                                                \
  do {                                                                        \
    using namespace std::placeholders;                                        \
    this->_manager.RegisterCommand(                                           \
        name,                                                                 \
        std::bind(&ClientActions::method, this, _1, _2, _3)                   \
    )                                                                         \
  } while(false)                                                              \

#define UNREGISTER(name)                                                      \
  this->_manager.UnregisterCommand(name)                                      \

//
// ---------- contructors & descructors ---------------------------------------
//



ClientActions::ClientActions(Manager& manager) :
  _manager(manager)
{
  using namespace std::placeholders;
  REGISTER("RUN", _OnRun);
  REGISTER("STOP", _OnStop);
}

//
// ---------- methods  --------------------------------------------------------
//


void ClientActions::_OnRun(Connection& conn,
                           Client& client,
                           QVariantList const& args)
{
  UNREGISTER("RUN");
}

void ClientActions::_OnStop(Connection& conn,
                           Client& client,
                           QVariantList const& args)
{
  this->_manager.Stop();
}
