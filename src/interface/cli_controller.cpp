#include "interface/cli_controller.h"

#include "ai/eval.h"

#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>

#include <libsnake/position.h>
#include <libsnake/util.h>
#include <libsnake/state.h>
#include <functional>
#include <vector>

static std::pair<cli::detail::KeyType, char> GetKey() {
	int c = _getch();
	switch (c) {
		case EOF:
		case 4:  // EOT ie CTRL-D
		case 26: // CTRL-Z
		case 3:  // CTRL-C
			return std::make_pair(cli::detail::KeyType::eof, ' ');
			break;
		case 0: //Arrow keys
		case 224: {// symbol
			c = _getch();
			switch (c) {
				case 72: return std::make_pair(cli::detail::KeyType::up, ' ');
				case 80: return std::make_pair(cli::detail::KeyType::down, ' ');
				case 75: return std::make_pair(cli::detail::KeyType::left, ' ');
				case 77: return std::make_pair(cli::detail::KeyType::right, ' ');
				case 71: return std::make_pair(cli::detail::KeyType::home, ' ');
				case 79: return std::make_pair(cli::detail::KeyType::end, ' ');
				case 83: return std::make_pair(cli::detail::KeyType::canc, ' ');
				default: return std::make_pair(cli::detail::KeyType::ignored, ' ');
			}
		}
		case 8:
			return std::make_pair(cli::detail::KeyType::backspace, c);
			break;
		case 13:
			return std::make_pair(cli::detail::KeyType::ret, c);
			break;
		default: {// hopefully ascii
			const char ch = static_cast<char>(c);
			return std::make_pair(cli::detail::KeyType::ascii, ch);
		}
	}
	return std::make_pair(cli::detail::KeyType::ignored, ' ');
}

static void _PrintBoard(std::ostream& out, unsigned width, unsigned height, ls::Position selected, ls::Foods& food, std::vector<std::vector<ls::Position>>& snakes) {
	ls::State::drawBoard(out, width, height, [&out, selected, food, snakes](const ls::Position& pos){
		if (selected == pos)
			out << rang::fg::blue;
		if (food.get(pos.x, pos.y))
			out << 'o';
		else {
			unsigned snake = 0;
			for (snake = 0; snake < snakes.size(); snake++)
				if (utl::contains(snakes[snake], pos))
					break;
			if (snake == snakes.size())
				out << '.';
			else
				out << (snake+1);
		}
		out << rang::style::reset;
		return "";
	});
}

static void _PlaceFood(std::ostream& out, unsigned width, unsigned height, ls::Foods& food, std::vector<std::vector<ls::Position>>& snakes) {
	ls::Position selected(0,0);
	while (true) {
		std::cout << rang::style::bold << '['<<(char)0x10<<(char)0x11<<(char)0x1E<<(char)0x1F<<"] to move the selection; [Space] to (un)place food; [ESC] to exit placement" << rang::style::reset << std::endl;
		_PrintBoard(out, width, height, selected, food, snakes);
		auto in = GetKey();
		if (in.first == cli::detail::KeyType::eof) {
			for (unsigned y = 0; y < height+2; y++)
				out << "\u001B[A\r" << std::string(width*2+3, ' ');
			out << "\u001B[A";
			return;
		} else if (in.first == cli::detail::KeyType::up) {
			selected = selected.after_move(ls::Move::up);
		} else if (in.first == cli::detail::KeyType::down) {
			selected = selected.after_move(ls::Move::down);
		} else if (in.first == cli::detail::KeyType::left) {
			selected = selected.after_move(ls::Move::left);
		} else if (in.first == cli::detail::KeyType::right) {
			selected = selected.after_move(ls::Move::right);
		} else if (in.first == cli::detail::KeyType::ascii && in.second == ' ') {
			food.set(selected, !food.get(selected.x, selected.y));
		}
		selected = selected.clamp({0,0}, {(int)width-1, (int)height-1});
		for (unsigned y = 0; y < height+2; y++)
			out << "\u001B[A\r" << std::string(width*2+3, ' ');
		out << "\u001B[A";
	}
}

static void _PlaceSnake(std::ostream& out, unsigned width, unsigned height, unsigned snake, ls::Foods& food, std::vector<std::vector<ls::Position>>& snakes) {
	ls::Position selected(0,0);
	//Place Head
	while (true) {
		std::cout << rang::style::bold << '[' << (char)0x10<<(char)0x11<<(char)0x1E<<(char)0x1F<<"] to move the selection; [Space] to place snake-head            " << rang::style::reset << std::endl;
		_PrintBoard(out, width, height, selected, food, snakes);
		auto in = GetKey();
		if (in.first == cli::detail::KeyType::eof) {
			for (unsigned y = 0; y < height+2; y++)
				out << "\u001B[A\r" << std::string(width*2+3, ' ');
			out << "\u001B[A";
			return;
		} else if (in.first == cli::detail::KeyType::up) {
			selected = selected.after_move(ls::Move::up);
		} else if (in.first == cli::detail::KeyType::down) {
			selected = selected.after_move(ls::Move::down);
		} else if (in.first == cli::detail::KeyType::left) {
			selected = selected.after_move(ls::Move::left);
		} else if (in.first == cli::detail::KeyType::right) {
			selected = selected.after_move(ls::Move::right);
		} else if (in.first == cli::detail::KeyType::ascii && in.second == ' ') {
			snakes[snake].push_back(selected);
			break;
		}
		for (unsigned y = 0; y < height+2; y++)
			out << "\u001B[A\r" << std::string(width*2+3, ' ');
		out << "\u001B[A";
	}
	for (unsigned y = 0; y < height+2; y++)
		out << "\u001B[A\r";
	out << "\u001B[A";
	//Place Body using arrow-keys
	while(true) {
		std::cout << rang::style::bold << '['<<(char)0x10<<(char)0x11<<(char)0x1E<<(char)0x1F<<"] to place the body; [ESC] to exit placement" << rang::style::reset << std::endl;
		_PrintBoard(out, width, height, selected, food, snakes);
		auto in = GetKey();
		if (in.first == cli::detail::KeyType::eof) {
			for (unsigned y = 0; y < height+2; y++)
				out << "\u001B[A\r" << std::string(width*2+3, ' ');
			out << "\u001B[A";
			return;
		} else if (in.first == cli::detail::KeyType::up) {
			selected = selected.after_move(ls::Move::up);
			snakes[snake].push_back(selected);
		} else if (in.first == cli::detail::KeyType::down) {
			selected = selected.after_move(ls::Move::down);
			snakes[snake].push_back(selected);
		} else if (in.first == cli::detail::KeyType::left) {
			selected = selected.after_move(ls::Move::left);
			snakes[snake].push_back(selected);
		} else if (in.first == cli::detail::KeyType::right) {
			selected = selected.after_move(ls::Move::right);
			snakes[snake].push_back(selected);
		}
		for (unsigned y = 0; y < height+2; y++)
			out << "\u001B[A\r" << std::string(width*2+3, ' ');
		out << "\u001B[A";
	}
}

static ls::State _InputGameState(std::ostream& out) {
	unsigned width;
	unsigned height;

	out << "Please input the dimensions of the board" << std::endl;
	out << "Width: " << std::flush;
	std::cin >> width;
	out << "Height: " << std::flush;
	std::cin >> height;
	ls::Position selected(0,0);
	ls::Foods food(width, height);
	std::vector<std::vector<ls::Position>> snakes(2, std::vector<ls::Position>());
	_PlaceFood(out, width, height, food, snakes);
	for (unsigned i = 0; i < 2; i++)
		_PlaceSnake(out, width, height, i, food, snakes);

	std::vector<ls::Snake> sd;
	for (auto& body : snakes) {
		std::reverse(body.begin(), body.end());
		sd.emplace_back(std::move(body), 100);
	}
	return ls::State(width, height, std::move(sd), std::move(food));
}

static void EvalCmd(std::ostream& out) {
	const auto state = _InputGameState(out);
	Evaluator eval(ls::gm::Arena, state.getSnakes().size(), state.getWidth(), state.getHeight());
	const auto e = eval.evaluate(state);
	out << "Evaluating state\n";
	out << state << "\n";
	out << "Winner: " << e.winner << '\n';
	for (const auto& se : e.snakes) {
		out << "Snake Eval:\n"
			<< "\tMobility: " << se.mobility << '\n';
	}
	out << std::flush;
}

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
	diagMenu->Insert("eval", [](std::ostream& out){ EvalCmd(out); }, "Evaluate a specified gamestate");
	
    rootMenu->Insert(std::move(diagMenu));
}

void CLIController::Run() noexcept {
	cli::Cli cli(std::move(rootMenu));
	cli::LoopScheduler scheduler;
	cli.ExitAction( [&scheduler](auto& out){ scheduler.Stop(); } );
	cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout);
	this->session = &localSession;
	scheduler.Run();
}