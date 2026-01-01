#pragma once
#include <cpr/cpr.h>

//Gör en wrapper så att cpr kan Mockas.

class ApiClient {
public:
    virtual ~ApiClient() = default;
    virtual cpr::Response get(const std::string& url) {
        return cpr::Get(cpr::Url{url});
    }
};