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

namespace Speedrun {

    int PingIntervalId = 0;

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
            FS.syncfs(false, function (err) {
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

    NobihazaVN::ApiResponse StartGame() {
        NobihazaVN::UserToken user = NobihazaVN::GetUserToken();
        return NobihazaVN::RequestSync("/start?username=" + user.username + "&token=" + user.token, "GET", true, {});
    }

    NobihazaVN::ApiResponse Continue() {
        NobihazaVN::UserToken user = NobihazaVN::GetUserToken();
        return NobihazaVN::RequestSync("/continue?username=" + user.username + "&token=" + user.token, "GET", true, {});
    }

    void Ping(void*) {
        NobihazaVN::UserToken user = NobihazaVN::GetUserToken();
        NobihazaVN::Request("/ping?username=" + user.username + "&token=" + user.token, "GET", true, {}, [](NobihazaVN::ApiResponse res) {
            if (!res.success) Output::Error(res.message);
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

}

#endif