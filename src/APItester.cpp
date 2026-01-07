#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include "APItester.hpp"
#include <nlohmann/json.hpp>

/* This fiel will try to fetch data from https://fakestoreapi.com/products
   Usually thsi works on a local computer but it seems that the API is blocking calls from
   CI environments like Github Actions. For that purpose the main functions purpose si to create a
   JSON file with the data from the API. In case the calls to the API fail, then the file will be fetched from
   an offline copy that was previously fetcehd from the API on a local environment computer */

using json = nlohmann::json;

bool products_file_loaded = false;


int main() {
   
    json products; //define JSON object to hold products
    ApiClient client;
    // 
    spdlog::info("Starting API fetch from fakestoreapi.com...");   
    spdlog::info("APItester.cpp: Attempting call to actual API.");
    
     //products = client.call_api(API_URL); // Make GET request to API
     cpr::Response r = client.get(API_URL);
    if (r.status_code == 200) {
        spdlog::info("APItest response was succesful. Status code {}.", r.status_code);
        try {
            products = json::parse(r.text);
            std::cout << "Antal produkter: " << products.size() << std::endl;
            for (const auto& p : products) {
                std::cout << "- " <<"Id " << p["id"] << " " << p["title"] << " (" << p["price"] << " USD)" << std::endl;
            }

        } catch (const json::parse_error& e) {
            throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
        }
        } else {
            if (std::getenv("GITHUB_ACTIONS") != nullptr) {
             spdlog::info("APItest response was negative probably due to running from GitHub Actions.", r.status_code);}
            throw std::runtime_error("API call failed with status: " + std::to_string(r.status_code));
        }

    return 0;
}
