# Homework 22.2 (Part 1) — Enhanced E-Commerce Catalog

C++ implementation extending the e-commerce catalog with image metadata, serialization, and checksumming.

## Algorithms & Complexity

| Operation | Time | Method |
|---|---|---|
| Serialize catalog | O(n) | Sorted traversal by id + string building |
| Deserialize catalog | O(n * m) | Manual JSON-like parser (m = avg field length) |
| Product hash (djb2) | O(m) | Bernstein hash: h = 33*h XOR c |
| Catalog checksum | O(n * m) | XOR of all product hashes |
| Category lookup | O(1) amortized | unordered_map index |

## Build & Run

```bash
g++ -std=c++17 -O2 -o homework22_1 main.cpp
./homework22_1
```

## Description

- Product struct with optional Image metadata (path, dimensions, byte size)
- Manual JSON-like serialization and deserialization without external libraries
- Round-trip verification: serialize -> deserialize -> serialize produces identical output
- djb2 hash for individual product fingerprinting
- XOR-based catalog checksum for integrity verification
- Per-category image statistics
