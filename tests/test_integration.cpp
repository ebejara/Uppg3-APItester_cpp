#include <gmock/gmock.h>
#include "Apitester.hpp"

class MockApiClient : public ApiClient {
public:
    MOCK_METHOD(cpr::Response, get, (const std::string&), (override));
};

TEST(ApiClientTest, GetReturns200OnSuccess) {
    MockApiClient mock;

    cpr::Response fake_success;
    fake_success.status_code = 200;
    fake_success.text = R"( [{"id":1,"title":"Test Product"}] )";

    EXPECT_CALL(mock, get(::testing::_)).WillOnce(::testing::Return(fake_success));

    cpr::Response result = mock.get(API_URL);

    EXPECT_EQ(result.status_code, 200);
    EXPECT_FALSE(result.text.empty());
    //EXPECT_THAT(result.text, ::testing::HasSubstr("Test Product"));  // ← Rätt!
}


TEST(ApiClientTest, GetReturns403OnFailure) {
    MockApiClient mock;

    cpr::Response fake_failure;
    fake_failure.status_code = 403;
    fake_failure.text = "Forbidden";

    EXPECT_CALL(mock, get(::testing::_)).WillOnce(::testing::Return(fake_failure));

    cpr::Response result = mock.get(API_URL);

    EXPECT_EQ(result.status_code, 403);
   // EXPECT_THAT(result.text, ::testing::HasSubstr("Forbidden"));  // ← Rätt!
}