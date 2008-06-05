/**
 ** \file scheduler/scheduler.hh
 ** \brief Definition of scheduler::Scheduler.
 */

#ifndef SCHEDULER_SCHEDULER_HH
# define SCHEDULER_SCHEDULER_HH

# include <iosfwd>
# include <vector>

# include <boost/any.hpp>
# include <boost/function.hpp>
# include <boost/utility.hpp>
# include <libport/utime.hh>

# include <scheduler/coroutine.hh>
# include <scheduler/fwd.hh>

namespace scheduler
{

  enum {
    SCHED_EXIT      = -1,
    SCHED_IMMEDIATE = 0
  };

  class Scheduler : boost::noncopyable
  {
  public:
    /// Constructor.
    ///
    /// \param get_time A function which, when called, returns the
    ///        current system time.
    Scheduler(boost::function0<libport::utime_t> get_time);

    /// Destructor.
    ~Scheduler();

  public:
    /// Do one cycle of work, and return the next time we expect to be called.
    ///
    /// \return If we have work to do, 0 will be returned
    /// (\p SCHED_IMMEDIATE) in order to be called again as soon as
    /// possible. If we only have time-suspended or dependent jobs, we
    /// will return the time of the next scheduled one. If \p killall_jobs()
    /// has been called and all jobs are dead, \p SCHED_EXIT will be returned.
    ///
    /// Calling work() again before the returned time is useless as there will
    /// be nothing to do except if some new work has been entered in.
    libport::utime_t work();

    /// Add a job to the list of jobs to be run later.
    ///
    /// \param job Job to be started. Please do not use this function
    ///        directly except from the \c Job::start_job() method.
    ///
    /// Jobs added during a cycle will be started at the next cycle by the
    /// scheduler.
    void add_job(rJob job);

    /// Terminate all jobs. This must be called only when the executable
    /// is going to terminate.
    void killall_jobs();

    /// Resume scheduler execution.
    ///
    /// \param job Job currently being executed. This job will relinquish
    ///        the CPU to the scheduler. Note that the scheduler is free
    ///        to reschedule \a job immediately if it wishes to do so.
    void resume_scheduler(rJob job);

    /// Get the currently executing job.
    ///
    /// \return A reference onto the currently executing job.
    ///
    /// It is an error to call this method if no job is currently
    /// executing or to keep the reference after yielding since the
    /// Job may no longer be valid then.
    Job& current_job() const;

    /// Check whether a job is the current job.
    ///
    /// \param job Job to be checked against the current job.
    ///
    /// \return \a true if \a job is the current job.
    bool is_current_job(const Job& job) const;

    /// Signal that a \c stop or a \c block has been issued on a tag.
    ///
    /// \param tag The tag that has been recently stopped or blocked.
    /// \param payload The data to embed in the StopException.
    ///
    /// After this function has been called, the scheduler will determine,
    /// at the end of the current cycle, which jobs need to react to this
    /// action.
    void signal_stop(const rTag& tag, boost::any payload);

    /// Get the current cycle number.
    ///
    /// \return The current cycle index, increasing by 1 at each cycle.
    unsigned int cycle_get() const;

    /// Get the time as seen by the scheduler.
    ///
    /// \return The current system time.
    libport::utime_t get_time() const;

    /// Get the current jobs list.
    ///
    /// \return The currently non-terminated known jobs.
    std::vector<rJob> jobs_get() const;

    /// Tell the scheduler that the world has changed because
    /// of an external event.
    ///
    void signal_world_change();

  private:
    /// Execute one round in the scheduler.
    ///
    /// \return See work().
    libport::utime_t execute_round();

    /// Function to retrieve the current system time.
    boost::function0<libport::utime_t> get_time_;

    /// List of jobs we are in charge of. During a cycle execution,
    /// this is where jobs will accumulate themselves after they have
    /// been executed.
    std::vector<rJob> jobs_;

    /// List of jobs currently being scheduled during the current round.
    std::vector<rJob> pending_;

    /// Current job.
    rJob current_job_;

    /// Coroutine corresponding to the scheduler.
    Coro* coro_;

    /// Has there been a possible side-effect since last time we reset
    /// this field?
    bool possible_side_effect_;

    /// Has a new job been added to the list of jobs to start in the current
    /// cycle?
    bool jobs_to_start_;

    /// Cycles counter.
    unsigned int cycle_;

    /// Ready to die when all jobs are also dead.
    bool ready_to_die_;
  };

} // namespace scheduler

# include <scheduler/scheduler.hxx>

#endif // !SCHEDULER_SCHEDULER_HH
