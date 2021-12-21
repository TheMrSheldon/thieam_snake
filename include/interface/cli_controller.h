#pragma once

#include <cli/cli.h>
#include <cli/clilocalsession.h>

class CLIController final {
private:
	std::unique_ptr<cli::Menu> rootMenu;
	cli::CliLocalTerminalSession* session;
public:
	CLIController() noexcept;
	void Run() noexcept;
};