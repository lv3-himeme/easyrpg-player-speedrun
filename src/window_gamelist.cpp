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
#include "window_gamelist.h"
#include "filefinder.h"
#include "bitmap.h"
#include "font.h"
#include "system.h"

Window_GameList::Window_GameList(int ix, int iy, int iwidth, int iheight) :
	Window_Selectable(ix, iy, iwidth, iheight) {
	column_max = 1;
}

bool Window_GameList::Refresh(FilesystemView filesystem_base, bool show_dotdot) {
	base_fs = filesystem_base;
	if (!base_fs) {
		return false;
	}

	game_entries.clear();

	this->show_dotdot = show_dotdot;

#ifndef USE_CUSTOM_FILEBUF
	// Calling "Create" while iterating over the directory list appears to corrupt
	// the file entries probably because of a reallocation due to caching new entries.
	// Create a copy of the entries instead to workaround this issue.
	DirectoryTree::DirectoryListType files = *base_fs.ListDirectory();
#else
	DirectoryTree::DirectoryListType* files = base_fs.ListDirectory();
#endif

	// Find valid game diectories
#ifndef USE_CUSTOM_FILEBUF
	for (auto& dir : files) {
#else
	for (auto& dir : *files) {
#endif
		assert(!dir.second.name.empty() && "VFS BUG: Empty filename in the folder");

#ifdef EMSCRIPTEN
		if (dir.second.name == "Save") {
			continue;
		}
#endif

		if (EndsWith(dir.second.name, ".save")) {
			continue;
		}
		if (dir.second.type == DirectoryTree::FileType::Regular) {
			if (FileFinder::IsSupportedArchiveExtension(dir.second.name)) {
				// The type is only determined on platforms with fast file IO (Windows and UNIX systems)
				// A platform is considered "fast" when it does not require our custom IO buffer
#ifndef USE_CUSTOM_FILEBUF
				auto fs = base_fs.Create(dir.second.name);
				game_entries.push_back({ dir.second.name, FileFinder::GetProjectType(fs) });
#else
				game_entries.push_back({ dir.second.name, FileFinder::ProjectType::Unknown });
#endif
			}
		} else if (dir.second.type == DirectoryTree::FileType::Directory) {
#ifndef USE_CUSTOM_FILEBUF
			auto fs = base_fs.Create(dir.second.name);
			game_entries.push_back({ dir.second.name, FileFinder::GetProjectType(fs) });
#else
			game_entries.push_back({ dir.second.name, FileFinder::ProjectType::Unknown });
#endif
		}
	}

	// Sort game list in place
	std::sort(game_entries.begin(), game_entries.end(),
			  [](const FileFinder::GameEntry &ge1, const FileFinder::GameEntry &ge2) {
				  return strcmp(Utils::LowerCase(ge1.dir_name).c_str(),
								Utils::LowerCase(ge2.dir_name).c_str()) <= 0;
			  });

	if (show_dotdot) {
		game_entries.insert(game_entries.begin(), { "..", FileFinder::ProjectType::Unknown });
	}

	if (HasValidEntry()) {
		item_max = game_entries.size();

		CreateContents();

		contents->Clear();

		for (int i = 0; i < item_max; ++i) {
			DrawItem(i);
		}
	}
	else {
		item_max = 1;

		SetContents(Bitmap::Create(width - 16, height - 16));

		if (show_dotdot) {
			DrawItem(0);
		}

		DrawErrorText(show_dotdot);
	}

	return true;
}

void Window_GameList::DrawItem(int index) {
	Rect rect = GetItemRect(index);
	contents->ClearRect(rect);

	auto& ge = game_entries[index];

#ifndef USE_CUSTOM_FILEBUF
	auto color = Font::ColorDefault;
	if (ge.type == FileFinder::Unknown) {
		color = Font::ColorHeal;
	} else if (ge.type > FileFinder::ProjectType::Supported) {
		color = Font::ColorKnockout;
	}
#else
	auto color = Font::ColorDefault;
#endif

	contents->TextDraw(rect.x, rect.y, color, ge.dir_name);

	if (ge.type > FileFinder::ProjectType::Supported) {
		auto notice = fmt::format("{}", FileFinder::kProjectType.tag(ge.type));
		contents->TextDraw(rect.width, rect.y, color, notice, Text::AlignRight);
	}
}

void Window_GameList::DrawErrorText(bool show_dotdot) {
	std::vector<std::string> error_msg = {
#ifdef EMSCRIPTEN
		"Bạn có nhập sai URL không?",
		"",
		"Nếu bạn nghĩ đây là lỗi, hãy liên hệ cho chủ sở",
		"hửu của website này.",
		"",
		"Thông tin thêm: không thể tìm thấy tệp index.json.",
		"",
		"Hãy chắc chắn qua công cụ phát triển trên trình",
		"duyệt của bạn rằng các tệp tin đã ở đúng vị trí.",
#else
		"Với EasyRPG Player bạn có thể chơi các trò chơi",
		"được tạo bằng RPG Maker 2000 và RPG Maker 2003.",
		"",
		"Các trò chơi này có một tệp tin RPG_RT.ldb và",
		"chúng có thể được giải nén hoặc ở tệp nén ZIP.",
		"",
		"Các nền tảng mới hơn như RPG Maker XP, VX, MV và",
		"MZ không được hỗ trợ."
#endif
	};

	int y = (show_dotdot ? 4 + 14 : 0);

#ifdef EMSCRIPTEN
	contents->TextDraw(0, y, Font::ColorKnockout, "Không thể tìm thấy trò chơi.");
#else
	contents->TextDraw(0, y, Font::ColorKnockout, "Không có trò chơi nào ở thư mục hiện tại.");
#endif

	y += 14 * 2;
	for (size_t i = 0; i < error_msg.size(); ++i) {
		contents->TextDraw(0, y + 14 * i, Font::ColorCritical, error_msg[i]);
	}
}

bool Window_GameList::HasValidEntry() {
	size_t minval = show_dotdot ? 1 : 0;
	return game_entries.size() > minval;
}

FileFinder::FsEntry Window_GameList::GetFilesystemEntry() const {
	const auto& entry = game_entries[GetIndex()];
	return { base_fs.Create(entry.dir_name), entry.type };
}
