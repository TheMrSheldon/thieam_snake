#pragma once

#include "position.h"

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Board : public oatpp::DTO {
		DTO_INIT(Board, DTO)
		
		DTO_FIELD(UInt32, height);
		DTO_FIELD(UInt32, width);
		DTO_FIELD(Vector<Object<Position>>, food);
		DTO_FIELD(Vector<Object<Position>>, hazards);
		DTO_FIELD(Vector<Object<Battlesnake>>, snakes);
	};
}

#include OATPP_CODEGEN_END(DTO)