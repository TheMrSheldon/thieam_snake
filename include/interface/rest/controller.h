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
			agent.startGame(body->game->createGameInfo());
			return createResponse(Status::CODE_200, "OK");
		}

		ENDPOINT("POST", "/move", move, BODY_DTO(Object<rest::dto::Gameinfo>, body)) {
			auto state = body->board->createState(*(body->you->id.get()));
			auto move = agent.getAction(body->game->createGameInfo(), body->turn, state);
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
			agent.endGame(body->game->createGameInfo());
			return createResponse(Status::CODE_200, "OK");
		}
	};
}

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen