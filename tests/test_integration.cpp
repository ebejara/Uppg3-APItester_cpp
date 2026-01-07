#include <gmock/gmock.h>
#include "APItester.hpp"
#include <spdlog/spdlog.h>


using json = nlohmann::json;
bool products_file_loaded_from_API = false;
ApiClient client;
/*****************************************************************
*Help function to load products JSON from pre-feteched file      *
*located in /src/products_prefetched.json. Functions finds the   * 
*project root directory and constructs the path to the JSON file.*
*Used for unit testing the API client without calling the        *
actual API.                                                      *
/*****************************************************************/
json load_products_json() {
    json j;

    cpr::Response  r = client.get(API_URL);
    if (r.status_code == 200) {
    
        try {
            j = json::parse(r.text);  // Parse the JSON from the file
           products_file_loaded_from_API = true;
        } catch (const json::parse_error& e) {
            throw std::runtime_error("JSON-parsing error on API data " + std::string(e.what()));
        }
    }
    else{
        std::filesystem::path exe_path = std::filesystem::current_path();
        std::filesystem::path project_root;

        // Försök att hitta projektets rotmapp
        std::filesystem::path current_path = exe_path;
        while (current_path != current_path.root_path()) {
            if (std::filesystem::exists(current_path / "src")) {
                project_root = current_path;
                break;
            }
            current_path = current_path.parent_path();
        }

        std::string path = project_root.string() + "/src/products_prefetched.json";
        std::ifstream file(path);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path + 
                                    " – check src/ and is committed to GitHub!");
        }

        try {
            j = json::parse(file);  // Parse the JSON from the file
            products_file_loaded_from_API = false;
        } catch (const json::parse_error& e) {
            throw std::runtime_error("JSON-parsing error in products_prefetched.json: " + std::string(e.what()));
        }
    } //else
    return j;
}

class MockApiClient : public ApiClient {
public:
    MOCK_METHOD(cpr::Response, get, (const std::string&), (override));
};

TEST(ApiClientUnitTest, GetReturns200OnSuccess) {
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

TEST(ApiClientUnitTest, GetReturns403OnFailure) {
    MockApiClient mock;

    cpr::Response fake_failure;
    fake_failure.status_code = 403;
    fake_failure.text = "Forbidden";

    EXPECT_CALL(mock, get(::testing::_)).WillOnce(::testing::Return(fake_failure));

    cpr::Response result = mock.get(API_URL);

    EXPECT_EQ(result.status_code, 403);
   // EXPECT_THAT(result.text, ::testing::HasSubstr("Forbidden"));  // ← Rätt!
}

/*****************************************************************
*Unit test: Checks that the JSON file contains exactly 20        *
*products in /src/products_prefetched.json. Functions finds the   * 
*project root directory and constructs the path to the JSON file.*
*Used for unit testing the API client without calling the        *
actual API.                                                      *
/*****************************************************************/
TEST(FakeStoreApiIntegrationTest, ReturnsExactly20Products) {
   // json products = load_products_json();
    cpr::Response  r = client.get(API_URL);
    json products = json::parse(r.text);
    std::cout << "[----------] Number of products: " << products.size() << std::endl;
    spdlog::info("Number of products returned from API is: {}", products.size());
    EXPECT_EQ(products.size(), 20) << "API";
}

// VG-krav 2: En specifik produkt innehåller korrekta fält (title, price, category)
TEST(FakeStoreApiIntegrationTest, FirstProductHasCorrectFields) {
    cpr::Response  r = client.get(API_URL);
    json products = json::parse(r.text);

    EXPECT_EQ(products[0]["id"].get<int>(), 1);
    EXPECT_EQ(products[0]["title"].get<std::string>(),
              "Fjallraven - Foldsack No. 1 Backpack, Fits 15 Laptops");
    EXPECT_EQ(products[0]["category"].get<std::string>(), "men's clothing");
    // Extra checks för säkerhet
    EXPECT_TRUE(products[0].contains("description"));
    EXPECT_TRUE(products[0].contains("image"));
    EXPECT_TRUE(products[0].contains("rating"));

// VG-krav 3: Ett specifikt produkt-ID returnerar rätt data
TEST(FakeStoreApiIntegration, ProductByIdReturnsCorrectData) {
    cpr::Response  r = client.get(API_URL);
    json products = json::parse(r.text);
    
    // Hitta produkt med id = 5 (exempel)
    json product5;
    bool found = false;
    for (const auto& p : products) {
        if (p["id"].get<int>() == 5) {
            product5 = p;
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found) << "Produkt med id 5 hittades inte i JSON-filen";

    EXPECT_EQ(product5["title"].get<std::string>(), "John Hardy Women's Legends Naga Gold & Silver Dragon Station Chain Bracelet");
    EXPECT_DOUBLE_EQ(product5["price"].get<double>(), 695.0);
    EXPECT_EQ(product5["category"].get<std::string>(), "jewelery");
}

// Bonus: Testa en annan produkt för extra säkerhet (valfritt men bra för VG)
TEST(FakeStoreApiIntegration, LastProductHasCorrectId) {
    json products = load_products_json();
    json last = products.back();

    EXPECT_EQ(last["id"].get<int>(), 20);
}