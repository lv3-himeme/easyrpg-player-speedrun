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

#include "api.h"

const std::string ApiUrl = std::string(NBHZVN_API_URL);
const std::string ApiToken = std::string(NBHZVN_API_TOKEN);

namespace NobihazaVN {
    
    UserToken CurrentToken;

    EM_JS(const char*, get_cookie, (const char* name), {
        const cookies = document.cookie.split("; ");
        for (let c of cookies) {
            const [k, v] = c.split("=");
            if (k === UTF8ToString(name)) {
                const len = lengthBytesUTF8(v) + 1;
                const buffer = _malloc(len);
                stringToUTF8(v, buffer, len);
                return buffer;
            }
        }
        const buffer = _malloc(1);
        stringToUTF8("", buffer, 1);
        return buffer;
    });

    User ParseUser(const nlohmann::json& j) {
        User u;
        u.id = SafeGet<int>(j, "id", 0);
        u.username = SafeGet<std::string>(j, "username", 0);
        u.display_name = SafeGet<std::string>(j, "display_name", 0);
        return u;
    }

    void OnSuccess(emscripten_fetch_t* fetch) {
        auto ctx = static_cast<FetchContext*>(fetch->userData);

        ApiResponse res;
        auto json_str = std::string(fetch->data, fetch->numBytes);
        nlohmann::json json = nlohmann::json::parse(json_str, nullptr, false);
        if (json.is_discarded()) {
            res.success = false;
            res.status_code = fetch->status;
            res.message = "Không thể phân tích dữ liệu JSON từ máy chủ Nobihaza Vietnam Community.";
            res.data = nlohmann::json{};
        } else {
            res.success = json.value("success", false);
            res.status_code = json.value("status_code", fetch->status);
            res.message = json.value("message", "Không có thông báo.");
            res.data = json.value("data", nlohmann::json{});
        }

        ctx->callback(res);
        delete ctx->body;
        delete ctx;
        emscripten_fetch_close(fetch);
    }

    void OnError(emscripten_fetch_t* fetch) {
        auto ctx = static_cast<FetchContext*>(fetch->userData);

        NobihazaVN::ApiResponse res;
        res.success = false;
        res.status_code = fetch->status;
        res.message = "Không thể kết nối tới máy chủ của Nobihaza Vietnam Community. Vui lòng thử lại.";
        auto json_str = std::string(fetch->data, fetch->numBytes);
        nlohmann::json json = nlohmann::json::parse(json_str, nullptr, false);
        if (!json.is_discarded()) {
            res.success = json.value("success", false);
            res.status_code = json.value("status_code", fetch->status);
            res.message = json.value("message", "Không thể kết nối tới máy chủ của Nobihaza Vietnam Community. Vui lòng thử lại.");
            res.data = json.value("data", nlohmann::json{});
        }

        ctx->callback(res);
        delete ctx->body;
        delete ctx;
        emscripten_fetch_close(fetch);
    }

    void Request(const std::string& endpoint, const std::string& method, bool authentication, const nlohmann::json& data, std::function<void(ApiResponse)> callback) {
        std::string url = ApiUrl + endpoint;
        std::string token = ApiToken;
        std::string* bodyPtr = nullptr;
        if (method != "GET") {
            bodyPtr = new std::string(data.dump());
        }

        auto ctx = new FetchContext{callback, bodyPtr};

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, method.c_str());
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = OnSuccess;
        attr.onerror = OnError;
        attr.userData = ctx;

        if (method != "GET") {
            attr.requestData = ctx->body->c_str();
            attr.requestDataSize = ctx->body->size();
        }

        static std::string authHeader;
        authHeader = token;
        const char* headers_with_auth[] = {
            "Content-Type", "application/json",
            "Authorization", authHeader.c_str(),
            nullptr
        };
        const char* headers_no_auth[] = {
            "Content-Type", "application/json",
            nullptr
        };
        attr.requestHeaders = authentication ? headers_with_auth : headers_no_auth;

        emscripten_fetch(&attr, url.c_str());
    }

    ApiResponse RequestSync(const std::string& endpoint, const std::string& method, bool authentication, const nlohmann::json& data) {
        NobihazaVN::ApiResponse response;
        bool done = false;
        Request(endpoint, method, authentication, data, [&](ApiResponse res) {
            response = res;
            done = true;
        });
        while (!done) {
            emscripten_sleep(10);
        }
        return response;
    }

    UserToken GetUserToken() {
        const char* user = get_cookie("nbhzvn_username");
        const char* tok = get_cookie("nbhzvn_login_token");

        UserToken t;
        t.username = user;
        t.token = tok;

        free((void*)user);
        free((void*)tok);
        return t;
    }

    void SetCurrentToken(UserToken token) {
        CurrentToken = token;
    }

}

#endif