#pragma once
#include "Entity.h"

#include <map>
#include <unordered_map>
#include <vector>

namespace Sherphy 
{
	template<typename entity_type = id_type>
	class DataWareHouse 
	{
		template <typename _type>
		using storage_of_type = std::unordered_map<_type, std::map<_type, >>;
			//std::map<
		std::type_info type_ubfi = typeid(_type);

		private:
			storage_of_type<entity_type>
	};

}