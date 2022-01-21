#pragma once

#include "position.h"

#include <libsnake/state.h>

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

		ls::State createState(const std::string* id) const noexcept {
			//FIXME: load squads
			auto width = this->width;
			auto height = this->height;
			std::vector<ls::Snake> snakes;
			std::vector<ls::Position> food = FromDTO(this->food);
			for (auto& s : *(this->snakes)) {
				if (id != nullptr && *id == *(s->id))
					snakes.emplace_back(FromDTO(s->body), s->health, ls::SnakeFlags::ByIndex(snakes.size()));
			}
			for (auto& s : *(this->snakes)) {
				if (id == nullptr || *id != *(s->id))
					snakes.emplace_back(FromDTO(s->body), s->health, ls::SnakeFlags::ByIndex(snakes.size()));
			}
			return ls::State((unsigned)width.getValue(0), (unsigned)height.getValue(0), std::move(snakes), food);
		}

	private:
		static std::vector<ls::Position> FromDTO(const Vector<Object<Position>>& pos) {
			std::vector<ls::Position> ret;
			ret.reserve(pos->size());
			for (auto& p : *pos)
				ret.emplace_back(ls::Position(p->x, p->y));
			return std::move(ret);
		}
	};
}

#include OATPP_CODEGEN_END(DTO)