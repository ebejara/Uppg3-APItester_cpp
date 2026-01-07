#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
#include "APItester.hpp" // Wrapper class for API calls

using ::testing::Return;
using ::testing::_;

// Mock-klass
class MockApiClient : public ApiClient {
public:
    MOCK_METHOD(json, call_api, (const std::string& url), (override));
    MOCK_METHOD(cpr::Response, get, (const std::string&), (override));
};


/*******************************************************************
*   Unit test: Test that call_api() will throw an error if responce*
*   is not 200.                                                    *
*   Mocks the response to call_api() to be 403 so that call_api()  *
*   will throw an error.                                           *
*******************************************************************/
TEST(ApiClientTest, ThrowsOnHttpErrorStatus) {
    MockApiClient mock;

    // Skapa en fake response med status 403
    cpr::Response fake_response;
    fake_response.status_code = 403;
    fake_response.text = "Forbidden";  // valfritt, används inte vid throw

    // Mocka get()-metoden att returnera fake_response
    EXPECT_CALL(mock, get(_)).WillOnce(Return(fake_response));

    // Testa att call_api kastar rätt undantag
    EXPECT_THROW(
        mock.call_api(API_URL),
        std::runtime_error
    );
}


/*Actual call to API with handling for 403 Nehagtive responce*/
TEST(FakeStoreApiTest, GetProductsReturns200_Or_403_In_CI) {
    spdlog::info("Starting real API test...");

    ApiClient client;  // Använder wrapper → riktig cpr::Get
    cpr::Response r = client.get(API_URL);
    //I CI-miljö kommer anrop till API:t att nekas.
    if (std::getenv("GITHUB_ACTIONS") != nullptr) {
        spdlog::info("CI environment detected – expecting 200 or 403. Actual: {}", r.status_code);
        EXPECT_TRUE(r.status_code == 200 || r.status_code == 403); //Positive if Code is 200 or 403
    } else {
        EXPECT_EQ(r.status_code, 200); //Expectde if call done from local computer.
        EXPECT_GT(r.text.length(), 0);
        EXPECT_EQ(r.header["content-type"], "application/json; charset=utf-8");
    }

    spdlog::info("Real API test completed with status: {}", r.status_code);
};  

/* Mock-test: Always simulates positve reponce: 200*/
TEST(FakeStoreApiTest, Handles200_OK_WithMock) {
    spdlog::info("Starting mocked API test (forcing 200)...");

    MockApiClient mock_client;

    cpr::Response mock_response;
    mock_response.status_code = 200;
    mock_response.text = R"([{"id":1,"title":"Mock Product"}])";
    mock_response.header["content-type"] = "application/json; charset=utf-8";

    EXPECT_CALL(mock_client, get(_))
        .WillOnce(Return(mock_response));

    cpr::Response r = mock_client.get(API_URL);

    EXPECT_EQ(r.status_code, 200);
    EXPECT_GT(r.text.length(), 0);
    EXPECT_EQ(r.header["content-type"], "application/json; charset=utf-8");

    spdlog::info("Mocked API test completed with status: {}", r.status_code);
    if (r.status_code == 200) {
        spdlog::info("Tests will run on actual API database data");
    } 
    else {
        spdlog::info("Tests will use predefined (pre-fetched) JSON data due to API access issues.");
    }  
}




/*Mock-test: Always sinulates responce:  403*/
TEST(FakeStoreApiTest, Handles403ForbiddenWithMock) {
    spdlog::info("Starting mocked API test (forcing 403)...");

    MockApiClient mock_client;

    cpr::Response mock_response;
    mock_response.status_code = 403;
    mock_response.text = R"({"error": "Forbidden"})";
    mock_response.header["content-type"] = "application/json; charset=utf-8";

    EXPECT_CALL(mock_client, get(_))
        .WillOnce(Return(mock_response));

    cpr::Response r = mock_client.get(API_URL);

    EXPECT_EQ(r.status_code, 403);
    EXPECT_GT(r.text.length(), 0);
    EXPECT_EQ(r.header["content-type"], "application/json; charset=utf-8");

    spdlog::info("Mocked API test completed with status: {}", r.status_code);
}

// Ta bort din GetProductsReturns200withMock – den är duplikat

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}