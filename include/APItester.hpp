#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <iostream>


using json = nlohmann::json;

// Constant string used for API URL
const std::string API_URL = "https://fakestoreapi.com/products";

class ApiClient {
public:
    virtual ~ApiClient() = default;

    // CPR-based method to perform GET request - fails with 403 in GitHub Actions
    //Wrapps cpr::Get to allow mocking in tests, but not used anymore in main application or test
    virtual cpr::Response get(const std::string& url = API_URL) {
        return cpr::Get(cpr::Url{url});
    }
    // Method to call the API and parse JSON response. If call fails or parsing fails, throws an exception.
 /*   virtual json call_api(const std::string& url = API_URL) {
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
    }*/

    
};


