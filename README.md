# Uppg3 - API Tester (C++)

Detta är inlämningsuppgift 3 i kursen Kontinuerlig Integration (CI).
Projektet använder Selenium WebDriver i Java med JUnit 5 och Maven för att automatisera tester mot webbplatsen saucedemo.com.

En C++-applikation som testar FakeStoreAPI[](https://fakestoreapi.com) med integrationstester och GitHub Actions CI.

## Översikt

Projektet implementerar:
- HTTP-anrop till `GET /products` med CPR-biblioteket
- JSON-parsing med nlohmann/json
- Integrationstester med GoogleTest
- Automatisk CI med GitHub Actions (Ubuntu och Windows)

## Kravuppfyllelse

### Grundkrav för G
- Integrationstest som anropar `GET /products` och verifierar statuskod 200
- GitHub Actions-pipeline som kör testerna vid varje push

### Utökade tester för VG
- Validering av antal produkter (exakt 20)
- Validering av specifika fält (title, price, category) på en produkt
- Validering av data för specifikt produkt-ID (t.ex. id 5)

## Bygg och kör lokalt
- Lokala anrop till returnerar positive respons (HTTP 200).
- Anrop från Hithub Actions returnerar negativ respons (HTTP 403)
- Rekommenderar att köra lokalt.


### Förutsättningar
- CMake ≥ 3.14
- C++17-kompilator (g++, clang++, MSVC)
- Git

### Utvecklingsmiljö
- VSCode (lokalt)
- Bibliotek: se CMakeLists.txt

### Steg
git clone https://github.com/ebejara/Uppg3-APItester_cpp.git
cd Uppg3-APItester_cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/APItester        # Linux/macOS
build\Release\APItester.exe  # Windows
