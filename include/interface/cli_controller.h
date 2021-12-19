#pragma once

#include <cli/cli.h>

class CLIController final {
private:
	std::unique_ptr<cli::Menu> rootMenu;
public:
	CLIController() noexcept;
	void Run() noexcept;
};