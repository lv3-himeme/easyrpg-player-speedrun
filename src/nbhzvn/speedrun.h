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
#ifndef NBHZVN_SPEEDRUN_H
#define NBHZVN_SPEEDRUN_H

#include "api.h"

namespace Speedrun {

    struct Data {
        int id;
        int user_id;
        std::string discord_id;
        std::string os;
        long long start_timestamp;
        long long playtime;
        long long real_playtime;
        int saves;
        int ranking;
        std::string ban_reason;
    };

    struct User {
        NobihazaVN::User user;
        Data speedrun_data;
    };

    Data ParseSpeedrunData(const nlohmann::json& j);

    User ParseUser(const nlohmann::json& j);
    
    void CheckUser(NobihazaVN::UserToken user, std::function<void(User)> callback);

}

#endif
#endif