#ifndef __ISCREEN_H

#define __ISCREEN_H

#include "stdstyle.h"
#include <stack>
#include <memory>

class iscreen
{
	public:
		virtual bool should_run() = 0;
		virtual void update_game() = 0;
		virtual void process_input() = 0;
		virtual void render() = 0;
};

class screen_manager
{
	public:
		bool should_run();
		void update_game();
		void process_input();
		void render();
		void push_screen(std::shared_ptr<iscreen> new_screen);
		void pop_screen();
	private:
		void pop_up();
		std::stack<std::shared_ptr<iscreen>> st;
};
#endif