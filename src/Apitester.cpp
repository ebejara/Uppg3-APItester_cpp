#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include "api_client.hpp"
#include <nlohmann/json.hpp>

/* This fiel will try to fetch data from https://fakestoreapi.com/products
   Usually thsi works on a local computer but it seems that the API is blocking calls from
   CI environments like Github Actions. For that purpose the main functions purpose si to create a
   JSON file with the data from the API. In case the calls to the API fail, then teh file will be fetched from
   an ofline copy that was previously fetcehd from the API on a local environment computer */

using json = nlohmann::json;

bool products_file_loaded = false;


int main() {
    /*Defining Path to /src. relative to folder where .exe built. 
    Path may differ between running on local client computer  vs. 
    running on Github Actions runner. This is so exeutable file can find the
    JSON files in the src directory which mirror teh APIc content */
  
    std::string src_dir = project_root_path () + "/src/"; // Path to src folder
    std::string product_file = src_dir + "products.json";   // File to save products from API
    std::string fallback_file = src_dir + "products_prefetched.json"; //File to use if API fails

    json products; //define JSON object to hold products

    // 
    spdlog::info("Starting API fetch from fakestoreapi.com...");   
    /*This function: call_api(),
     is tested in test section with actual call and simulated responces*/
    cpr::Response r = call_api(); // Make GET request to API
    
    /* Code below is for establishing products database from API. */
    spdlog::info("Attempting call to actual API.");
    if (r.status_code == 200) { // Successful API call
        spdlog::info("API call successful (status 200). Using fresh data from API.");
        try {// Parse JSON response so that it is valid & readable
            products = json::parse(r.text);
            std::ofstream file(product_file);
            if (!file.is_open()) {
                spdlog::error("Failed to open {} for writing. Check path.", product_file);
                return 1;
            }
            file << products.dump(4); //Save parsed JSON data to file with indentation
            file.close();
            spdlog::info("Updated product.json file with parsed data from API.");
            products_file_loaded = true;
        } catch (const json::parse_error& e) {
            spdlog::error("Failed to parse fresh JSON: {}", e.what());
            return 1;
        }
    } else { // API call failed
        spdlog::warn("API call failed (status {}). Falling back to predefined file.", r.status_code);
        // Läs in fördefinierad fil
        std::ifstream backupfile(fallback_file);
        if (!backupfile.is_open()) {
            spdlog::error("Could not open fallback file: {}", fallback_file);
            std::cout << "Kritiskt fel: Ingen JSON-data tillgänglig!" << std::endl;
            return 1;
        }
        try {
            backupfile >> products; //Load  backup file to JSON object for validation
            spdlog::info("Successfully loaded fallback JSON from {}", fallback_file);
            std::ofstream file (product_file);
            if (!file.is_open()) {
                spdlog::error("Failed to open {} for writing. Check path", product_file);
            return 1;
            }
            file << products.dump(4); //Save validated & parsed fallback data to products.json
            file.close();
            spdlog::info("Updated product.json file with data from backup_file.");
            products_file_loaded = true;

        } catch (const json::parse_error& e) {
            spdlog::error("Failed to parse fallback JSON: {}", e.what());
            std::cout << "Kritiskt fel: Ingen JSON-data tillgänglig!" << std::endl;
            return 1;
        }
    }

    // Print product titles and prices to console
    std::cout << "Antal produkter: " << products.size() << std::endl;
    for (const auto& p : products) {
        std::cout << "- " << p["title"] << " (" << p["price"] << " USD)" << std::endl;
    }

    return 0;
}