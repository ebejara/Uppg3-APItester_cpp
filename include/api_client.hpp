#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>
#include <filesystem>

using json = nlohmann::json;

// Konstant för API-URL
const std::string API_URL = "https://fakestoreapi.com/products";

// Hjälpfunktion för att hitta projektroten (används i APItester.cpp)
inline std::string project_root_path() {
    std::filesystem::path exe_path = std::filesystem::current_path();
    std::filesystem::path project_root;

    if (std::getenv("GITHUB_ACTIONS") != nullptr) {
        // I GitHub Actions ligger exe:n i output/Release
        std::cout << "Detected GitHub Actions environment." << std::endl;
        project_root = exe_path.parent_path().parent_path();
    } else {
        // Lokalt ligger exe:n i output
        project_root = exe_path.parent_path();
    }
    return project_root.string();
}

// Wrapper-klass för att kunna mocka API-anrop
class ApiClient {
public:
    virtual ~ApiClient() = default;
    virtual cpr::Response get(const std::string& url = API_URL){
        return cpr::Get(cpr::Url{url});
    }
    // Den gemensamma funktionen som både program och tester använder
    virtual json call_api(const std::string& url = API_URL) {
        cpr::Response r = cpr::Get(cpr::Url{url});

        if (r.status_code == 200) {
            try {
                return json::parse(r.text);
                //return nullptr; // Temporary return to avoid parse error in CI
            } catch (const json::parse_error& e) {
                throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
            }
        } else {
            throw std::runtime_error("API call failed with status: " + std::to_string(r.status_code));
        }
    }
};