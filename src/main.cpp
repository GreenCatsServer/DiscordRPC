﻿#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJShopLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SecretLayer4.hpp>

#include "../include/CustomPresense.hpp"
#include <Geode/loader/Dispatch.hpp>
#include <matjson.hpp>

// #include "./WineDRPCBridge.c"

using namespace geode::prelude;
using namespace gdrpc;

// template <std::move_constructible T, std::move_constructible P = std::monostate>
// class WineBridgeTask final;

$execute {
	srand(time(NULL));
	
	using NewRPCFilter = geode::DispatchFilter<std::string>;
	// log::info("{}", "set_default_rpc_enabled"_spr);
	new EventListener<NewRPCFilter>(+[](std::string const& newRPCStr) {
		// log::info("event going off!");
		// log::info("{}", newRPCStr);
		auto newRPCRes = matjson::parse(newRPCStr);
		if (newRPCRes.isErr()) {
			log::error("err updating rpc: {}", newRPCRes.err()->message);
			return ListenerResult::Propagate;
		}
		auto newRPC = newRPCRes.unwrap();
		gdrpc::GDRPC::getSharedInstance()->updateDiscordRP(
			newRPC["modID"].asString().unwrapOr(""),
			newRPC["details"].asString().unwrapOr(""),
			newRPC["state"].asString().unwrapOr(""),
			newRPC["smallImageKey"].asString().unwrapOr(""),
			newRPC["smallImageText"].asString().unwrapOr(""),
			newRPC["useTime"].asBool().unwrapOr(false),
			newRPC["shouldResetTime"].asBool().unwrapOr(false),
			newRPC["largeImageKey"].asString().unwrapOr(""),
			0,
			newRPC["joinSecret"].asString().unwrapOr(""),
			newRPC["largeImageText"].asString().unwrapOr(""),
			newRPC["partyMax"].asInt().unwrapOr(1)
		);
		return ListenerResult::Propagate;
	}, NewRPCFilter("update_rpc"_spr));

	using ToggleRPCFilter = geode::DispatchFilter<bool>;
	new EventListener(+[](bool enabled) {
		// log::info("updating default rpc enabled to {}", enabled);
		gdrpc::defaultRPCEnabled = enabled;
		return ListenerResult::Propagate;
	}, ToggleRPCFilter("set_default_rpc_enabled"_spr));
};

auto rpc = GDRPC::getSharedInstance();
const std::string MODID = ""_spr;

std::string convertGJDifficultyDemonToAssetKey(int difficulty) {
	switch (difficulty) {
		case 3:
			return "easy_demon";
		case 4:
			return "medium_demon";
		case 0:
			return "hard_demon";
		case 5:
			return "insane_demon";
		case 6:
			return "extreme_demon";
	}
	return "na";
}

std::string convertGJDifficultyToAssetKey(int difficulty) {
	switch (difficulty) {
		case -1:
			return "auto";
		case 0:
			return "na";
		case static_cast<int>(GJDifficulty::Easy):
			return "easy";
		case static_cast<int>(GJDifficulty::Normal):
			return "normal";
		case static_cast<int>(GJDifficulty::Hard):
			return "hard";
		case static_cast<int>(GJDifficulty::Harder):
			return "harder";
		case static_cast<int>(GJDifficulty::Insane):
			return "insane";
	}
	return "na";
}

std::string convertRobTopLevelToAssetKey(int lvlID) {
	switch (lvlID) {
		case 1:
			return "easy"; // Stereo Madness
		case 2:
			return ((Mod::get()->getSettingValue<bool>("funny-mode")) ? "extreme_demon" : "easy"); // Back On Track
		case 3:
			return "normal"; // Polargeist
		case 4:
			return "normal"; // Dry Out
		case 5:
			return "hard"; // Base After Base
		case 6:
			return "hard"; // Can't Let Go
		case 7:
			return "harder"; // Jumper
		case 8:
			return "harder"; // Time Machine
		case 9:
			return "harder"; // Cycles
		case 10:
			return "insane"; // xStep
		case 11:
			return "insane"; // Clutterfunk
		case 12:
			return "insane"; // Theory of Everything
		case 13:
			return "insane"; // Electroman Adventures
		case 14:
			return "hard_demon"; // Clubstep
		case 15:
			return ((Mod::get()->getSettingValue<bool>("funny-mode")) ? "easy_demon" : "insane"); // Electrodynamix
		case 16:
			return "insane"; // Hexagon Force
		case 17:
			return "harder"; // Blast Processing
		case 18:
			return "hard_demon"; // TOE 2
		case 19:
			return "harder"; // Geometrical Dominator
		case 20:
			return "hard_demon"; // Deadlocked
		case 21:
			return "insane"; // Fingerdash
		case 22:
			return "insane"; // Dash
		case 23:
			return "hard_demon"; // Explorers
		case 3001:
			return "hard"; // The Challenge
		// Spinoffs
		// Meltdown
		case 1001:
			return "easy"; // The Seven Seas
		case 1002:
			return "normal"; // Viking Arena
		case 1003:
			return "hard"; // Airborne Robots
		// GD World
		case 2001:
			return "easy"; // Payload
		case 2002:
			return "easy"; // Beast Mode
		case 2003:
			return "normal"; // Machina
		case 2004:
			return "normal"; // Years
		case 2005:
			return "normal"; // Frontlines
		case 2006:
			return "normal"; // Space Pirates
		case 2007:
			return "normal"; // Striker
		case 2008:
			return "normal"; // Embers
		case 2009:
			return "normal"; // Round 1
		case 2010:
			return "normal"; // Monster Dance Off
		// SubZero (i loooove gd cologne)
		case 4001:
			return "normal"; // Press Start
		case 4002:
			return "hard"; // Nock Em
		case 4003:
			return "harder"; // Power Trip
	}
	return "na";
}

auto getAverageDifficulty(GJGameLevel* level) {
	return level->m_ratings / level->m_ratingsSum;
}

std::string getAssetKey(GJGameLevel* level) {
	int stars = level->m_stars.value();
	auto difficulty = level->getAverageDifficulty();

	if (stars == 0) {
		return convertGJDifficultyToAssetKey(difficulty);
	}
	if (stars == 10) {
		auto demonKey = convertGJDifficultyDemonToAssetKey(level->m_demonDifficulty);
		// grand demoz
		// if (demonKey == "extreme_demon" && Loader::get()->isModLoaded("itzkiba.grandpa_demon")) {

		// }
		return demonKey;
	}
	if (level->m_levelID.value() < 128 || level->m_levelID.value() == 3001) {
		return convertRobTopLevelToAssetKey(level->m_levelID.value());
	}
	if (Loader::get()->isModLoaded("uproxide.more_difficulties"))
	{
		switch (stars) {
			case 1:
				return "auto";
			case 2:
				return "easy";
			case 3:
				return "normal";
			case 4:
				return "casual";
			case 5:
				return "hard";
			case 6:
				return "harder";
			case 7:
				return "touch";
			case 8:
				return "insane";
			case 9:
				return "cruel";
		}
		return "na";
	} else {
		switch (stars) {
			case 1:
				return "auto";
			case 2:
				return "easy";
			case 3:
				return "normal";
			case 4:
				return "hard";
			case 5:
				return "hard";
			case 6:
				return "harder";
			case 7:
				return "harder";
			case 8:
				return "insane";
			case 9:
				return "insane";
		}
		return "na";
	}
	
}

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		rpc->initDiscordRP();
		rpc->registerMod("gcs.rpc");
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Главное меню");

		return true;
	}

	void onOptions(CCObject* sender) {
		rpc->updateDiscordRP(MODID, "Листает менюшки","В настройках");
		return MenuLayer::onOptions(sender);
	}

	void onPlay(CCObject* sender) {
		rpc->updateDiscordRP(MODID, "Смотрит официальные уровни");
		return MenuLayer::onPlay(sender);
	}

	void onGarage(CCObject* sender) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Выбирает иконки");
		return MenuLayer::onGarage(sender);
	}

	void onAchievements(CCObject* sender) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		rpc->updateDiscordRP(MODID, "Листает менюшки", shouldBeFunny ? "Мечтает о ЮФОшке за рейт" : "Достижения");
		MenuLayer::onAchievements(sender); // NetheriteMiner didn't test this one :()
	}
};

class $modify(CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) return false;
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Вкладка Креатора");
		return true;
	}

	void onLeaderboards(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит лидерборды");
		return CreatorLayer::onLeaderboards(p0);
	}

	void onMyLevels(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит локальные уровни");
		return CreatorLayer::onMyLevels(p0);
	}

	void onSavedLevels(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит сохранённые уровни");
		return CreatorLayer::onSavedLevels(p0);
	}

	void onMapPacks(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Смотрит наиужаснейшие за всё время уровни";
		} else {
			state = "Смотрит Мап-Паки";
		}
		rpc->updateDiscordRP(MODID, "Листает менюшки", state);
		return CreatorLayer::onMapPacks(p0);
	}

	void onDailyLevel(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит ежедневный уровень");
		return CreatorLayer::onDailyLevel(p0);
	}

	void onWeeklyLevel(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит еженедельный уровень");
		return CreatorLayer::onWeeklyLevel(p0);
	}

	void onFeaturedLevels(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит вкладку Featured");
		return CreatorLayer::onFeaturedLevels(p0);
	}

	void onGauntlets(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Смотрит нормальные Мап-Паки";
		} else {
			state = "Смотрит Гаунтлеты";
		}
		rpc->updateDiscordRP(MODID, "Листает менюшки", state);
		return CreatorLayer::onGauntlets(p0);
	}

	void onAdventureMap(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит карту");
		return CreatorLayer::onAdventureMap(p0);
	}

	void onEventLevel(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит Ивент уровень");
		return CreatorLayer::onEventLevel(p0);
	}

	void onMultiplayer(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Завидует вертолёту Скрэтча";
		}
		else {
			state = "Смотрит Версус Мод";
		}
		rpc->updateDiscordRP(MODID, "Листает менюшки", state);
		return CreatorLayer::onMultiplayer(p0);
	}
	
	void onPaths(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит Пути");
		return CreatorLayer::onPaths(p0);
	}

	void onSecretVault(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string details = "";
		std::string state = "";
		if (shouldBeFunny) {
			details = "Ищет ответы на";
			state = "вики с Glubfub";
		}
		else {
			details = "Листает хранилища";
			state = "Vault of Secrets";
		}
		rpc->updateDiscordRP(MODID, details, state);
		return CreatorLayer::onSecretVault(p0);
	}

	void onTopLists(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Смотрит Мап-Паки от модераторов";
		}
		else {
			state = "Смотрит списки уровней";
		}
		rpc->updateDiscordRP(MODID, "Листает менюшки", state);
		return CreatorLayer::onTopLists(p0);
	}

	void onTreasureRoom(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит комнату сокровищ");
		return CreatorLayer::onTreasureRoom(p0);
	}

	void onChallenge(CCObject* p0) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит квесты"); 
		return CreatorLayer::onChallenge(p0);
	}
};

class $modify(LevelSearchLayer) {
	bool init(int p0) {
		if (!LevelSearchLayer::init(p0)) return false;
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Вкладка поиска");
		return true;
	}
};

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, p1)) return false;
		bool isRated = level->m_stars.value() != 0;
		
		rpc->updateDiscordRP(MODID, 
			"Играет в уровень",
			std::string(level->m_levelName) + " от " + std::string(level->m_creatorName),
			getAssetKey(level),
			(isRated) ? "Оценён" : "Не оценён"
		);
		return true;
	}
};

// stolen, er, borrowed from BetterInfo (https://github.com/Cvolton/betterinfo-geode/blob/master/src/utils/TimeUtils.cpp)
std::string workingTime(int value){
    if(value < 0) return fmt::format("NA ({})", value);
    if(value == 0) return "NA";

    int hours = value / 3600;
    int minutes = (value % 3600) / 60;
    int seconds = value % 60;

    std::ostringstream stream;
    if(hours > 0) stream << hours << "ч ";
    if(minutes > 0) stream << minutes << "м ";
    stream << seconds << "с";

    return stream.str();
}

class $modify(MyLevelEditorLayer, LevelEditorLayer) {
	bool init(GJGameLevel* level, bool p0) {
		if (!LevelEditorLayer::init(level, p0)) return false;

		#ifdef GEODE_IS_WINDOWS
		this->schedule(schedule_selector(MyLevelEditorLayer::updateStatus), 1);
		#else
		MyLevelEditorLayer::updateStatus(69.420);
		#endif

		return true;
	}

	void updateStatus(float dt) {
		auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");
		std::string details = "";
		if (shouldShowSensitive) {
			details = "Работает над \"" + std::string(m_level->m_levelName) + "\"";
		} else {
			details = "Работает над уровнем";
		}
		int objectCount;
		#ifdef GEODE_IS_WINDOWS
		objectCount = m_objects->count();
		#endif
		#ifdef GEODE_IS_MACOS
		objectCount = m_level->m_objectCount;
		#endif
		auto showTotalTime = Mod::get()->getSettingValue<bool>("show-total-time");
		std::string totalTime;
		if (showTotalTime) {
			totalTime = " (уже " + workingTime(m_level->m_workingTime) + ")";
		}
		rpc->updateDiscordRP(
			MODID,
			details,
			std::to_string(objectCount) + " объектов",
			"editor",
			"Строит уровень",
			true,
			false,
			"",
			m_level->m_workingTime
		);
	}
};

class $modify(MyPlayLayer, PlayLayer) {
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;

		MyPlayLayer::updateRP(true);
		#ifdef GEODE_IS_WINDOWS
		this->schedule(schedule_selector(MyPlayLayer::updateRPLoop), 1);
		#endif

		return true;
	}

	#ifdef GEODE_IS_MACOS
	void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
		PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);

		MyPlayLayer::updateRP();
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		MyPlayLayer::updateRP();
	}

	void levelComplete() {
		PlayLayer::levelComplete();
		MyPlayLayer::updateRP();
	}
	#endif

	void updateRPLoop(float dt) {
		MyPlayLayer::updateRP();
	}

	void updateRP(bool resetTime = false) {
		bool isRated = m_level->m_stars.value() != 0;
		auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");

		std::string state;

		bool isRobTopLevel = (
			(m_level->m_levelID.value() < 128 && m_level->m_levelID.value() != 0) ||
				m_level->m_levelID.value() == 3001 || m_level->m_levelID.value() < 5003 ||
				m_level->m_levelID.value() > 5000
		) && std::string(m_level->m_creatorName) == "";
		// log::info("{}", std::to_string(m_level->m_levelID.value()));

		if (m_level->m_levelType != GJLevelType::Editor || shouldShowSensitive) {
			state = std::string(m_level->m_levelName) 
				+ " by " + 
				(
					(isRobTopLevel) ? "RobTopGames" : std::string(m_level->m_creatorName)
				);
		} else if (!shouldShowSensitive) {
			state = "Играет в свой уровень";
		}

		std::string bestString;
		if (m_level->isPlatformer()) {
			int sec = round(m_level->m_bestTime / 1000);
			bestString = std::to_string(sec) + "сек";

			if (m_level->m_bestTime == 0) {
				bestString = "Нет рекорда";
			}
		} else {
			bestString = std::to_string(m_level->m_normalPercent.value()) + "%";
		}

		std::string details = (m_isPracticeMode ? "Практикует" : "Играет в");

		bool isDaily = m_level->m_dailyID.value() != 0;
		bool isDemon = m_level->m_demon.value() != 0;

		if (isDaily) {
			if (isDemon) {
				details = details + " еженедельный";
			}
			else {
				details = details + " ежедневный";
			}
		}
		
		if (m_level->isPlatformer()) {
			details = details + " платформер";
		} else {
			details = details + " уровень";
		}

		std::string detailsPercentString = "";
		auto gm = GameManager::sharedState()->getPlayLayer();
		#ifdef GEODE_IS_WINDOWS
		if (!m_level->isPlatformer() && Mod::get()->getSettingValue<bool>("show-percent")) {
			detailsPercentString = " (" + std::to_string(gm->getCurrentPercentInt()) + "%)";
		}
		#endif
		details = fmt::format("{} {} (Рекорд: {})", details, detailsPercentString, bestString);

		rpc->updateDiscordRP(MODID, 
			details,
			state,
			getAssetKey(m_level),
			(isRated) ? "Оценён" : "Не оценён",
			true,
			resetTime
		);
	}
};

class $modify(GJShopLayer) {
	bool init(ShopType p0) {
		if (!GJShopLayer::init(p0)) return false;

		auto orbs = fmt::format("∴{}", GameStatsManager::sharedState()->m_playerStats->valueForKey("14")->getCString());
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		switch(p0) {
			case ShopType::Normal:
				rpc->updateDiscordRP(MODID, shouldBeFunny ? "За покупочками у Золгурота" : "За покупочками у Шопкипера", orbs);
				break;
			case ShopType::Secret:
				rpc->updateDiscordRP(MODID, "За покупочками у Скрэтча", orbs);
				break;
			case ShopType::Community:
				rpc->updateDiscordRP(MODID, "За покупочками у Потбора", orbs);
				break;
			default:
				if (shouldBeFunny) {
					// ⬘ seems like the best unicode diamond for the diamond shopkeeper when the ShopType enum contains all 5
					rpc->updateDiscordRP(MODID, "За покупочками", "в Ашане");
				}
				else {
					rpc->updateDiscordRP(MODID, "За покупочками", orbs);
				}
		}

		return true;
	}
};

class $modify(GJGarageLayer) {
	void onShards(CCObject* sender) {
		rpc->updateDiscordRP(MODID, "Листает менюшки", "Смотрит Шарды");
		GJGarageLayer::onShards(sender);
	}
};

#ifdef GEODE_IS_WINDOWS
class $modify(OptionsLayer) {
	void onSecretVault(CCObject* sender) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string details = "";
		std::string state = "";
		if (shouldBeFunny) {
			details = "Ищет ответы на";
			state = "вики с Spooky";
		}
		else {
			details = "Листает хранилища";
			state = "The Vault";
		}
		rpc->updateDiscordRP(MODID, details, state);
		return OptionsLayer::onSecretVault(sender);
	}
};
#endif

class $modify(SecretLayer4) {
	bool init() {
		if (!SecretLayer4::init()) return false;

		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string details = "";
		std::string state = "";
		if (shouldBeFunny) {
			details = "Ищет ответы на";
			state = "вики с Gatekeeper";
		}
		else {
			details = "Листает хранилища";
			state = "Chamber of Time";
		}
		rpc->updateDiscordRP(MODID, details, state);

		return true;
	}
};
