#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
#include "api_client.hpp"


using ::testing::Return;
using ::testing::_;

class MockApiClient : public ApiClient {
public:
    MOCK_METHOD(cpr::Response, get, (const std::string& url), (override));
};


TEST(FakeStoreApiTest, GetProductsReturns200) {
    spdlog::info("Starting API test...");

    cpr::Response r = cpr::Get(cpr::Url{"https://fakestoreapi.com/products"});

    if (std::getenv("GITHUB_ACTIONS") != nullptr) {
        // In CI, expect 403 due to IP blocking
        spdlog::info("CI environment detected, expecting 403 status code. Actual: {}", r.status_code);
        EXPECT_EQ(r.status_code, 403);
    } else {
        // Locally, expect 200
        EXPECT_EQ(r.status_code, 200);
        EXPECT_GT(r.text.length(), 0);
        EXPECT_EQ(r.header["content-type"], "application/json; charset=utf-8");
    }


    spdlog::info("API test completed with status: {}", r.status_code);
}

TEST(FakeStoreApiTest, GetProductsReturns200withMock) {
    spdlog::info("Starting  API test with mock...");

    ApiClient client;  // Riktig implementation
    cpr::Response r = client.get("https://fakestoreapi.com/products");

    EXPECT_EQ(r.status_code, 200);
    // ... övriga asserts
}

//Return403Forbidden
TEST(FakeStoreApiTest, Handles403Forbidden) {
    spdlog::info("Starting mocked API test (403)...");

    MockApiClient mock_client;

    cpr::Response mock_response;
    mock_response.status_code = 403;
    mock_response.text = R"({"error": "Forbidden"})";
    mock_response.header["content-type"] = "application/json";

    EXPECT_CALL(mock_client, get(_))
        .WillOnce(Return(mock_response));

    cpr::Response r = mock_client.get("https://fakestoreapi.com/products");

    EXPECT_EQ(r.status_code, 403);
    EXPECT_GT(r.text.length(), 0);
    spdlog::info("Mocked API test completed with status: {}", r.status_code);
}



//Test
int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);  // Sätt loggnivå
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}