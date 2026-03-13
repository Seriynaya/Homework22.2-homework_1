#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <optional>
#include <iomanip>
#include <numeric>
#include <ranges>
#include <cmath>

struct Image {
    std::string path;
    int width;
    int height;
    std::size_t bytes;
};

struct Product {
    int id;
    int category_id;
    std::string name;
    double price;
    int stock;
    std::optional<Image> image;
};

struct Category {
    int id;
    std::string name;
};

// h = 33 * h xor c
[[nodiscard]] auto djb2_hash(std::string_view s) -> unsigned long {
    unsigned long h = 5381;
    for (const char c : s)
        h = ((h << 5) + h) ^ static_cast<unsigned char>(c);
    return h;
}

// h(p) = djb2(id || name || price || stock)
[[nodiscard]] auto product_hash(const Product& p) -> unsigned long {
    std::string data = std::to_string(p.id) + "|" + p.name + "|" +
                       std::to_string(p.price) + "|" + std::to_string(p.stock);
    if (p.image.has_value())
        data += "|" + p.image->path;
    return djb2_hash(data);
}

// h(C) = xor_{p in C} h(p)
[[nodiscard]] auto catalog_checksum(const std::vector<Product>& products) -> unsigned long {
    unsigned long h = 0;
    for (const auto& p : products)
        h ^= product_hash(p);
    return h;
}

[[nodiscard]] auto escape_str(std::string_view s) -> std::string {
    std::string result;
    result.reserve(s.size());
    for (const char c : s) {
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else result += c;
    }
    return result;
}

[[nodiscard]] auto serialize_image(const Image& img) -> std::string {
    std::ostringstream oss;
    oss << "{\"path\":\"" << escape_str(img.path)
        << "\",\"width\":" << img.width
        << ",\"height\":" << img.height
        << ",\"bytes\":" << img.bytes << "}";
    return oss.str();
}

[[nodiscard]] auto serialize_product(const Product& p) -> std::string {
    std::ostringstream oss;
    oss << "{\"id\":" << p.id
        << ",\"category_id\":" << p.category_id
        << ",\"name\":\"" << escape_str(p.name) << "\""
        << ",\"price\":" << std::fixed << std::setprecision(2) << p.price
        << ",\"stock\":" << p.stock;
    if (p.image.has_value())
        oss << ",\"image\":" << serialize_image(*p.image);
    else
        oss << ",\"image\":null";
    oss << "}";
    return oss.str();
}

// sort by id, serialize sequentially: O(n log n)
[[nodiscard]] auto serialize_catalog(std::vector<Product> products) -> std::string {
    std::ranges::sort(products, [](const Product& a, const Product& b) {
        return a.id < b.id;
    });
    std::ostringstream oss;
    oss << "[";
    for (int i = 0; i < static_cast<int>(products.size()); ++i) {
        if (i) oss << ",";
        oss << serialize_product(products[i]);
    }
    oss << "]";
    return oss.str();
}

[[nodiscard]] constexpr auto skip_ws(std::string_view s, std::size_t pos) -> std::size_t {
    while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\n' || s[pos] == '\t' || s[pos] == '\r'))
        ++pos;
    return pos;
}

[[nodiscard]] auto parse_string(const std::string& s, std::size_t& pos) -> std::string {
    ++pos; // skip opening "
    std::string result;
    while (pos < s.size() && s[pos] != '"') {
        if (s[pos] == '\\' && pos + 1 < s.size()) {
            ++pos;
            result += s[pos];
        } else {
            result += s[pos];
        }
        ++pos;
    }
    ++pos; // skip closing "
    return result;
}

[[nodiscard]] auto parse_number(const std::string& s, std::size_t& pos) -> double {
    const std::size_t start = pos;
    if (pos < s.size() && s[pos] == '-') ++pos;
    while (pos < s.size() && (std::isdigit(s[pos]) || s[pos] == '.'))
        ++pos;
    return std::stod(s.substr(start, pos - start));
}

auto expect(const std::string& s, std::size_t& pos, char c) -> void {
    pos = skip_ws(s, pos);
    if (pos < s.size() && s[pos] == c) ++pos;
}

[[nodiscard]] auto parse_key(const std::string& s, std::size_t& pos) -> std::string {
    pos = skip_ws(s, pos);
    return parse_string(s, pos);
}

[[nodiscard]] auto deserialize_image(const std::string& s, std::size_t& pos) -> Image {
    Image img;
    expect(s, pos, '{');
    for (int field = 0; field < 4; ++field) {
        pos = skip_ws(s, pos);
        if (field > 0) expect(s, pos, ',');
        const auto key = parse_key(s, pos);
        expect(s, pos, ':');
        pos = skip_ws(s, pos);
        if (key == "path") img.path = parse_string(s, pos);
        else if (key == "width") img.width = static_cast<int>(parse_number(s, pos));
        else if (key == "height") img.height = static_cast<int>(parse_number(s, pos));
        else if (key == "bytes") img.bytes = static_cast<std::size_t>(parse_number(s, pos));
    }
    expect(s, pos, '}');
    return img;
}

[[nodiscard]] auto deserialize_product(const std::string& s, std::size_t& pos) -> Product {
    Product p;
    expect(s, pos, '{');
    for (int field = 0; field < 6; ++field) {
        pos = skip_ws(s, pos);
        if (field > 0) expect(s, pos, ',');
        const auto key = parse_key(s, pos);
        expect(s, pos, ':');
        pos = skip_ws(s, pos);
        if (key == "id") p.id = static_cast<int>(parse_number(s, pos));
        else if (key == "category_id") p.category_id = static_cast<int>(parse_number(s, pos));
        else if (key == "name") p.name = parse_string(s, pos);
        else if (key == "price") p.price = parse_number(s, pos);
        else if (key == "stock") p.stock = static_cast<int>(parse_number(s, pos));
        else if (key == "image") {
            pos = skip_ws(s, pos);
            if (s.substr(pos, 4) == "null") {
                pos += 4;
                p.image = std::nullopt;
            } else {
                p.image = deserialize_image(s, pos);
            }
        }
    }
    expect(s, pos, '}');
    return p;
}

// O(n * m), m = avg product string length
[[nodiscard]] auto deserialize_catalog(const std::string& s) -> std::vector<Product> {
    std::vector<Product> products;
    std::size_t pos = 0;
    expect(s, pos, '[');
    pos = skip_ws(s, pos);
    while (pos < s.size() && s[pos] != ']') {
        if (!products.empty()) expect(s, pos, ',');
        pos = skip_ws(s, pos);
        products.push_back(deserialize_product(s, pos));
        pos = skip_ws(s, pos);
    }
    expect(s, pos, ']');
    return products;
}

// find product by id, nullopt if not found
[[nodiscard]] auto find_product(const std::vector<Product>& products, int id)
    -> std::optional<Product> {
    for (const auto& p : products) {
        if (p.id == id) return p;
    }
    return std::nullopt;
}

void print_product(const Product& p) {
    std::cout << "  [" << p.id << "] " << p.name
              << " | cat:" << p.category_id
              << " | $" << std::fixed << std::setprecision(2) << p.price
              << " | stock:" << p.stock;
    if (p.image.has_value())
        std::cout << " | img:" << p.image->path
                  << " (" << p.image->width << "x" << p.image->height
                  << ", " << p.image->bytes << "B)";
    else
        std::cout << " | img:none";
    std::cout << "\n";
}

int main() {
    const std::vector<Category> categories = {
        {1, "Electronics"},
        {2, "Clothing"},
        {3, "Books"},
        {4, "Home & Garden"}
    };

    // |P| = 12
    std::vector<Product> products = {
        {1, 1, "Laptop Pro 15",       1299.99, 25,
            Image{"images/laptop.jpg", 800, 600, 245760}},
        {2, 1, "Wireless Mouse",         29.99, 150,
            Image{"images/mouse.png", 400, 400, 51200}},
        {3, 1, "USB-C Hub",              49.99, 80, std::nullopt},
        {4, 2, "Cotton T-Shirt",         19.99, 200,
            Image{"images/tshirt.jpg", 600, 800, 184320}},
        {5, 2, "Denim Jeans",            59.99, 75,
            Image{"images/jeans.jpg", 600, 900, 204800}},
        {6, 2, "Winter Jacket",         149.99, 3, std::nullopt},
        {7, 3, "C++ Primer",             49.99, 30,
            Image{"images/cpp_primer.jpg", 300, 450, 40960}},
        {8, 3, "CLRS Algorithms",        79.99, 20,
            Image{"images/clrs.jpg", 300, 450, 38400}},
        {9, 3, "Clean Code",             34.99, 2, std::nullopt},
        {10, 4, "Garden Hose 50ft",      39.99, 40,
            Image{"images/hose.jpg", 500, 500, 102400}},
        {11, 4, "LED Desk Lamp",         24.99, 100, std::nullopt},
        {12, 4, "Ceramic Plant Pot",     12.99, 0,
            Image{"images/pot.png", 400, 400, 61440}},
    };

    std::cout << "=== Product Catalog (" << products.size() << " products) ===\n";
    for (const auto& p : products) print_product(p);

    // find_product(7)
    if (const auto found = find_product(products, 7)) {
        std::cout << "\nfind_product(7):\n";
        print_product(*found);
    }

    // serialize: O(n log n)
    std::cout << "\n=== Serialize Catalog ===\n";
    auto json = serialize_catalog(products);
    std::cout << "JSON (" << json.size() << " chars):\n";
    if (json.size() > 300)
        std::cout << json.substr(0, 300) << "...\n";
    else
        std::cout << json << "\n";

    // deserialize: O(n * m)
    std::cout << "\n=== Deserialize & Verify ===\n";
    auto restored = deserialize_catalog(json);
    std::cout << "Restored " << restored.size() << " products:\n";
    for (const auto& p : restored) print_product(p);

    // serialize(deserialize(s)) = s
    const auto json2 = serialize_catalog(restored);
    std::cout << "\nRound-trip match: " << (json == json2 ? "YES" : "NO") << "\n";

    // h = 33 * h xor c
    std::cout << "\n=== Product Hashes (djb2) ===\n";
    for (const auto& p : products) {
        std::cout << "  [" << p.id << "] " << p.name
                  << " -> 0x" << std::hex << product_hash(p) << std::dec << "\n";
    }

    // h(C) = xor_{p in C} h(p)
    const unsigned long cksum = catalog_checksum(products);
    std::cout << "\nCatalog checksum: 0x" << std::hex << cksum << std::dec << "\n";

    const unsigned long cksum2 = catalog_checksum(restored);
    std::cout << "Restored checksum: 0x" << std::hex << cksum2 << std::dec << "\n";
    std::cout << "Checksum match: " << (cksum == cksum2 ? "YES" : "NO") << "\n";

    // category image stats
    std::cout << std::dec << "\n=== Category Image Stats ===\n";
    std::unordered_map<int, std::vector<int>> cat_map;
    for (int i = 0; i < static_cast<int>(products.size()); ++i)
        cat_map[products[i].category_id].push_back(i);

    for (const auto& cat : categories) {
        const auto it = cat_map.find(cat.id);
        if (it == cat_map.end()) continue;
        int with_img = 0, without_img = 0;
        std::size_t total_bytes = 0;
        for (const int idx : it->second) {
            if (products[idx].image.has_value()) {
                ++with_img;
                total_bytes += products[idx].image->bytes;
            } else {
                ++without_img;
            }
        }
        std::cout << "  " << cat.name << ": "
                  << with_img << " with image, "
                  << without_img << " without, "
                  << total_bytes << " bytes total\n";
    }

    return 0;
}
