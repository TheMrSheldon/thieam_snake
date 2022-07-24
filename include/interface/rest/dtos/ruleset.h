#pragma once

#include "ruleset_settings.h"

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	/**
	 * @brief https://docs.battlesnake.com/references/api#ruleset
	 */
	class Ruleset : public oatpp::DTO {
		DTO_INIT(Ruleset, DTO)
		
		DTO_FIELD(String, name);
		DTO_FIELD(String, version);
		DTO_FIELD(Object<RulesetSettings>, settings);
	};
}

#include OATPP_CODEGEN_END(DTO)