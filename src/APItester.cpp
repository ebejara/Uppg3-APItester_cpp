#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include "Apitester.hpp"
#include <nlohmann/json.hpp>

/* This fiel will try to fetch data from https://fakestoreapi.com/products
   Usually thsi works on a local computer but it seems that the API is blocking calls from
   CI environments like Github Actions. For that purpose the main functions purpose si to create a
   JSON file with the data from the API. In case the calls to the API fail, then teh file will be fetched from
   an ofline copy that was previously fetcehd from the API on a local environment computer */

using json = nlohmann::json;

bool products_file_loaded = false;


int main() {

    CurlGlobalGuard curl_guard;  // Initieras automatiskt här, cleanup vid exit
    /*Defining Path to /src. relative to folder where .exe built. 
    Path may differ between running on local client computer  vs. 
    running on Github Actions runner. This is so exeutable file can find the
    JSON files in the src directory which mirror teh APIc content */
  
    std::string src_dir = project_root_path () + "/src/"; // Path to src folder
    std::string product_file = src_dir + "products.json";   // File to save products from API
    std::string fallback_file = src_dir + "products_prefetched.json"; //File to use if API fails
   
    json products; //define JSON object to hold products
    ApiClient client;
    // 
    spdlog::info("Starting API fetch from fakestoreapi.com...");   
    /*This function: call_api(),
     is tested in test section with actual call and simulated responces*/
     spdlog::info("APItester.cpp: Attempting call to actual API.");
    
     products = client.call_api(API_URL); // Make GET request to API
     
     //auto [status, body] = client.http_get_with_headers();  // Default URL
     //products = json::parse(body);
     if (products.is_null()) {
        spdlog::warn("API call returned null data. Falling back to predefined file.");
        std::ifstream backupfile(fallback_file);
        if (!backupfile.is_open()) {
            spdlog::error("Failed to open {} for writing. Check path.", fallback_file);
            return 1;
        }
        backupfile >> products; //Load backup file (instead of API answer) to JSON object
        spdlog::info("Successfully loaded fallback JSON from {}", fallback_file);
    }
    
    std::ofstream file(product_file); //Open/create file to save products
    if (!file.is_open()) {
        spdlog::error("Failed to create/open {} for writing. Check path.", product_file);
        return 1;
    }
    file << products.dump(4); //Save parsed JSON data to product file
    file.close();
    spdlog::info("Successfully saved products to file {}.", product_file);

        
    // Print product titles and prices to console
    /*std::cout << "Antal produkter: " << products.size() << std::endl;
    for (const auto& p : products) {
        std::cout << "- " << p["title"] << " (" << p["price"] << " USD)" << std::endl;
    }*/

   

    return 0;
}
