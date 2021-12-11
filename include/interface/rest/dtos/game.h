#pragma once

#include "ruleset.h"

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Game : public oatpp::DTO {
		DTO_INIT(Game, DTO)
		
		DTO_FIELD(String, id);
		DTO_FIELD(Object<Ruleset>, ruleset);
		DTO_FIELD(UInt32, timeout);
		DTO_FIELD(String, source);
	};
}

#include OATPP_CODEGEN_END(DTO)