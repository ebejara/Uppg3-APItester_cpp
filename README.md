# Uppg3 - API Tester (C++)
Inlämn ingsuppgift 3 . kursen Testautomatisering med Continuous Integration (CI)
En C++-applikation som testar FakeStoreAPI[](https://fakestoreapi.com) med integrationstester och GitHub Actions CI.
Applicationen anropar API:t och skriver ut producterna i terminalfönstret vid lyckad anrop, annars kastas en "exeption"
Testerna innehåller  mockade anrop och även riktiga anrop (som ju är kravet på uppgiften)

## Översikt

Projektet implementerar:
- HTTP-anrop till `GET /products` med CPR-biblioteket
- JSON-parsing med nlohmann/json
- Integrationstester med GoogleTest
- Automatisk CI med GitHub Actions (Ubuntu och Windows)

## Kravuppfyllelse

### Grundläggande krav
- Integrationstest som anropar `GET /products` och verifierar statuskod 200
- GitHub Actions-pipeline som kör testerna vid varje push

### Utökade krav
- Validering av antal produkter (exakt 20)
- Validering av specifika fält (title, price, category) på en produkt
- Validering av data för specifikt produkt-ID (t.ex. id 5)
## Bygg och kör lokalt
- När man kör lokalt så lyckas anropen till API:t. Misslyckas när man kör från github actions.
### Förutsättningar
- CMake ≥ 3.14
- C++17-kompilator (g++, clang++, MSVC)
- Git

### Utvecklingsmiljö
- VSCode (lokalt med MinGW g++ kompilator)
- Bibliotek: se CMakeLists.txt

### Steg
git clone https://github.com/ebejara/Uppg3-APItester_cpp.git
cd Uppg3-APItester_cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/APItester        # Linux/macOS
build\Release\APItester.exe  # Windows
(eller kör Cmake konfiguration in VSCode, första gången måste man välja kompilator)
