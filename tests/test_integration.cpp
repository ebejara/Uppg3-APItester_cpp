#include <gmock/gmock.h>
#include "APItester.hpp"
#include <spdlog/spdlog.h>


using json = nlohmann::json;

ApiClient client;

class MockApiClient : public ApiClient {
public:
    MOCK_METHOD(cpr::Response, get, (const std::string&), (override));
};


/******************************************************************
 * Unit test: Verifies that client.get() returns HTTP 200 and 
 * a valid JSON body when the API call succeeds successfully.
 * 
 * This test:
 *   - Mocks a successful API response (status 200)
 *   - Uses a realistic JSON array payload
 *   - Verifies status code, non-empty body, and content presence
 ******************************************************************/
TEST(ApiClientUnitTest, GetReturns200OnSuccess) {
    // Arrange
    MockApiClient mock;
    // Prepare fake successful response that the mock should return
    cpr::Response fake_success;
    fake_success.status_code = 200;
    fake_success.text = R"( [{"id":1,"title":"Test Product"}] )";// Raw string literal for clean JSON
    
    // Set expectation: when get() is called with ANY argument,
    // return our prepared successful response exactly once
    EXPECT_CALL(mock, get(::testing::_)).WillOnce(::testing::Return(fake_success));
    
    // Act
    cpr::Response result = mock.get(API_URL);
    
    //Assert
    EXPECT_EQ(result.status_code, 200);
    EXPECT_FALSE(result.text.empty());
    EXPECT_THAT(result.text, ::testing::HasSubstr("Test Product"));
}

/******************************************************************
 * Unit test: Verifies that client.get() returns HTTP 403 and 
 * a an empty JSON body when the API call fails.
 * 
 * This test:
 *   - Mocks a failed API response (status 403)
 *   - Uses an empty JSON body
 *   - Verifies status code and empty body
 ******************************************************************/
TEST(ApiClientUnitTest, GetReturns200OnSuccess) {
TEST(ApiClientUnitTest, GetReturns403WithEmptyBodyOnFailure) {
    
    //Arrange
    MockApiClient mock;

    // Skapar en simulerad felrespons (403 Forbidden)
    cpr::Response fake_failure;
    fake_failure.status_code = 403;
    fake_failure.text = "";  // Empty body – according to test requirement

    EXPECT_CALL(mock, get(::testing::_)).WillOnce(::testing::Return(fake_failure));

    // Act
    cpr::Response result = mock.get(API_URL);

    // Assert
    EXPECT_EQ(result.status_code, 403) << "Status code shall be 403";
    EXPECT_TRUE(result.text.empty()) << "Response body shall be empty at fault";
    
}

/*****************************************************************
 * Integration Test
 * 
 * Test Name:    ReturnsExactly20Products
 * Description:  Verifies that the Fake Store API endpoint 
 *               returns exactly 20 products in the JSON array response
 * 
 * Note:         FakeStoreAPI always returns exactly 
 *               20 dummy products 
 *****************************************************************/
TEST(FakeStoreApiIntegrationTest, ReturnsExactly20Products) {
    // Send GET request to the products endpoint
    cpr::Response  r = client.get(API_URL);


    // Here we assume success
    // Parse the raw JSON response body into a nlohmann::json object
    json products = json::parse(r.text);
    
    // Basic logging for debugging / CI visibility - Status code is 200 when successful
    spdlog::info("HTTP response code from API is: {}", r.status_code);
    
    // Log the actual number of products we received (very useful when test fails)
    spdlog::info("Number of products returned from API is: {}", products.size());
    
    // Assertion: verify we received exactly the expected number of items
    EXPECT_EQ(products.size(), 20) << "FakeStoreAPI should always return exactly 20 products\n"
        << "(this is a well-known characteristic of this public test API)";
}

/*****************************************************************
 * Integration Test
 *
 * Test Name:    FirstProductHasCorrectFields
 * Description:  Verifies that the **first product** returned by the
 *               Fake Store API has the expected structure and contains
 *               the correct values for the most important fields.
 *
 * Purpose:      - Confirms the API returns data in the expected format
 *               - Catches breaking schema changes early
 *               - Verifies that the dummy data hasn't changed unexpectedly
 *
 * Note:         The first product is hardcoded in FakeStoreAPI and has
 *               been stable since the API's creation (~2021).
 *****************************************************************/
TEST(FakeStoreApiIntegrationTest, FirstProductHasCorrectFields) {
    
    // Fetch all products from the API (real call)
    cpr::Response  r = client.get(API_URL);

    // Assert that the call was successful
    ASSERT_EQ(r.status_code, 200) << "API request failed";

    // Parse the JSON response
    json products = json::parse(r.text);
    
    // Quick visibility in logs (especially useful in CI)
    spdlog::info("HTTP response code from API is: {}", r.status_code);
    
    // Make sure we actually have at least one product
    ASSERT_FALSE(products.empty()) << "API returned empty product list";

    //Check that the first product has the expected id
    EXPECT_EQ(products[0]["id"].get<int>(), 1);

    // Verify key fields of the first product
    EXPECT_EQ(products[0]["title"].get<std::string>(),
              "Fjallraven - Foldsack No. 1 Backpack, Fits 15 Laptops");
    EXPECT_EQ(products[0]["category"].get<std::string>(), "men's clothing");
    EXPECT_TRUE(products[0].contains("id"));
    EXPECT_TRUE(products[0].contains("title"));
    EXPECT_TRUE(products[0].contains("category"));

    // Extra  check to be sure other fields are present
    EXPECT_TRUE(products[0].contains("description"));
    EXPECT_TRUE(products[0].contains("image"));
    EXPECT_TRUE(products[0].contains("rating"));
}

/*****************************************************************
*Integration test: Checks that the JSON file contains an item    *
*with correct fields and values .                                *
/*****************************************************************/
TEST(FakeStoreApiIntegration, ProductByIdReturnsCorrectData) {
    //Fetch all products from API
    cpr::Response r = client.get(API_URL);
    json products = json::parse(r.text);
    spdlog::info("HTTP response code from API is: {}", r.status_code);
    // Hitta produkt med id = 5
    json product5;
    bool found = false;
    for (const auto& p : products) {
        if (p["id"].get<int>() == 5) {
            product5 = p;
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found) << "Product with id 5 not found in JSON body from API response";

    //====== Title field checks for product with id 5 ======
    EXPECT_EQ(product5["title"].get<std::string>(),
              "John Hardy Women's Legends Naga Gold & Silver Dragon Station Chain Bracelet");

    //====== Price field checks ======
    ASSERT_TRUE(product5.contains("price")) 
    << "Product is missing 'price'-field";          
    double price = product5["price"].get<double>();
    EXPECT_GT(price, -0.1) 
    << "Price should be at least 0 (actual value: " << price << ")";
    EXPECT_EQ(product5["category"].get<std::string>(), "jewelery");

    //====== Description field checks ======
    ASSERT_TRUE(product5.contains("description")) 
        << "Product with id 5 missing 'description'-field";

    std::string description = product5["description"].get<std::string>();
    EXPECT_FALSE(description.empty()) 
        << "Description-field cannot be empty.Not salesable without description in client application.";
    EXPECT_LE(description.length(), 1000) 
        << "Description-field is too long. Not compatible w/ client application" 
        << description.length() << ")";


    //====== Description field checks ======
    ASSERT_TRUE(product5.contains("category")) 
    << "Product is missing 'category'-field"; 
    std::string category = product5["category"].get<std::string>();
    EXPECT_FALSE(category.empty()) 
        << "Category-field cannot be empty.Not salesable without category in client application.";
    EXPECT_LE(category.length(), 50) 
        << "Category-field is too long. Not compatible w/ client application" 
        << category.length() << ")";
    //====== ratinh field checks ======

    // Kontrollera att "rating"-fältet finns
    ASSERT_TRUE(product5.contains("rating")) 
        << "Product is missing 'rating'-field";

    // Hämta rating-objektet
    const json& rating = product5["rating"];

    // Check that rating is an object (not null, string, etc.)
    ASSERT_TRUE(rating.is_object()) 
        << "'rating'-filed is not a JSON object";

    // Check that rating is not empty (has keys)
    ASSERT_FALSE(rating.empty()) 
        << "'rating'-object is empty – expects at least 'rate' and 'count'";
    // Check that both 'rate' and 'count' fields are present
    ASSERT_TRUE(rating.contains("rate")) << "Rating saknar 'rate'-fält";
    ASSERT_TRUE(rating.contains("count")) << "Rating saknar 'count'-fält";

    // Check that rate is a number between 0 and 5
    ASSERT_TRUE(rating["rate"].is_number()) << "'rate' is not a number";
    double rate = rating["rate"].get<double>();
    EXPECT_GE(rate, 0.0) << "Rating rate must be at least 0";
    EXPECT_LE(rate, 5.0) << "Rating rate must be at most 5";

    // Check that count is an integer and greater than or equal to 0
    ASSERT_TRUE(rating["count"].is_number_integer()) << "'count' is not an integer";
    int count = rating["count"].get<int>();
    EXPECT_GT(count, 0) << "Rating count must be greater than 0";
        
}

// Bonus: Testa en annan produkt för extra säkerhet (valfritt men bra för VG)
/*TEST(FakeStoreApiIntegration, LastProductHasCorrectId) {
    json products = load_products_json();
    json last = products.back();

    EXPECT_EQ(last["id"].get<int>(), 20);
}*/
