#include <cstdlib>
#include <cassert>
#include <ctime>

#include <QDir>
#include <QFile>
#include <QLocalSocket>

#include <elle/log.hh>

#include <common/common.hh>

#include <plasma/common/resources.hh>

#include "LocalServer.hh"
#include "Application.hh"

using namespace plasma::watchdog;

Application::Application(int ac, char* av[]) :
  QCoreApplication(ac, av)
{
  this->_server = new LocalServer(*this);
}

Application::~Application()
{
  delete this->_server;
  this->_server = nullptr;
}

namespace
{

  int initRand() { ::srand(::time(nullptr)+42); return 42; }
  int dummy = initRand();

  /// Returns string of size `len` filled randomly with chars contained in
  /// the string `in'.
  std::string randString(std::string const& in, size_t len)
  {
    assert(in.size() > 0);
    assert(len > 0);
    (void) dummy;

    std::string res;
    res.resize(len);
    size_t in_size = in.size();
    for (size_t i = 0; i < len; ++i)
      res[i] = in[static_cast<size_t>(::rand()) % in_size];
    return res;
  }

  static std::string ASCII{
      "qwertyuiopasdfghjklzxcvbnnm0123456789QWERTYUIOPASDFGHJKJKLZXCVVBNM"
  };

} // !ns anonymous

int Application::exec()
{
  QDir homeDirectory(common::infinit::home().c_str());

  // Generate new watchdog id
  std::string watchdogId = randString(ASCII, 42);
  elle::log::debug("New watchdog id:", watchdogId);

  // Saving watchdog id
  {
    QFile f(homeDirectory.filePath("infinit.wtg"));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
      {
        elle::log::fatal("Cannot open file:", f.fileName().toStdString());
        return EXIT_FAILURE;
      }
    f.write(watchdogId.c_str());
    f.flush();
    f.close();
    f.setPermissions(
        QFile::ReadOwner | QFile::WriteOwner
    );
  }


  this->_server->start(watchdogId);
  return this->QCoreApplication::exec();
}

