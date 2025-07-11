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
#ifndef NBHZVN_API_H
#define NBHZVN_API_H

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <emscripten.h>
#include <emscripten/fetch.h>

namespace NobihazaVN {

    struct ApiResponse {
        bool success;
        int status_code;
        std::string message;
        nlohmann::json data;
    };

    struct UserToken {
        std::string username;
        std::string token;
    };

    extern UserToken CurrentToken;

    struct User {
        int id;
        std::string username;
        std::string display_name;
    };

    struct FetchContext {
        std::function<void(ApiResponse)> callback;
        std::string* body;
    };
    
    template <typename T>
    T SafeGet(const nlohmann::json& j, const std::string& key, T default_value) {
        if (j.contains(key) && !j[key].is_null())
            return j[key].get<T>();
        return default_value;
    }

    UserToken GetUserToken();
    void SetCurrentToken(UserToken token);

    User ParseUser(const nlohmann::json& j);

    void OnSuccess(emscripten_fetch_t* fetch);
    void OnError(emscripten_fetch_t* fetch);

    void Request(const std::string& endpoint, const std::string& method, bool authentication, const nlohmann::json& data, std::function<void(ApiResponse)> callback);
    ApiResponse RequestSync(const std::string& endpoint, const std::string& method, bool authentication, const nlohmann::json& data);

}

#endif
#endif