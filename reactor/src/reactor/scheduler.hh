#ifndef INFINIT_REACTOR_SCHEDULER_HH
# define INFINIT_REACTOR_SCHEDULER_HH

# include <memory>
# include <set>
# include <thread>
# include <unordered_map>

# include <boost/thread.hpp>

# include <elle/Printable.hh>

# include <reactor/asio.hh>
# include <reactor/duration.hh>
# include <reactor/fwd.hh>
# include <reactor/backend/thread.hh>

namespace reactor
{
  /** Scheduler
   *
   */
  class Scheduler:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Scheduler();

  /*------------------.
  | Current scheduler |
  `------------------*/
  public:
    static
    Scheduler*
    scheduler();

  /*----.
  | Run |
  `----*/
  public:
    void run();
    bool step();
  private:
    void _step(Thread* t);

  /*-------------------.
  | Threads management |
  `-------------------*/
  public:
    typedef std::set<Thread*> Threads;
    Thread* current() const;
    void terminate();
    void terminate_later();
  private:
    bool _shallstop;

  private:
    void _freeze(Thread& thread);
    void _thread_register(Thread& thread);
    void _unfreeze(Thread& thread);
  private:
    void _terminate(Thread* thread);
    void _terminate_now(Thread* thread);
    Thread* _current;
    Threads _starting;
    boost::mutex _starting_mtx;
    Threads _running;
    Threads _frozen;

    /*-------.
    | Status |
    `-------*/
    public:
      void dump(std::ostream&);
      void debug();

  /*-------------------------.
  | Thread Exception Handler |
  `-------------------------*/
  private:
    std::exception_ptr _eptr;

  /*----------------.
  | Multithread API |
  `----------------*/
  public:
    template <typename R>
    R
    mt_run(const std::string& name,
           const boost::function<R ()>& action);
  private:
    void
    _mt_run_void(const std::string& name,
                 const boost::function<void ()>& action);

  /*----------.
  | Printable |
  `----------*/
  public:
    void print(std::ostream& s) const;

  /*----------.
  | Shortcuts |
  `----------*/
  public:
    void CallLater(const boost::function<void ()>&      f,
                   const std::string&                   name,
                   Duration                             delay);
    /** Run an operation at a given frequency.
     *
     *  Run the \param op operation every \param freq. For now, the
     *  implementation simply sleeps for \param freq between every action thus
     *  inducing a drift. This is suitable for actions that need to be run
     *  in background roughly regularly such as garbage collecting,
     *  notification checking, ... It is not suitable if the frequency must be
     *  exactly met on the long term.
     *
     *  The thread that run the operation is returned, enabling the caller to
     *  stop it at will.
     *
     *  \param op   The operation to run.
     *  \param name The name of the thread that will run the operation.
     *  \param freq The frequency at which to run the operation.
     *  \return     The thread running the operation.
     */
    Thread*
    every(const boost::function<void ()>& op,
          const std::string& name,
          Duration freq);

    /*-----.
    | Asio |
    `-----*/
    public:
      boost::asio::io_service& io_service();
    private:
      boost::asio::io_service _io_service;
      boost::asio::io_service::work* _io_service_work;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class Thread;
      backend::Manager _manager;
  };
}

# include <reactor/scheduler.hxx>

#endif
