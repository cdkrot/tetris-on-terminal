#include "jobs.h"

void job_manager::add_job(shared_ptr<job> p_job)
{
	jobs.push_back(p_job);
	p_job->init();
	sift_up(jobs.size() - 1);
}

clock_t job_manager::next_invoke()
{
	if (!jobs.empty())
		return jobs[0]->next_invoke();
	else
		return clock();
}

void job_manager::invoke()
{
	if (!jobs.empty())
		while (jobs[0]->next_invoke() <= clock())
		{
			jobs[0]->invoke();
			sift_down(0);
		}
}

void job_manager::sift_up(size_t ind)
{
	if (!jobs.empty())
		while (ind != 0 and jobs[(ind - 1) / 2]->next_invoke() > jobs[ind]->next_invoke())
		{
			std::swap(jobs[(ind - 1) / 2], jobs[ind]);
			ind = (ind - 1) / 2;
		}
}

void job_manager::sift_down(size_t ind)
{
	if (!jobs.empty())
		while (true)
		{
			size_t best_ind = ind;
			
			if (2 * ind + 1 < jobs.size() and jobs[2 * ind + 1]->next_invoke() < jobs[best_ind]->next_invoke())
				best_ind = 2 * ind + 1;
			if (2 * ind + 2 < jobs.size() and jobs[2 * ind + 2]->next_invoke() < jobs[best_ind]->next_invoke())
				best_ind = 2 * ind + 2;
			
			if (best_ind == ind)
				break;
			std::swap(jobs[ind], jobs[best_ind]);
			ind = best_ind;
		}
}

