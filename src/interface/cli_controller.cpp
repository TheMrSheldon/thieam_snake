#include "interface/cli_controller.h"

#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>

CLIController::CLIController() noexcept : rootMenu(std::make_unique<cli::Menu>("thieam")) {
	cli::SetColor();
    rootMenu->Insert(
		"color",
		[](std::ostream& out){ out << "Colors ON\n"; cli::SetColor(); },
		"Enable colors in the cli");
    rootMenu->Insert(
		"nocolor",
		[](std::ostream& out){ out << "Colors OFF\n"; cli::SetNoColor(); },
		"Disable colors in the cli");

    auto diagMenu = std::make_unique<cli::Menu>("diagnostic");
    diagMenu->Insert(
		"start-explaining",
		[](std::ostream& out){
			out << rang::fg::red << "Not yet implemented\n" << rang::fg::reset; //TODO: implement
		},
		"Start explaining the chosen move");
    diagMenu->Insert(
		"stop-explaining",
		[](std::ostream& out){
			out << rang::fg::red << "Not yet implemented\n" << rang::fg::reset; //TODO: implement
		},
		"Stop explaining the chosen move");
	
    rootMenu->Insert(std::move(diagMenu));
}

void CLIController::Run() noexcept {
	cli::Cli cli(std::move(rootMenu));
	cli::LoopScheduler scheduler;
	cli.ExitAction( [&scheduler](auto& out){ scheduler.Stop(); } );
	cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout);
	scheduler.Run();
}