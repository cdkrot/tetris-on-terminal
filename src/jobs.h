// awesome pretty abstraction... which we don't use.
#ifndef __JOBS_H
#define __JOBS_H

#include <memory>
#include <vector>
using std::vector;
using std::shared_ptr;

class job_manager;

class job
{
	public:
		job() = default;
		~job() = default;
		virtual void invoke() = 0;
		
		// the value of this function couldn't change
		// while not running invoke() method.
		virtual clock_t next_invoke() = 0;
		
		// called by job_manager after adding this job
		virtual void init() = 0;
	private:
		job_manager* job_mgr;
		
		friend job_manager;
};

class job_manager
{
	public:
		job_manager() = default;
		~job_manager() = default;
		
		void add_job(shared_ptr<job> p_job);
		
		void invoke();
		clock_t next_invoke(); // the next invoke.
	private:
		vector<shared_ptr<job>> jobs; // heap.
		void sift_up(size_t ind);
		void sift_down(size_t ind);
};


#endif
