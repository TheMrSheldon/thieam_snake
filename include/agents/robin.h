#include "../agent.h"

#include <libsnake/state.h>

namespace agents {
	class Robin final : public Agent {
	private:

	public:
		Robin() noexcept;

		void startGame(const GameInfo& info) override;
		Move getAction(const GameInfo& info, uint32_t turn, ls::State& state) override;
		void endGame(const GameInfo& info) override;
	};
}