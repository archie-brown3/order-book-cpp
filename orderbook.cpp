#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <queue>
#include "orders.h"
#include "ordersApi.h"
#include <iostream>

int main() {
    OrderBook book;
    
    // Test Case 1: Partial fill
    std::cout << "Test 1: Partial Fill\n";
    auto buy1 = std::make_shared<Order>(1, OrderSide::Buy, 100, 100, OrderType::Goodtillcancel);
    auto sell1 = std::make_shared<Order>(2, OrderSide::Sell, 100, 50, OrderType::Goodtillcancel);
    
    book.ProcessNewOrder(buy1);
    Trades trades1 = book.ProcessNewOrder(sell1);
    std::cout << "Trades: " << trades1.size() << " (Expected: 1)\n\n";
    
    // Test Case 2: No match (prices don't cross)
    std::cout << "Test 2: No Match\n";
    auto buy2 = std::make_shared<Order>(3, OrderSide::Buy, 95, 50, OrderType::Goodtillcancel);
    auto sell2 = std::make_shared<Order>(4, OrderSide::Sell, 105, 50, OrderType::Goodtillcancel);
    
    book.ProcessNewOrder(buy2);
    Trades trades2 = book.ProcessNewOrder(sell2);
    std::cout << "Trades: " << trades2.size() << " (Expected: 0)\n\n";
    
    // Test Case 3: FOK rejected
    std::cout << "Test 3: FOK Rejected\n";
    auto fok = std::make_shared<Order>(5, OrderSide::Buy, 110, 1000, OrderType::Fillandkill);
    Trades trades3 = book.ProcessNewOrder(fok);
    std::cout << "Trades: " << trades3.size() << " (Expected: 0)\n\n";
    
    // Test Case 4: Cancel order
    std::cout << "Test 4: Cancel Order\n";
    try {
        book.CancelOrder(3);
        std::cout << "Successfully cancelled order 3\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    
    return 0;
}



