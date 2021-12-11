#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class RulesetSettings : public oatpp::DTO {
		DTO_INIT(RulesetSettings, DTO)
		
		DTO_FIELD(Int32, foodSpawnChance);
		DTO_FIELD(Int32, minimumFood);
		DTO_FIELD(Int32, hazardDamagePerTurn);
		DTO_FIELD(Int32, shrinkEveryNTurns);//royale only
		DTO_FIELD(Boolean, allowBodyCollisions);//squad only
		DTO_FIELD(Boolean, sharedElimination);//squad only
		DTO_FIELD(Boolean, sharedHealth);//squad only
		DTO_FIELD(Boolean, sharedLength);//squad only
	};
}

#include OATPP_CODEGEN_END(DTO)