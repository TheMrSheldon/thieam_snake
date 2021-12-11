#pragma once

#include "battlesnake.h"
#include "board.h"
#include "game.h"

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Gameinfo : public oatpp::DTO {
		DTO_INIT(Gameinfo, DTO)
		
		DTO_FIELD(Object<Game>, game);
		DTO_FIELD(Int32, turn);
		DTO_FIELD(Object<Board>, board);
		DTO_FIELD(Object<Battlesnake>, you);
	};
}

#include OATPP_CODEGEN_END(DTO)