#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include "api_client.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    spdlog::info("Starting API fetch from fakestoreapi.com...");

    const std::string api_url = "https://fakestoreapi.com/products";
    const std::string fallback_file = "../src/products_prefetched.json";
    const std::string product_file = "../src/products.json";

    json products;

    // Försök hämta från API först
    ApiClient client;
    cpr::Response r = client.get(api_url);

    if (r.status_code == 200) {
        spdlog::info("API call successful (status 200). Using fresh data.");
        // Spara rå JSON
        

        // Spara formaterad version
        try {
            products = json::parse(r.text);
            std::ofstream file(product_file);
            if (!file.is_open()) {
                spdlog::error("Failed to open {} for writing. Check path.", product_file);
                return 1;
            }
            file << products.dump(4);
            file.close();
            spdlog::info("Updated both product file with data from API.");
        } catch (const json::parse_error& e) {
            spdlog::error("Failed to parse fresh JSON: {}", e.what());
            return 1;
        }
    } else {
        spdlog::warn("API call failed (status {}). Falling back to predefined file.", r.status_code);
        // Läs in fördefinierad fil
        std::ifstream backupfile(fallback_file);
        if (!backupfile.is_open()) {
            spdlog::error("Could not open fallback file: {}", fallback_file);
            std::cout << "Kritiskt fel: Ingen JSON-data tillgänglig!" << std::endl;
            return 1;
        }
        try {
            backupfile >> products; 
            spdlog::info("Successfully loaded fallback JSON from {}", fallback_file);
            products = json::parse(r.text); // se till att filen innehåller giltig JSON formatering
            std::ofstream file  (product_file);
            if (!file.is_open()) {
                spdlog::error("Failed to open {} for writing. Does the 'src' folder exist in current directory?", product_file);
            return 1;
            }
            file << products.dump(4);
            file.close();
            spdlog::info("Updated both product file with data from backup_file.");


        } catch (const json::parse_error& e) {
            spdlog::error("Failed to parse fallback JSON: {}", e.what());
            return 1;
        }
    }

    // Nu har du products som json-objekt – du kan använda det vidare!
    std::cout << "Antal produkter: " << products.size() << std::endl;
    for (const auto& p : products) {
        std::cout << "- " << p["title"] << " (" << p["price"] << " USD)" << std::endl;
    }

    return 0;
}