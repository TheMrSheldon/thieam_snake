#pragma once

#include "dtos/snake_info.h"
#include "dtos/move.h"
#include "dtos/gameinfo.h"
#include "gameinfo.h"
#include "agent.h"

#include <libsnake/definitions.h>
#include <libsnake/state.h>

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>

#include <vector>

#include <iostream>

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace rest {
	class Controller : public oatpp::web::server::api::ApiController {
	private:
		Agent& agent;
	public:
		/**
		 * @brief Construct a new Controller object
		 * 
		 * @param agent - the agent used to answer the queries.
		 * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
		 */
		Controller(Agent& agent, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) : oatpp::web::server::api::ApiController(objectMapper), agent(agent) {
			ASSERT(((oatpp::parser::json::mapping::ObjectMapper&)*objectMapper).getDeserializer()->getConfig()->allowUnknownFields, "We want to allow unknown json fields");
		}
	public:
		ENDPOINT("GET", "/", root) {
			auto dto = rest::dto::SnakeInfo::createShared();
			dto->apiversion = "1";
			dto->author = agent.author;
			dto->color = agent.color;
			dto->head = agent.head;
			dto->tail = agent.tail;
			dto->version = agent.version;
			return createDtoResponse(Status::CODE_200, dto);
		}
		
		ENDPOINT("POST", "/start", start, BODY_DTO(Object<rest::dto::Gameinfo>, body)) {
			agent.startGame(FromDTO(body->game));
			return createResponse(Status::CODE_200, "OK");
		}

		ENDPOINT("POST", "/move", move, BODY_DTO(Object<rest::dto::Gameinfo>, body)) {
			auto state = FromDTO2(body->board, body->you->id.get());
			auto move = agent.getAction(FromDTO(body->game), body->turn, state);
			auto dto = rest::dto::Move::createShared();
			if (move == ls::Move::down)
				dto->move = "down";
			else if (move == ls::Move::up)
				dto->move = "up";
			else if (move == ls::Move::left)
				dto->move = "left";
			else if (move == ls::Move::right)
				dto->move = "right";
			return createDtoResponse(Status::CODE_200, dto);
		}

		ENDPOINT("POST", "/end", end, BODY_DTO(Object<rest::dto::Gameinfo>, body)) {
			agent.endGame(FromDTO(body->game));
			return createResponse(Status::CODE_200, "OK");
		}

		static GameInfo FromDTO(const Object<rest::dto::Game>& info) {
			return GameInfo{
				.id = info->id,
				.timeout = info->timeout,
				.source = info->source,
				.rules = {
					.name = info->ruleset->name,
					.version = info->ruleset->version,
					.settings = {
						.foodSpawnChance = info->ruleset->settings->foodSpawnChance,
						.minimumFood = info->ruleset->settings->hazardDamagePerTurn,
						.hazardDamagePerTurn = info->ruleset->settings->hazardDamagePerTurn,
						.royale = {
							.shrinkEveryNTurns = info->ruleset->settings->shrinkEveryNTurns
						},
						.squad = {
							.allowBodyCollisions = info->ruleset->settings->allowBodyCollisions,
							.sharedElimination = info->ruleset->settings->sharedElimination,
							.sharedHealth = info->ruleset->settings->sharedHealth,
							.sharedLength = info->ruleset->settings->sharedLength
						}
					}
				}
			};
		}
		static ls::State FromDTO2(const Object<rest::dto::Board>& info, const std::string* id) {
			//FIXME: load squads
			auto width = info->width;
			auto height = info->height;
			std::vector<ls::Snake> snakes;
			std::vector<ls::Position> food = FromDTO3(info->food);
			for (auto& s : *(info->snakes)) {
				if (id != nullptr && *id == *(s->id))
					snakes.emplace_back(FromDTO3(s->body), s->health, ls::SnakeFlags::ByIndex(snakes.size()));
			}
			for (auto& s : *(info->snakes)) {
				if (id == nullptr || *id != *(s->id))
					snakes.emplace_back(FromDTO3(s->body), s->health, ls::SnakeFlags::ByIndex(snakes.size()));
			}
			return ls::State((unsigned)width.getValue(0), (unsigned)height.getValue(0), std::move(snakes), food);
		}
		static std::vector<ls::Position> FromDTO3(const oatpp::data::mapping::type::DTO::Vector<Object<rest::dto::Position>>& pos) {
			std::vector<ls::Position> ret;
			ret.reserve(pos->size());
			for (auto& p : *pos)
				ret.emplace_back(ls::Position(p->x, p->y));
			return std::move(ret);
		}
	};
}

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen