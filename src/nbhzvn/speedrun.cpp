/*
 * Copyright (C) 2025 Serena1432
 *
 * This file is originally intended to be used for Nobihaza Vietnam
 * Community website (nbhzvn.one) integration.
 *
 * You can use it for any other projects, but you have to edit this
 * file manually to make it compatible with your projects.
 *
 * This file is licensed with the same GPLv3 license as the EasyRPG
 * Player itself. See <http://www.gnu.org/licenses/> for more
 * information.
 *
 * Currently, only Emscripten is supported.
 */

#ifdef EMSCRIPTEN

#include "speedrun.h"
#include "output.h"
#include "player.h"
#include "main_data.h"
#include "game_variables.h"

namespace Speedrun {

    int PingIntervalId = 0;

    int PLAYTIME_HOURS_VARIABLE = 91;
    int PLAYTIME_MINUTES_VARIABLE = 92;
    int PLAYTIME_SECONDS_VARIABLE = 285;
    int RANKING_VARIABLE = 117;
    int SAVES_VARIABLE = 93;
    int COMPLETED_SWITCH = 1134;
    int REDIRECT_SWITCH = 1135;
    std::string REDIRECT_URL = "https://speedrun.nbhzvn.one/completed";

    User CurrentUser;

    Data ParseSpeedrunData(const nlohmann::json& j) {
        Data d;
        d.id = NobihazaVN::SafeGet<int>(j, "id", 0);
        d.user_id = NobihazaVN::SafeGet<int>(j, "user_id", 0);
        d.discord_id = NobihazaVN::SafeGet<std::string>(j, "discord_id", "");
        d.os = NobihazaVN::SafeGet<std::string>(j, "os", "");
        d.start_timestamp = NobihazaVN::SafeGet<long long>(j, "start_timestamp", 0);
        d.playtime = NobihazaVN::SafeGet<long long>(j, "playtime", 0);
        d.real_playtime = NobihazaVN::SafeGet<long long>(j, "real_playtime", 0);
        d.saves = NobihazaVN::SafeGet<int>(j, "saves", 0);
        d.ranking = NobihazaVN::SafeGet<int>(j, "ranking", 0);
        d.ban_reason = NobihazaVN::SafeGet<std::string>(j, "ban_reason", "");
        return d;
    }
    
    void DeleteRecursive(const std::string& path) {
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return;
        if (S_ISDIR(st.st_mode)) {
            DIR* dir = opendir(path.c_str());
            if (!dir) return;
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name == "." || name == "..") continue;

                std::string fullPath = path + "/" + name;
                DeleteRecursive(fullPath);
            }
            closedir(dir);
            rmdir(path.c_str());
        } else {
            unlink(path.c_str());
        }
    }

    void DeleteSave() {
        DeleteRecursive("/easyrpg/Save");
        EM_ASM({
            FS.syncfs(function(err) {
                if (err) console.error("Sync failed after deletion:", err);
                else console.log("Save folder deleted and changes synced to IndexedDB.");
            });
        });
    }

    User ParseUser(const nlohmann::json& j) {
        User u;
        u.user = NobihazaVN::ParseUser(NobihazaVN::SafeGet<nlohmann::json>(j, "user", nlohmann::json{}));
        u.speedrun_data = ParseSpeedrunData(NobihazaVN::SafeGet<nlohmann::json>(j, "speedrun_data", nlohmann::json{}));
        return u;
    }

    void CheckUser(NobihazaVN::UserToken user, std::function<void(User)> callback) {
        NobihazaVN::Request("/user?username=" + user.username + "&token=" + user.token, "GET", true, {}, [callback](NobihazaVN::ApiResponse res) {
            if (res.success) callback(ParseUser(res.data));
            else {
                Output::Error(res.message);
                callback(User{});
            }
        });
    }

    void SetCurrentUser(User user) {
        CurrentUser = user;
    }

    NobihazaVN::ApiResponse StartGame() {
        NobihazaVN::UserToken& user = NobihazaVN::CurrentToken;
        return NobihazaVN::RequestSync("/start?username=" + user.username + "&token=" + user.token, "GET", true, {});
    }

    NobihazaVN::ApiResponse Continue() {
        NobihazaVN::UserToken& user = NobihazaVN::CurrentToken;
        return NobihazaVN::RequestSync("/continue?username=" + user.username + "&token=" + user.token, "GET", true, {});
    }

    void Ping(void*) {
        NobihazaVN::UserToken& user = NobihazaVN::CurrentToken;
        NobihazaVN::Request("/ping?username=" + user.username + "&token=" + user.token, "GET", true, {}, [](NobihazaVN::ApiResponse res) {
            if (!res.success) {
                if (!Player::Paused) {
                    if (res.status_code == 0) res.message += " Bạn sẽ được tiếp tục chơi khi đã kết nối lại thành công.";
                    Player::Pause();
                    EM_ASM({
                        Speedrun.stopPlaytime = true;
                        Speedrun.pause(UTF8ToString($0));
                    }, res.message.c_str());
                }
            }
            else {
                Player::Resume();
                EM_ASM({
                    Speedrun.stopPlaytime = false;
                    Speedrun.resume($0);
                }, GetPlaytime());
            }
        });
    }

    void StartPing() {
        if (PingIntervalId <= 0) PingIntervalId = emscripten_set_interval(Ping, 5000, nullptr);
    }

    void StopPing() {
        if (PingIntervalId > 0) {
            emscripten_clear_interval(PingIntervalId);
            PingIntervalId = 0;
        }
    }

    int32_t GetPlaytime() {
        int32_t hours = Main_Data::game_variables->Get(PLAYTIME_HOURS_VARIABLE);
        int32_t minutes = Main_Data::game_variables->Get(PLAYTIME_MINUTES_VARIABLE);
        int32_t seconds = Main_Data::game_variables->Get(PLAYTIME_SECONDS_VARIABLE);
        return (hours * 3600) + (minutes * 60) + seconds;
    }

    void Complete() {
        User& speedrunUser = CurrentUser;
        nlohmann::json j;
        j["user_id"] = speedrunUser.user.id;
        j["playtime"] = GetPlaytime();
        j["saves"] = Main_Data::game_variables->Get(SAVES_VARIABLE);
        j["ranking"] = Main_Data::game_variables->Get(RANKING_VARIABLE);
        NobihazaVN::Request("/submit", "POST", true, j, [](NobihazaVN::ApiResponse res) {
            if (res.success) Output::Info(res.message);
            else Output::Error("Đã xảy ra lỗi khi tải lên phần chơi của bạn:\n{}\n\nHãy khởi động lại game và tải lại save để tiến hành gửi lại.", res.message);
        });
    }

    extern "C" {
        EMSCRIPTEN_KEEPALIVE
        void AltTabMute() {
            User& speedrunUser = CurrentUser;
            nlohmann::json j;
            j["user_id"] = speedrunUser.user.id;
            j["ban_reason"] = "Chuyển đổi cửa sổ quá nhiều lần";
            NobihazaVN::Request("/ban", "POST", true, j, [](NobihazaVN::ApiResponse res) {});
        }
    }

}

#endif