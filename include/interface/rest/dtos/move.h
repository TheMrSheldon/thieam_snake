#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Move : public oatpp::DTO {
		DTO_INIT(Move, DTO)
		
		DTO_FIELD(String, move);
		DTO_FIELD(String, shout);
	};
}

#include OATPP_CODEGEN_END(DTO)