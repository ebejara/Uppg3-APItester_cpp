#include <gtest/gtest.h>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

TEST(FakeStoreApiTest, GetProductsReturns200) {
    spdlog::info("Starting API test...");

    cpr::Response r = cpr::Get(cpr::Url{"https://fakestoreapi.com/products"});

    // Kontrollera statuskod
    EXPECT_EQ(r.status_code, 200);

    // Extra kontroller för robusthet
    EXPECT_GT(r.text.length(), 0);  // Fick något svar
    EXPECT_EQ(r.header["content-type"], "application/json; charset=utf-8");

    spdlog::info("API test completed with status: {}", r.status_code);
}
//Test
int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);  // Sätt loggnivå
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}