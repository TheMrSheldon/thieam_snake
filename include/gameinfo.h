#pragma once

#include <exception>
#include <inttypes.h>
#include <memory>
#include <string>

#include <libsnake/gamemode.h>
#include <libsnake/gamemodes/standard.h>
#include <libsnake/gamemodes/challenge.h>
#include <libsnake/gamemodes/squad.h>

namespace GamemodeNames {
	constexpr auto Standard = "standard";
	constexpr auto Solo = "solo";
	constexpr auto Royale = "royale";
	constexpr auto Squad = "squad";
	constexpr auto Constrictor = "constrictor";
}

struct GameInfo {
	std::string id;
	uint32_t timeout;
	std::string source;
	struct {
		/**
		 * @brief Name of the ruleset being used to run this game. Possible values include:
		 * standard, solo, royale, squad, constrictor. See Game Modes for more information on each ruleset.
		 */
		std::string name;
		/**
		 * @brief The release version of the Rules module used in this game.
		 */
		std::string version;
		/**
		 * @brief A collection of specific settings being used by the current game that control how the rules are applied.
		 */
		struct {
			/**
			 * @brief Percentage chance of spawning a new food every round.
			 */
			int foodSpawnChance;
			/**
			 * @brief Minimum food to keep on the board every turn.
			 */
			int minimumFood;
			/**
			 * @brief Health damage a snake will take when ending its turn in a hazard. This stacks on top of
			 * the regular 1 damage a snake takes per turn.
			 */
			int hazardDamagePerTurn;
			/**
			 * @brief Settings specific to the royale gamemode.
			 */
			struct {
				/**
				 * @brief The number of turns between generating new hazards (shrinking the safe board space).
				 */
				int shrinkEveryNTurns;
			} royale;
			/**
			 * @brief Settings specific to the squad gamemode.
			 */
			struct {
				/**
				 * @brief Allow members of the same squad to move over each other without dying.
				 */
				bool allowBodyCollisions;
				/**
				 * @brief All squad members are eliminated when one is eliminated.
				 */
				bool sharedElimination;
				/**
				 * @brief All squad members share health.
				 */
				bool sharedHealth;
				/**
				 * @brief All squad members share length.
				 */
				bool sharedLength;
			} squad;
		} settings;
	} rules;
	
	std::unique_ptr<ls::Gamemode> constructGamemode() const {
		if (rules.name == GamemodeNames::Standard) {
			return std::make_unique<ls::gm::StandardGamemode>();
		} else if (rules.name == GamemodeNames::Solo) {
			return std::make_unique<ls::gm::ChallengeGamemode>();
		} else if (rules.name == GamemodeNames::Royale) {
			//return std::make_unique<ls::gm::RoyaleGamemode>();
			throw std::exception("The constrictor gamemode is not yet implemented");
		} else if (rules.name == GamemodeNames::Squad) {
			return std::make_unique<ls::gm::SquadGamemode>();
		} else if (rules.name == GamemodeNames::Constrictor) {
			//return std::make_unique<ls::gm::ConstrictorGamemode>();
			throw std::exception("The constrictor gamemode is not yet implemented");
		} else {
			std::string msg = "The gamemode '"+rules.name+"' is unknown";
			throw std::exception(msg.c_str());
		}
	}	
};