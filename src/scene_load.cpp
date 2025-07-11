/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

// Headers
#include <sstream>
#include "filefinder.h"
#include "output.h"
#include "player.h"
#include "scene_load.h"
#include "scene_map.h"
#include "nbhzvn/speedrun.h"
#include "emscripten.h"

Scene_Load::Scene_Load() :
	Scene_File(ToString(lcf::Data::terms.load_game_message)) {
	Scene::type = Scene::Load;
}

void Scene_Load::Action(int index) {
	#if EMSCRIPTEN
		auto res = Speedrun::Continue();
		if (res.success) {
			Output::Info(res.message);
			Speedrun::StartPing();
			std::string save_name = fs.FindFile(fmt::format("Save{:02d}.lsd", index + 1));
			Player::LoadSavegame(save_name, index + 1);
			std::string display_name = res.data["display_name"].get<std::string>();
			int32_t delta_time = static_cast<int32_t>(res.data["current_time"]) - static_cast<int32_t>(res.data["timestamp"]);
			EM_ASM_({
				Speedrun.start(UTF8ToString($0), $1, $2);
			}, display_name.c_str(), Speedrun::GetPlaytime(), delta_time);
		}
		else Output::Warning(res.message);
	#else
		std::string save_name = fs.FindFile(fmt::format("Save{:02d}.lsd", index + 1));
		Player::LoadSavegame(save_name, index + 1);
	#endif
}

bool Scene_Load::IsSlotValid(int index) {
	return file_windows[index]->IsValid();
}
