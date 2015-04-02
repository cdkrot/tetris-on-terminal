#include "./stdstyle.h"
#include "./Iscreen.h"


void screen_manager::pop_up()
{
	while (!st.empty() and st.top()->should_run() == false)
		st.pop();
}

bool screen_manager::should_run()
{
	pop_up();
	return !st.empty();
}

void screen_manager::update_game()
{
	pop_up();
	if (!st.empty())
		st.top()->update_game();
}

void screen_manager::process_input()
{
	pop_up();
	if (!st.empty())
		st.top()->process_input();
}

void screen_manager::render()
{
	pop_up();
	if (!st.empty())
		st.top()->render();
}

void screen_manager::push_screen(std::shared_ptr<iscreen> new_screen)
{
	st.push(new_screen);
	pop_up();
}

void screen_manager::pop_screen()
{
	if (!st.empty())
		st.pop();
	pop_up();
}