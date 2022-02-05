#pragma once

#include "position.h"

#include <libsnake/state.h>
#include <libsnake/snake_flags.h>

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include <map>
#include <string>
#include <vector>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Board : public oatpp::DTO {
		DTO_INIT(Board, DTO)
		
		DTO_FIELD(UInt32, height);
		DTO_FIELD(UInt32, width);
		DTO_FIELD(Vector<Object<Position>>, food);
		DTO_FIELD(Vector<Object<Position>>, hazards);
		DTO_FIELD(Vector<Object<Battlesnake>>, snakes);

		ls::State createState(const std::string& localPlayer) const noexcept {
			const std::vector<ls::Position> food = FromDTO(this->food);
			const std::vector<ls::Position> hazards = FromDTO(this->hazards);
			const SquadInfo squads(snakes, localPlayer);
			auto snakeData = std::move(squads.createSnakeVector(snakes));
			return ls::State(*width, *height, std::move(snakeData), food, hazards);
		}

	private:
		struct SquadInfo {
		private:
			std::string localSquadID;
			size_t localPlayerIndex;
			std::map<std::string, ls::SnakeFlags> squads;
			ls::SnakeFlags solos;

		public:
			SquadInfo(const Vector<Object<Battlesnake>>& snakes, const std::string& localPlayer) {
				for (size_t i = 0; i < snakes->size(); ++i) {
					const auto& snake = (*snakes)[i];
					const auto flag = ls::SnakeFlags::ByIndex(i);
					if (localPlayer == *(snake->id)) {
						localPlayerIndex = i;
						if (snake->squad && *(snake->squad) != "") {
							localSquadID = *(snake->squad);
							squads[localSquadID] |= flag;
						} else
							solos |= flag;
					} else {
						if (snake->squad && *(snake->squad) != "")
							squads[*(snake->squad)] |= flag;
						else //This is a dirty fix to temporarily disable Max^n search for more-player matches (TODO: remove)
							//solos |= flag;
							squads[""] |= flag;
					}
				}
			}

			std::vector<ls::Snake> createSnakeVector(const Vector<Object<Battlesnake>>& snakes) const {
				std::vector<ls::Snake> ret;
				ret.reserve(snakes->size());
				//Add the snakes grouped by squad starting with the local player's snake
				const bool localSolo = solos.containsAny(ls::SnakeFlags::ByIndex(localPlayerIndex));
				if (localSolo) {//Add the local player as solo player
					const auto& snake = (*snakes)[localPlayerIndex];
					ret.emplace_back(FromDTO(snake->body), snake->health, ls::SnakeFlags::ByIndex(0));
				} else {//Add the local player's squad starting with the localplayer
					const auto& squad = squads.find(localSquadID)->second;
					const auto flags = ls::SnakeFlags::FromTo(0, squad.size());
					{
						const auto& snake = (*snakes)[localPlayerIndex];
						ret.emplace_back(FromDTO(snake->body), snake->health, flags);
					}
					for (size_t i = 0; i < snakes->size(); i++) {
						if (i != localPlayerIndex && squad.containsAny(ls::SnakeFlags::ByIndex(i))) {
							const auto& snake = (*snakes)[i];
							ret.emplace_back(FromDTO(snake->body), snake->health, flags);
						}
					}
				}
				//Add the soloing players
				for (size_t i = 0; i < snakes->size(); i++) {
					if (i != localPlayerIndex && solos.containsAny(ls::SnakeFlags::ByIndex(i))) {
						const auto flags = ls::SnakeFlags::ByIndex(ret.size());
						const auto& snake = (*snakes)[i];
						ret.emplace_back(FromDTO(snake->body), snake->health, flags);
					}
				}
				//Add the remaining squads
				for (const auto& entry : squads) {
					if (localSolo || entry.first != localSquadID) {
						const auto& squad = entry.second;
						const auto flags = ls::SnakeFlags::FromTo(ret.size(), ret.size()+squad.size());
						for (size_t i = 0; i < snakes->size(); i++) {
							if (i != localPlayerIndex && squad.containsAny(ls::SnakeFlags::ByIndex(i))) {
								const auto& snake = (*snakes)[i];
								ret.emplace_back(FromDTO(snake->body), snake->health, flags);
							}
						}
					}
				}
				ASSERT(ret.size() == snakes->size(), "No snakes shall be lost");
				return std::move(ret);
			}
		};

		static std::vector<ls::Position> FromDTO(const Vector<Object<Position>>& pos) {
			std::vector<ls::Position> ret;
			if (pos != nullptr) {
				ret.reserve(pos->size());
				for (auto& p : *pos)
					ret.emplace_back(ls::Position(p->x, p->y));
			}
			return std::move(ret);
		}
	};
}

#include OATPP_CODEGEN_END(DTO)