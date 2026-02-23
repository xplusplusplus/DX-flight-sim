#include "InputSystem.h"
#include <Windows.h>

InputSystem* InputSystem::m_input_system = nullptr;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	InputSystem::m_input_system = nullptr;
}

void InputSystem::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (m_first_time) 
	{
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
		m_first_time = false;
	}

	if (current_mouse_pos.x != m_old_mouse_pos.m_x || current_mouse_pos.y != m_old_mouse_pos.m_y) 
	{
		std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();

		while (it != m_set_listeners.end())
		{
			(*it)->onMouseMove(Point(current_mouse_pos.x,current_mouse_pos.y));
			++it;
		}
	}
	m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);

	if (::GetKeyboardState(m_keys_state)) 
	{
		for (unsigned int i = 0; i < 256; i++) 
		{
			if (m_keys_state[i] & 0x80)
			{
				std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();

				while (it != m_set_listeners.end())
				{
					if (i == VK_LBUTTON) {
						if (m_keys_state[i] != m_old_keys_state[i]) 
							(*it)->onLeftMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}else if (i == VK_RBUTTON) {
						if (m_keys_state[i] != m_old_keys_state[i]) 
							(*it)->onRightMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
					}else
						(*it)->onKeyDown(i);
					++it;
				}
			}
			else 
			{
				if (m_keys_state[i] != m_old_keys_state[i]) 
				{
					std::unordered_set<InputListener*, InputListener*>::iterator it = m_set_listeners.begin();

					while (it != m_set_listeners.end())
					{
						if (i == VK_LBUTTON)
							(*it)->onLeftMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						else if(i == VK_RBUTTON)
							(*it)->onRightMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
						else
							(*it)->onKeyUp(i);
						
						++it;
					}
				}
			}
		}
	}
	::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
}

void InputSystem::addListener(InputListener* listener)
{
	m_set_listeners.insert(listener);
	(std::forward<InputListener*>(listener), std::forward<InputListener*>(listener));
}

void InputSystem::removeListener(InputListener* listener)
{
	std::unordered_set<InputListener*, InputListener*>::iterator it = m_set_listeners.find(listener);

	if (it != m_set_listeners.end())
	{
		m_set_listeners.erase(it);
	}
}

void InputSystem::setCursorPosition(const Point& pos)
{
	::SetCursorPos(pos.m_x, pos.m_y);
}

void InputSystem::showCuror(bool show)
{
	::ShowCursor(show);
}

InputSystem* InputSystem::get()
{
	return m_input_system;
}

void InputSystem::create()
{
	if (InputSystem::m_input_system) throw std::exception("InputSystem already created");
	InputSystem::m_input_system = new InputSystem();
}

void InputSystem::release()
{
	if (!InputSystem::m_input_system) return;
	delete InputSystem::m_input_system;
}
