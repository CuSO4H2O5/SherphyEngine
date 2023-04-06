#pragma once 
#include "Object.h"

namespace Sherphy 
{
	namespace Functions {
		void updatePosition(Entity ent, Vec3 movement) 
		{
			
		}
	}
	class System 
	{
	public:
		virtual void update() 
		{
			for (func_ptr _ptr : m_function_list) 
			{
				_ptr();
			}
		}
		virtual void registerFunc(func_ptr func_pointer)
		{
			m_function_list.push_back(func_pointer);
			return;
		}
		virtual void clear() 
		{
			m_function_list.clear();
		}
	private:
		std::vector<func_ptr> m_function_list;
	};

	class SystemUtils
	{
		static void update(System* sys)
		{
			sys->update();
		}
	};
}