#include <gtest/gtest.h>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

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
//Test
int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);  // Sätt loggnivå
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}