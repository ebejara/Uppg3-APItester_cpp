#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <iostream>


using json = nlohmann::json;

const std::string API_URL = "https://fakestoreapi.com/products";

/*inline std::string project_root_path() {
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
}*/

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

    
};


