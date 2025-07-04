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

#ifndef EP_GAME_CONFIG_GAME_H
#define EP_GAME_CONFIG_GAME_H

/**
 * This class manages game specific configuration.
 * For engine specific settings see Game_Config.
 *
 * All settings here are currently readonly (To be set in EasyRPG.ini)
 *
 * @see Game_Config
 */

#include "config_param.h"
#include "filesystem_stream.h"

class CmdlineParser;

struct Game_ConfigGame {
	// FIXME? Editing these settings through the config scene is not supported

	BoolConfigParam new_game{ "Bắt đầu game mới", "Bỏ qua phần tiêu đề và bắt đầu game mới luôn", "Game", "NewGame", false };
	StringConfigParam engine_str{ "Engine", "", "Game", "Engine", std::string() };
	BoolConfigParam fake_resolution{ "Giả mạo độ phân giải", "Cho phép trò chơi chạy ở độ phân giải cao hơn (có thể thành công một phần)", "Game", "FakeResolution", false };
	BoolConfigParam patch_easyrpg{ "EasyRPG", "Các phần mở rộng của EasyRPG", "Patch", "EasyRPG", false };
	BoolConfigParam patch_destiny{ "Destiny Patch", "", "Patch", "Destiny", false };
	BoolConfigParam patch_dynrpg{ "DynRPG", "", "Patch", "DynRPG", false };
	ConfigParam<int> patch_maniac{ "Maniac Patch", "", "Patch", "Maniac", 0 };
	BoolConfigParam patch_common_this_event{ "Common This Event", "Hỗ trợ \"Sự kiện này\" (This Event) trong các Sự kiện chung (Common Events)", "Patch", "CommonThisEvent", false };
	BoolConfigParam patch_unlock_pics{ "Mở khoá hình ảnh", "Cho phép các lệnh hình ảnh hoạt động khi hộp thoại đang hiển thị", "Patch", "PicUnlock", false };
	BoolConfigParam patch_key_patch{ "Ineluki Key Patch", "Hỗ trợ \"Ineluki Key Patch\"", "Patch", "KeyPatch", false };
	BoolConfigParam patch_rpg2k3_commands{ "Lệnh sự kiện RPG2k3", "Bật hỗ trợ cho các lệnh sự kiện của RPG Maker 2003", "Patch", "RPG2k3Commands", false };
	ConfigParam<int> patch_anti_lag_switch{ "Chống Lag", "Vô hiệu làm mới trang sự kiện khi công tắc được bật", "Patch", "AntiLagSwitch", 0 };
	ConfigParam<int> patch_direct_menu{ "Truy cập menu trực tiếp", "Cho phép truy cập trực tiếp vào các màn hình con trong menu mặc định", "Patch", "DirectMenu", 0 };

	// Command line only
	BoolConfigParam patch_support{ "Hỗ trợ bản vá", "Tắt tùy chọn này sẽ vô hiệu hóa toàn bộ hỗ trợ bản vá", "", "", true };

	// Indicators of overridden settings to disable auto detection
	bool patch_override = false;

	int engine = 0;

	/**
	 * Create a game config from the config file in the game directory, then loads command line arguments.
	 */
	static Game_ConfigGame Create(CmdlineParser& cp);

	/**
	 * Load configuration values from a stream;
	 *
	 * @param is stream to read from.
	 * @post values of this are updated with values found in the stream.
	 */
	void LoadFromStream(Filesystem_Stream::InputStream& is);

	/**
	 * Load configuration values from a command line arguments.
	 *
	 * @param cp the command line parser to use.
	 * @post values of this are updated with values found in command line args.
	 */
	void LoadFromArgs(CmdlineParser& cp);

	/** Outputs a list of active patches */
	void PrintActivePatches();
};

#endif
