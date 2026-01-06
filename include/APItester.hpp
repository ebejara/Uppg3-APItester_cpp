#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <curl/curl.h>
#include <utility>

using json = nlohmann::json;

const std::string API_URL = "https://fakestoreapi.com/products";

inline std::string project_root_path() {
    std::filesystem::path exe_path = std::filesystem::current_path();
    std::filesystem::path project_root;

    if (std::getenv("GITHUB_ACTIONS") != nullptr) {
        std::cout << "Apitester.hpp: project_root_path(): Detected GitHub Actions environment." << std::endl;
        project_root = exe_path.parent_path().parent_path();
    } else {
        project_root = exe_path.parent_path();
    }
    std::cout << "Apitester.hpp: project_root_path(): Project root path: " << project_root.string() << std::endl;
    return project_root.string();
}

class ApiClient {
public:
    virtual ~ApiClient() = default;

    // CPR-based method to perform GET request - fails with 403 in GitHub Actions
    virtual cpr::Response get(const std::string& url = API_URL) {
        return cpr::Get(cpr::Url{url});
    }

    virtual json call_api(const std::string& url = API_URL) {
        cpr::Response r = get(url);
        if (r.status_code == 200) {
            try {
                return json::parse(r.text);
            } catch (const json::parse_error& e) {
                throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
            }
        } else {
            throw std::runtime_error("API call failed with status: " + std::to_string(r.status_code));
        }
    }

    // New methood using libcurl with custom headers to avoid 403 in GitHub Actions
    virtual std::pair<long, std::string> http_get_with_headers(const std::string& url = API_URL) {
    CURL* curl = curl_easy_init();
    std::string response_body;
    long status_code = 0;

    if (!curl) {
        return {0, ""};
    }

    // Headers för att undvika 403 från Cloudflare
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36");
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.9");
    headers = curl_slist_append(headers, "Connection: keep-alive");

    // VIKTIGA timeout-inställningar – dessa tre tillsammans är säkra
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);               // Max total tid: 10 sek
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);         // Max tid för att connecta: 5 sek
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);               // Viktigt för timeouts i multitrådade program

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, std::string* s) -> size_t {
        s->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);         // Följ redirects

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    } else {
        // Logga felmeddelande – viktigt för debugging!
        std::cerr << "curl error (" << res << "): " << curl_easy_strerror(res) << std::endl;
        status_code = 0;  // Eller -1 för att markera fel
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return {status_code, response_body};
}
};

class CurlGlobalGuard {
public:
    CurlGlobalGuard() {
        CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
        if (res != CURLE_OK) {
            throw std::runtime_error("curl_global_init failed: " + std::string(curl_easy_strerror(res)));
        }
    }
    ~CurlGlobalGuard() {
        curl_global_cleanup();
    }
    // Förbjud kopiering
    CurlGlobalGuard(const CurlGlobalGuard&) = delete;
    CurlGlobalGuard& operator=(const CurlGlobalGuard&) = delete;
};