#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class Position : public oatpp::DTO {
		DTO_INIT(Position, DTO)
		
		DTO_FIELD(Int32, x);
		DTO_FIELD(Int32, y);
	};
}

#include OATPP_CODEGEN_END(DTO)