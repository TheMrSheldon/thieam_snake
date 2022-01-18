#include "interface/cli_controller.h"

#include "ai/eval.h"

#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>

#include <libsnake/position.h>
#include <libsnake/util.h>
#include <libsnake/state.h>
#include <functional>
#include <vector>
#include <iomanip>

static const rang::fg SnakeColors[] = {
	rang::fg::blue, rang::fg::green, rang::fg::magenta, rang::fg::yellow,
	rang::fg::cyan, rang::fg::red, rang::fg::gray
};

static constexpr auto PrevLineCR = "\u001B[A\r";
static constexpr auto ClearLine = "\33[2K\r";

static inline void ClearLastLines(std::ostream& os, size_t num) {
	for (size_t i = 0; i < num; i++)
		os << PrevLineCR << ClearLine;
}

static std::pair<cli::detail::KeyType, char> GetKey() {
	using KeyType = cli::detail::KeyType;
#ifdef WIN32
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
#else
	int c = std::getchar();
	switch(c) {
		case EOF:
		case 4:  // EOT
			return std::make_pair(KeyType::eof,' ');
			break;
		case 127: return std::make_pair(KeyType::backspace,' '); break;
		case 10: return std::make_pair(KeyType::ret,' '); break;
		case 27: // symbol
			c = std::getchar();
			if ( c == 91 ) {// arrow keys
				c = std::getchar();
				switch(c) {
					case 51:
						c = std::getchar();
						if (c == 126) return std::make_pair(KeyType::canc,' ');
						else return std::make_pair(KeyType::ignored,' ');
						break;
					case 65: return std::make_pair(KeyType::up,' ');
					case 66: return std::make_pair(KeyType::down,' ');
					case 68: return std::make_pair(KeyType::left,' ');
					case 67: return std::make_pair(KeyType::right,' ');
					case 70: return std::make_pair(KeyType::end,' ');
					case 72: return std::make_pair(KeyType::home,' ');
					default: return std::make_pair(KeyType::ignored,' ');
				}
			}
			break;
		default: {// hopefully ascii
			const char ch = static_cast<char>(c);
			return std::make_pair(KeyType::ascii, ch);
		}
	}
	return std::make_pair(KeyType::ignored,' ');
#endif
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
			ClearLastLines(out, height+3);
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
		ClearLastLines(out, height+3);
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
			ClearLastLines(out, height+3);
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
		selected = selected.clamp({0,0}, {(int)width-1, (int)height-1});
		ClearLastLines(out, height+3);
	}
	ClearLastLines(out, height+3);
	//Place Body using arrow-keys
	while(true) {
		std::cout << rang::style::bold << '['<<(char)0x10<<(char)0x11<<(char)0x1E<<(char)0x1F<<"] to place the body; [ESC] to exit placement" << rang::style::reset << std::endl;
		_PrintBoard(out, width, height, selected, food, snakes);
		auto in = GetKey();
		if (in.first == cli::detail::KeyType::eof) {
			ClearLastLines(out, height+3);
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
		ClearLastLines(out, height+3);
	}
}

static ls::State _InputGameState(std::ostream& out) {
	unsigned width;
	unsigned height;
	unsigned numSnakes;

	out << "Please input the dimensions of the board" << std::endl;
	out << "Width: " << std::flush;
	std::cin >> width;
	out << "Height: " << std::flush;
	std::cin >> height;
	out << "#Snakes: " << std::flush;
	std::cin >> numSnakes;
	ls::Position selected(0,0);
	ls::Foods food(width, height);
	std::vector<std::vector<ls::Position>> snakes(numSnakes, std::vector<ls::Position>());
	_PlaceFood(out, width, height, food, snakes);
	for (unsigned i = 0; i < numSnakes; i++)
		_PlaceSnake(out, width, height, i, food, snakes);

	std::vector<ls::Snake> sd;
	for (auto& body : snakes) {
		//std::reverse(body.begin(), body.end());
		sd.emplace_back(std::move(body), 100, ls::SnakeFlags::ByIndex(sd.size()));
	}
	return ls::State(width, height, std::move(sd), std::move(food));
}

static void EvalCmd(std::ostream& out) {
	const auto state = _InputGameState(out);
	StateOfMind mind(0);
	Evaluator eval(ls::gm::Standard, state.getSnakes().size(), state.getWidth(), state.getHeight(), mind);
	const auto e = eval.evaluate(state, 1);
	out << "Evaluating state\n";
	out << state << "\n";
	out << "Winner: " << e.winner << '\n';
	for (const auto& se : e.snakes) {
		out << "Snake Eval:\n"
			<< "\tMobility: " << se.mobility << '\n';
	}
	out << "Env'buffer:\n";
	const auto& env = eval.getEnvBuffer();
	ls::State::drawBoard(out, env.getWidth(), env.getHeight(), [&out, &env](const ls::Position& pos){
		const auto& entry = env.getEntry(pos);
		if (entry.snake.size() == 1) {
			ASSERT(entry.snake.getIndex() < sizeof(SnakeColors)/sizeof(SnakeColors[0]), "Too many snakes for our colors to handle");
			out <<  SnakeColors[entry.snake.getIndex()];
			if (entry.timeBlocked.until)
				out << rang::bg::red;
			out << std::right << std::setw(2) << (unsigned)entry.timeBlocked.turn
				<< rang::style::reset;
		} else {
			out << '#';
		}
		return "";
	}, 2);
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