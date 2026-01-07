#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include "APItester.hpp"
#include <nlohmann/json.hpp>

/***************************************************************************************************** * 
*This file will try to fetch data from https://fakestoreapi.com/products                               *
* Usually this works on a local computer but it seems that the API is blocking calls from              *
* CI environments like Github Actions in which case the main() application will exit throwing an error *
********************************************************************************************************/

using json = nlohmann::json;


int main() {
   
    json products; //define JSON object to hold products
    ApiClient client;
    //  
    spdlog::info("APItester.cpp: Attempting call to actual API. If succesful, products will be listed in console.");
    
     cpr::Response r = client.get(API_URL);
    if (r.status_code == 200) {
        spdlog::info("APItest response was succesful. Status code {}.", r.status_code);
        try {
            products = json::parse(r.text);
            std::cout << "Number of products: " << products.size() << std::endl;
            for (const auto& p : products) {
                std::cout << "- " <<"Id " << p["id"] << " " << p["title"] << " (" << p["price"] << " USD)" << std::endl;
            }

        } catch (const json::parse_error& e) {
            spdlog::error("Failed to parse JSON response.");
            throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
        }
        } else {
            if (std::getenv("GITHUB_ACTIONS") != nullptr) {
             spdlog::info("APItest response was negative probably due to running from GitHub Actions.", r.status_code);}
            throw std::runtime_error("API call failed with status: " + std::to_string(r.status_code));
        }

    return 0;
}
