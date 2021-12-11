#pragma once

#include "position.h"

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Battlesnake : public oatpp::DTO {
		DTO_INIT(Battlesnake, DTO)
		
		DTO_FIELD(String, id);
		DTO_FIELD(String, name);
		DTO_FIELD(Int32, health);
		DTO_FIELD(Vector<Object<Position>>, body);
		DTO_FIELD(String, latency);
		DTO_FIELD(Object<Position>, head);
		DTO_FIELD(Int32, length);
		DTO_FIELD(String, shout);
		DTO_FIELD(String, squad);
	};
}

#include OATPP_CODEGEN_END(DTO)