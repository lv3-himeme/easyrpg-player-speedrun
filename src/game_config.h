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

#ifndef EP_GAME_CONFIG_H
#define EP_GAME_CONFIG_H

/**
 * This class manages global engine configuration.
 * For game specific settings see Game_ConfigGame.
 *
 * @see Game_ConfigGame
 */

#include "config_param.h"
#include "filesystem.h"
#include "options.h"
#include "input_buttons.h"
#include "utils.h"

class CmdlineParser;

namespace ConfigEnum {
	enum class ScalingMode {
		/** Nearest neighbour to fit screen */
		Nearest,
		/** Like NN but only scales to integers */
		Integer,
		/** Integer followed by Bilinear downscale to fit screen */
		Bilinear,
	};

	enum class GameResolution {
		/** 320x240 */
		Original,
		/** 416x240 */
		Widescreen,
		/** 560x240 */
		Ultrawide
	};

	enum class StartupLogos {
		None,
		Custom,
		All
	};

	enum class StartupLangSelect {
		Never,
		/* Shows language screen when no saves are found */
		FirstStartup,
		/* Always show the language screen before the title */
		Always
	};

	enum class ShowFps {
		/** Do not show */
		OFF,
		/** When windowed: Title bar, fullscreen: Overlay */
		ON,
		/** Always overlay */
		Overlay
	};
};

#ifdef HAVE_FLUIDLITE
#define EP_FLUID_NAME "FluidLite"
#else
#define EP_FLUID_NAME "FluidSynth"
#endif

struct Game_ConfigPlayer {
	StringConfigParam autobattle_algo{ "", "", "", "", "" };
	StringConfigParam enemyai_algo{ "", "", "", "", "" };
	BoolConfigParam settings_autosave{ "Lưu cài đặt khi thoát", "Tự động lưu cài đặt khi thoát", "Player", "SettingsAutosave", false };
	BoolConfigParam settings_in_title{ "Hiển thị cài đặt ở màn hình tiêu đề", "Hiển thị mục chỉnh sửa cài đặt ở màn hình tiêu đề", "Player", "SettingsInTitle", false };
	BoolConfigParam settings_in_menu{ "Hiển thị cài đặt ở menu", "Hiển thị mục chỉnh sửa cài đặt ở màn hình menu", "Player", "SettingsInMenu", false };
	EnumConfigParam<ConfigEnum::StartupLogos, 3> show_startup_logos{
		"Biểu tượng khởi động", "Biểu tượng được hiển thị khi khởi động game", "Player", "StartupLogos", ConfigEnum::StartupLogos::Custom,
		Utils::MakeSvArray("Không", "Tuỳ chỉnh", "Tất cả"),
		Utils::MakeSvArray("none", "custom", "all"),
		Utils::MakeSvArray("Không hiển thị thêm biểu tượng nào", "Hiển thị biểu tượng tuỳ chỉnh được tích hợp vào game", "Hiển thị toàn bộ biểu tượng, bao gồm cả biểu tượng mặc định của RPG Maker")};
	PathConfigParam font1 { "Phông chữ 1", "Game sẽ chọn phông chữ số 1 hoặc phông chữ số 2", "Player", "Font1", "" };
	RangeConfigParam<int> font1_size { "Cỡ chữ của phông chữ 1", "", "Player", "Font1Size", 12, 6, 16};
	PathConfigParam font2 { "Phông chữ 2", "Game sẽ chọn phông chữ số 1 hoặc phông chữ số 2", "Player", "Font2", "" };
	RangeConfigParam<int> font2_size { "Cỡ chữ của phông chữ 2", "", "Player", "Font2Size", 12, 6, 16};
	EnumConfigParam<ConfigEnum::StartupLangSelect, 3> lang_select_on_start {
		"Menu chọn ngôn ngữ", "Hiển thị bảng chọn ngôn ngữ khi khởi động game", "Player", "StartupLangSelect", ConfigEnum::StartupLangSelect::FirstStartup,
		Utils::MakeSvArray("Không bao giờ", "Lần đầu tiên", "Luôn hiển thị"),
		Utils::MakeSvArray("never", "FirstStartup", "always"),
		Utils::MakeSvArray("Không bao giờ hiển thị menu ngôn ngữ khi khởi động", "Chỉ hiển thị lần đầu (khi không tìm thấy tệp lưu game)", "Luôn hiển thị menu ngôn ngữ trước màn hình tiêu đề") };
	BoolConfigParam lang_select_in_title{ "Menu ngôn ngữ ở màn hình tiêu đề", "Hiển thị mục menu ngôn ngữ trong màn hình tiêu đề", "Player", "LanguageInTitle", true };
	BoolConfigParam log_enabled{ "Ghi nhật ký", "Ghi các thông điệp chẩn đoán vào tệp nhật ký", "Player", "Logging", true };
	RangeConfigParam<int> screenshot_scale { "Tỉ lệ chụp màn hình", "Phóng ảnh chụp màn hình theo hệ số được chỉ định", "Player", "ScreenshotScale", 1, 1, 24};
	BoolConfigParam screenshot_timestamp{ "Dấu thời gian ảnh chụp", "Thêm ngày và giờ hiện tại vào tên tệp", "Player", "ScreenshotTimestamp", true };
	BoolConfigParam automatic_screenshots{ "Tự động chụp màn hình", "Chụp màn hình định kỳ", "Player", "AutomaticScreenshots", false };
	RangeConfigParam<int> automatic_screenshots_interval{ "Khoảng cách giữa các ảnh chụp", "Thời gian giữa các lần chụp màn hình tự động (giây)", "Player", "AutomaticScreenshotsInterval", 30, 1, 999999 };

	void Hide();
};

struct Game_ConfigVideo {
	LockedConfigParam<std::string> renderer{ "Bộ dựng hình", "Bộ máy kết xuất đồ họa", "auto" };
	BoolConfigParam vsync{ "Đồng bộ V-Sync", "Bật/tắt chế độ V-Sync (Khuyên dùng: BẬT)", "Video", "Vsync", true };
	BoolConfigParam fullscreen{ "Toàn màn hình", "Chuyển đổi giữa chế độ toàn màn hình và cửa sổ", "Video", "Fullscreen", true };
	EnumConfigParam<ConfigEnum::ShowFps, 3> fps{
		"Bộ đếm FPS", "Cách hiển thị bộ đếm FPS", "Video", "Fps", ConfigEnum::ShowFps::OFF,
		Utils::MakeSvArray("[TẮT]", "[BẬT]", "Overlay"),
		Utils::MakeSvArray("off", "on", "overlay"),
		Utils::MakeSvArray("Không hiển thị bộ đếm FPS", "Hiển thị bộ đếm FPS", "Luôn hiển thị bộ đếm FPS trong cửa sổ")};
	RangeConfigParam<int> fps_limit{ "Giới hạn khung hình", "Giới hạn số khung hình mỗi giây (Khuyên dùng: 60)", "Video", "FpsLimit", DEFAULT_FPS, 0, 99999 };
	ConfigParam<int> window_zoom{ "Thu phóng cửa sổ", "Điều chỉnh mức độ thu phóng cửa sổ", "Video", "WindowZoom", 2 };
	EnumConfigParam<ConfigEnum::ScalingMode, 3> scaling_mode{ "Phương pháp co giãn", "Cách thức hình ảnh được co giãn", "Video", "ScalingMode", ConfigEnum::ScalingMode::Nearest,
		Utils::MakeSvArray("Gần nhất", "Bội số", "Đa tuyến tính"),
		Utils::MakeSvArray("nearest", "integer", "bilinear"),
		Utils::MakeSvArray("Phóng theo kích thước màn hình (Có thể gây lỗi hình ảnh)", "Phóng theo bội số của độ phân giải trò chơi", "Giống tuỳ chọn Gần nhất nhưng đầu ra được làm mờ để tránh lỗi hình ảnh")};
	BoolConfigParam stretch{ "Kéo giãn", "Kéo giãn theo chiều ngang của cửa sổ/màn hình", "Video", "Stretch", false };
	BoolConfigParam pause_when_focus_lost{ "Tạm dừng khi chuyển khỏi cửa sổ", "Tạm dừng chương trình khi chạy nền", "Video", "PauseWhenFocusLost", true };
	BoolConfigParam touch_ui{ "Giao diện cảm ứng", "Hiển thị giao diện cảm ứng", "Video", "TouchUi", true };
	EnumConfigParam<ConfigEnum::GameResolution, 3> game_resolution{ "Độ phân giải", "Độ phân giải trò chơi. Thay đổi yêu cầu khởi động lại.", "Video", "GameResolution", ConfigEnum::GameResolution::Original,
		Utils::MakeSvArray("Gốc (khuyên dùng)", "Màn ảnh rộng (thử nghiệm)", "Màn ảnh siêu rộng (thử nghiệm)"),
		Utils::MakeSvArray("original", "widescreen", "ultrawide"),
		Utils::MakeSvArray("Độ phân giải mặc định (320x240, 4:3)", "Có thể gây lỗi (416x240, 16:9)", "Có thể gây lỗi (560x240, 21:9)")};

	// These are never shown and are used to restore the window to the previous position
	ConfigParam<int> window_x{ "", "", "Video", "WindowX", -1 };
	ConfigParam<int> window_y{ "", "", "Video", "WindowY", -1 };
	ConfigParam<int> window_width{ "", "", "Video", "WindowWidth", -1 };
	ConfigParam<int> window_height{ "", "", "Video", "WindowHeight", -1 };

	void Hide();
};

struct Game_ConfigAudio {
	RangeConfigParam<int> music_volume{ "Âm lượng nhạc nền", "Điều chỉnh âm lượng của nhạc nền", "Audio", "MusicVolume", 100, 0, 100 };
	RangeConfigParam<int> sound_volume{ "Âm lượng hiệu ứng", "Điều chỉnh âm lượng của hiệu ứng âm thanh", "Audio", "SoundVolume", 100, 0, 100 };
	BoolConfigParam fluidsynth_midi { EP_FLUID_NAME " (SF2)", "Phát MIDI bằng soundfont SF2", "Audio", "Fluidsynth", true };
	BoolConfigParam wildmidi_midi { "WildMidi (GUS)", "Phát MIDI bằng GUS patch", "Audio", "WildMidi", true };
	BoolConfigParam native_midi { "MIDI hệ thống", "Phát MIDI qua hệ điều hành", "Audio", "NativeMidi", true };
	LockedConfigParam<std::string> fmmidi_midi { "FmMidi", "Phát MIDI bằng bộ tổng hợp tích hợp sẵn", "[Luôn BẬT]" };
	PathConfigParam soundfont { "Soundfont", "Soundfont sử dụng cho " EP_FLUID_NAME, "Audio", "Soundfont", "" };

	void Hide();
};

struct Game_ConfigInput {
	RangeConfigParam<int> speed_modifier_a{ "Tăng tốc A: Tốc độ", "Thiết lập tốc độ khi nhấn tăng tốc A", "Input", "SpeedModifierA", 3, 2, 100 };
	RangeConfigParam<int> speed_modifier_b{ "Tăng tốc B: Tốc độ", "Thiết lập tốc độ khi nhấn tăng tốc B", "Input", "SpeedModifierB", 10, 2, 100 };
	BoolConfigParam gamepad_swap_analog{ "Tay cầm: Đảo cần analog", "Đảo vị trí cần analog trái và phải", "Input", "GamepadSwapAnalog", false };
	BoolConfigParam gamepad_swap_dpad_with_buttons{ "Tay cầm: Đảo D-Pad với nút", "Đảo D-Pad với các nút ABXY", "Input", "GamepadSwapDpad", false };
	BoolConfigParam gamepad_swap_ab_and_xy{ "Tay cầm: Đảo AB và XY", "Đảo A và B với X và Y", "Input", "GamepadSwapAbxy", false };
	Input::ButtonMappingArray buttons;

	void Hide();
};

struct Game_Config {
	/** Gameplay subsystem options */
	Game_ConfigPlayer player;

	/** Video subsystem options */
	Game_ConfigVideo video;

	/** Audio subsystem options */
	Game_ConfigAudio audio;

	/** Input subsystem options */
	Game_ConfigInput input;

	/**
	 * Create an application config. This first determines the config file path if any,
	 * loads the config file, then loads command line arguments.
	 */
	static Game_Config Create(CmdlineParser& cp);

	/** @return config file path from command line args if found */
	static std::string GetConfigPath(CmdlineParser& cp);

	/**
	 * Returns the a filesystem view to the global config directory
	 */
	static FilesystemView GetGlobalConfigFilesystem();

	/**
	 * Returns the filesystem view to the soundfont directory
	 * By default this is config/Soundfont
	 */
	static FilesystemView GetSoundfontFilesystem();

	/**
	 * Returns the filesystem view to the font directory
	 * By default this is config/Font
	 */
	static FilesystemView GetFontFilesystem();

	/**
	 * Returns a handle to the global config file for reading.
	 * The file is created if it does not exist.
	 *
	 * @return handle to the global file
	 */
	static Filesystem_Stream::InputStream GetGlobalConfigFileInput();

	/**
	 * Returns a handle to the global config file for writing.
	 * The file is created if it does not exist.
	 *
	 * @return handle to the global file
	 */
	static Filesystem_Stream::OutputStream GetGlobalConfigFileOutput();

	static Filesystem_Stream::OutputStream& GetLogFileOutput();

	static void CloseLogFile();

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

	/**
	 * Writes our configuration to the given stream.
	 *
	 * @param os stream to write to
	 */
	void WriteToStream(Filesystem_Stream::OutputStream& os) const;
};

#endif
