#pragma once

#include "ruleset.h"
#include "../../../gameinfo.h"

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

		GameInfo createGameInfo() const noexcept {
			return GameInfo{
				.id = this->id,
				.timeout = this->timeout,
				.source = this->source,
				.rules = {
					.name = this->ruleset->name,
					.version = this->ruleset->version,
					.settings = {
						.foodSpawnChance = this->ruleset->settings->foodSpawnChance,
						.minimumFood = this->ruleset->settings->hazardDamagePerTurn,
						.hazardDamagePerTurn = this->ruleset->settings->hazardDamagePerTurn,
						.royale = {
							.shrinkEveryNTurns = this->ruleset->settings->shrinkEveryNTurns
						},
						.squad = {
							.allowBodyCollisions = this->ruleset->settings->allowBodyCollisions,
							.sharedElimination = this->ruleset->settings->sharedElimination,
							.sharedHealth = this->ruleset->settings->sharedHealth,
							.sharedLength = this->ruleset->settings->sharedLength
						}
					}
				}
			};
		}
	};
}

#include OATPP_CODEGEN_END(DTO)