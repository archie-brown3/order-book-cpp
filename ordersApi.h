#include "orders.h"
#include <map>
#include <unordered_map>



// Contains order related methods for OrderBook class


class ModifyOrder{
 public:
  ModifyOrder(OrderId id, OrderSide side, Price price, Quantity quantity):
   id_{id},
   side_{side},
   price_{price},
   quantity_{quantity}
  { }
  
  OrderId GetOrderId() const { return id_; }
  Price GetPrice() const { return price_; }
  OrderSide GetOrderSide() const { return side_; }
  Quantity GetQuantity() const { return quantity_; }

  OrderPointer toOrderPointer() const { 
   return std::make_shared<Order>(id_, side_, price_, quantity_, OrderType::Goodtillcancel);
  }

 private:
  OrderId id_;
  Price price_;
  OrderSide side_;
  Quantity quantity_;
};



// Use map to represent bids and asks 
// Access a given order by its OrderId in O(1) time
class OrderBook{
 // represent order / location in order book:
 private: 
  struct OrderEntry{
   OrderPointer order_pointer{nullptr};  // shared pointer to the order
   OrderPointers::iterator location_;  // iterator to the order's position in the list at its price level
  };

  // Data members
  std::map<Price, OrderPointers, std::greater<Price>> bids_ ; // Map of price levels to lists of buy orders (highest price first), std::greater for descending order
  std::map<Price, OrderPointers, std::less<Price>> asks_ ; // Map of price levels to lists of sell orders (lowest price first)  
  std::unordered_map<OrderId, OrderEntry> order_map_ ; // Map of OrderId to OrderEntry for quick access

  // Match method to match incoming orders against existing orders
  // Canmatch: FOK orders only match if they can be fully filled immediately
  bool CanMatch(OrderSide side, Price price)const 
  {
  if (side == OrderSide::Buy) {
       if (asks_.empty()) {
           return false; // No asks available to match
       }
       // Check if price >= best ask price
       const auto& best_ask = asks_.begin(); // Lowest ask price
       return price >= best_ask->first;
  }
  else 
  {
  if (bids_.empty()) {
       return false; // No bids available to match
   } 
   const auto& best_bid = bids_.begin(); // Highest bid price
    return price <= best_bid->first;
  }
 }
 
 
 

 // Match method to match incoming orders against existing orders
 
 Trades MatchOrders()
 {
  Trades trades;
  trades.reserve(order_map_.size()); // Pre-allocate space for trades to minimize reallocations with empty vector 
  
  while(true){
   if (bids_.empty() || asks_.empty()){
    break; // No more possible matches
   }

   // Get best prices
   auto& [bid_price, bid_orders] = *bids_.begin(); 
   auto& [ask_price, ask_orders] = *asks_.begin(); 

   if (bid_price < ask_price){
    break;  // Prices don't cross - no match possible
   }
   
   while (!bid_orders.empty() && !ask_orders.empty()){
    auto& bid = bid_orders.front(); // get the first order at the best bid price
    auto& ask = ask_orders.front(); // get the first order at the best ask price
    
    Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity()); // eg; Bid wants 100, Ask wants 70 → trade 70
    
    // Fill both orders
    bid->Fill(quantity);  
    ask->Fill(quantity);  
    
    // remove filled orders from order book
    if (bid->IsFilled())
    {
        bid_orders.pop_front();
    }
    if (ask->IsFilled())
    {
        ask_orders.pop_front();
    }
    
    // Remove price levels if no orders remain
    if (bid_orders.empty())
    {
        bids_.erase(bid_price);
    }
    if (ask_orders.empty())
    {
        asks_.erase(ask_price);
    }

    // Create a trade object and add to trades vector
    trades.push_back(Trade(
     TradeInfo{bid->GetOrderId(), ask->GetOrderId(), ask_price, quantity },
     TradeInfo{ask->GetOrderId(), bid->GetOrderId(), bid_price, quantity }
    ));
 }  

 if (!bids_.empty()){
    auto& [bid_price_level, bid_orders_level] = *bids_.begin();
    auto& Order = bid_orders_level.front();
    if (Order->GetOrderType() == OrderType::Fillandkill){
        CancelOrder(Order->GetOrderId()); // Implement CancelOrder method to remove order from order book
    }
 }
 if (!asks_.empty()){
    auto& [ask_price_level, ask_orders_level] = *asks_.begin();
    auto& Order = ask_orders_level.front();
    if (Order->GetOrderType() == OrderType::Fillandkill){
        CancelOrder(Order->GetOrderId()); // Implement CancelOrder method to remove order from order book
    } 
  } 
}
return trades;
} // End MatchOrders method


public:

 Trades ProcessNewOrder(OrderPointer order){
  if (order_map_.find(order->GetOrderId()) != order_map_.end()){
   return {}; // Break if duplicate order id's
  }

  if (order->GetOrderType() == OrderType::Fillandkill && !CanMatch(order->GetOrderSide(), order->GetPrice())){
   return {}; // FOK order cannot be matched, so ignore it
  }

  OrderPointers::iterator iterator;

  if (order->GetOrderSide() == OrderSide::Buy){
   auto& orders = bids_[order->GetPrice()]; // Access or create list of orders at this price level
   orders.push_back(order); // Add order to the list
   iterator = std::prev(orders.end()); // Get iterator to the newly added order
  }
  else {
   auto& orders = asks_[order->GetPrice()]; // Access or create list of orders at this price level
   orders.push_back(order); // Add order to the list
   iterator = std::prev(orders .end()); // Get iterator to the newly added order
  }
  order_map_.insert({order->GetOrderId(), OrderEntry{order, iterator}}); // Insert into order map for quick access
  return MatchOrders(); // Attempt to match orders after adding the new order
 }



void CancelOrder(OrderId order_id){
 if (order_map_.find(order_id) == order_map_.end()){
   throw std::logic_error("CancelOrder: Order ID " + std::to_string(order_id) + " does not exist");
 }
 const auto& [order, orderiterator] = order_map_.at(order_id);
 order_map_.erase(order_id);

 if (order->GetOrderSide() == OrderSide::Buy){
   auto& orders = bids_[order->GetPrice()]; // Access list of orders at this price level
   orders.erase(orderiterator); // Remove order from the list at its price level
   if (orders.empty()){
       bids_.erase(order->GetPrice()); // Remove price level if no orders remain
   }
 }
 else {
   auto& orders = asks_[order->GetPrice()]; // Access list of orders at this price level
   orders.erase(orderiterator); // Remove order from the list at its price level
   if (orders.empty()){
       asks_.erase(order->GetPrice()); // Remove price level if no orders remain
   }
 }
 
  // Remove order from the list at its price level


}







}; // Closing brace for OrderBook class

