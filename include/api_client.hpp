#pragma once
#include <cpr/cpr.h>
#include <string>
#include <iostream>

/*Definitions*/
//Classes
class ApiClient; // Forward declaration

//Functions
std::string project_root_path(); // Forward declaration
cpr::Response call_api(); // Forward declaration

//Variables and constants
const std::string api_url = "https://fakestoreapi.com/products"; // API endpoint

/*================================ Body of code ================================*/
/* Wrapper class for CPR to allow mocking in tests */
class ApiClient {
public:
    virtual ~ApiClient() = default;
    virtual cpr::Response get(const std::string& url) {
        return cpr::Get(cpr::Url{url});
    }
};


    

 /*Defining  project root path relative to the .exe folder.
 Path may differ between local host and GitHub Actions runner*/
   std::string project_root_path (){
        std::filesystem::path exe_path = std::filesystem::current_path(); // Path to the executable
        std::filesystem::path project_root;
        if (std::getenv("GITHUB_ACTIONS") != nullptr) { // In CI environment
            project_root = exe_path.parent_path().parent_path();  // Goes up two steps from output/Release
        } else {// in local environment
            project_root = exe_path.parent_path();  // Goes up one step from output
        }
        return project_root.string();

   }

   /*Function to call API and return response.
     Shall be unit tested and Integration tested */
    cpr::Response call_api(){
        ApiClient client;
        cpr::Response r = client.get(api_url);
        if ( r.status_code != 200){
            std::cout << "call_api(): API call failed with status: " << r.status_code << std::endl;
        }
        else{
            std::cout << "call_api(): API call successful. Code: " << r.status_code << std::endl;   
        }
        return r; 
    }