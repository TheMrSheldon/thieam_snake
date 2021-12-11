#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace rest::dto {
	class SnakeInfo : public oatpp::DTO {
		DTO_INIT(SnakeInfo, DTO)
		
		DTO_FIELD(String, apiversion);
		DTO_FIELD(String, author);
		DTO_FIELD(String, color);
		DTO_FIELD(String, head);
		DTO_FIELD(String, tail);
		DTO_FIELD(String, version);
	};
}

#include OATPP_CODEGEN_END(DTO)