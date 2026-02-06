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
    
     std::cout << "Status code:          " << r.status_code << "\n";
    std::cout << "Error code:           " << static_cast<int>(r.error.code) << "\n";
    std::cout << "Error message:        " << r.error.message << "\n";
    std::cout << "Response text length: " << r.text.length() << "\n";
    if (!r.text.empty()) {
        std::cout << "Response preview:     " << r.text.substr(0, 200) << "...\n";
    }
    std::cout << "Final URL:            " << r.url << "\n";
    std::cout << "Elapsed time:         " << r.elapsed << " s\n";

    if (r.status_code == 0 && r.error) {
        spdlog::error("Request completely failed - libcurl error");
        throw std::runtime_error("cURL failed before HTTP response: " + r.error.message);
    }





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
             else{
             spdlog::info("APItest response was negative, running from local computer", r.status_code);
             }
            throw std::runtime_error("API call failed with status: " + std::to_string(r.status_code));
        }

    return 0;
}
