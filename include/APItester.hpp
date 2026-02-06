#pragma once

#include <cpr/cpr.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <iostream>


using json = nlohmann::json;

const std::string API_URL = "https://fakestoreapi.com/products";

class ApiClient {
public:
    virtual ~ApiClient() = default;

    // CPR-based method to perform GET request - fails with 403 in GitHub Actions
    virtual cpr::Response get(const std::string& url = API_URL) {
    cpr::Session session;
    session.SetUrl(cpr::Url{url});

    // ← This is usually enough to bypass most Windows Schannel revocation issues
        session.SetVerifySsl(cpr::VerifySsl(false));   // or: cpr::VerifySsl{false}

    // Alternative syntax if the above doesn't work (older cpr versions):
    // session.SetOption(cpr::VerifySsl{false});  // ← disables ALL verification (less secure, only temp)

    return session.Get();
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

    
};


