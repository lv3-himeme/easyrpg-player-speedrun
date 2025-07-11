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
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstdio>

namespace Speedrun {

    extern int PLAYTIME_HOURS_VARIABLE;
    extern int PLAYTIME_MINUTES_VARIABLE;
    extern int PLAYTIME_SECONDS_VARIABLE;
    extern int RANKING_VARIABLE;
    extern int SAVES_VARIABLE;
    extern int COMPLETED_SWITCH;
    extern int REDIRECT_SWITCH;
    extern std::string REDIRECT_URL;

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

    extern User CurrentUser;

    void DeleteRecursive(const std::string& path);
    void DeleteSave();

    Data ParseSpeedrunData(const nlohmann::json& j);

    User ParseUser(const nlohmann::json& j);
    void SetCurrentUser(User user);
    
    void CheckUser(NobihazaVN::UserToken user, std::function<void(User)> callback);
    NobihazaVN::ApiResponse StartGame();
    NobihazaVN::ApiResponse Continue();
    void Ping(void*);
    void StartPing();
    void StopPing();

    int32_t GetPlaytime();

    void Complete();

}

#endif
#endif