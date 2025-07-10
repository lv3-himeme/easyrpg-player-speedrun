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

}

#endif