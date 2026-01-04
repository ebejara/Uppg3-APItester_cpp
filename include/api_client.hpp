#pragma once
#include <cpr/cpr.h>
#include <string>

class ApiClient {
public:
    virtual ~ApiClient() = default;
    virtual cpr::Response get(const std::string& url) {
        return cpr::Get(cpr::Url{url});
    }
};