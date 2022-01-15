#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class RulesetSettings : public oatpp::DTO {
		DTO_INIT(RulesetSettings, DTO)
		
		DTO_FIELD(Int32, foodSpawnChance) = 25;
		DTO_FIELD(Int32, minimumFood) = 1;
		DTO_FIELD(Int32, hazardDamagePerTurn) = 14;
		DTO_FIELD(Int32, shrinkEveryNTurns, "royale.shrinkEveryNTurns") = 5;//royale only
		DTO_FIELD(Boolean, allowBodyCollisions, "squad.allowBodyCollisions") = true;//squad only
		DTO_FIELD(Boolean, sharedElimination, "squad.sharedElimination") = true;//squad only
		DTO_FIELD(Boolean, sharedHealth, "squad.sharedHealth") = true;//squad only
		DTO_FIELD(Boolean, sharedLength, "squad.sharedLength") = true;//squad only
	};
}

#include OATPP_CODEGEN_END(DTO)