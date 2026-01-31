#include<iostream>
#include <map>
#include <list>
#include <stack>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <queue>


enum class OrderType{
    Goodtillcancel,
    Fillandkill,
};

enum class OrderSide{
    Buy,
    Sell,
};

using Price =  std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

// Add a vector of OrderId to store multiple orders at the same price level? 

struct LevelInfo {
    Price price;
    Quantity quantity;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos{

    public:
        // Constructor for OrderBookLevelInfos
        OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks): 
        bids_ {bids},
        asks_ {asks}
        { }
        // Public API
        const LevelInfos& getBids() const { return bids_; }
        const LevelInfos& getAsks() const { return asks_; }
    private:
        LevelInfos bids_;
        LevelInfos asks_;
};

class Order{
 public:
  Order(OrderId id, OrderSide side, Price price, Quantity quantity, OrderType type):
   id_ {id},
   side_ {side},
   price_ {price},
   initial_quantity_ {quantity},     // Remaining quantity = initial quantity at order creation
   remaining_quantity_ {quantity},
   type_ {type}
   { }

   OrderId GetOrderId() const { return id_ ; }
   OrderSide GetOrderSide() const { return side_ ; }
   Price GetPrice() const { return price_ ; }
   OrderType GetOrderType() const { return type_ ; } 
   Quantity GetInitialQuantity() const { return initial_quantity_ ; }
   Quantity GetRemainingQuantity() const { return remaining_quantity_ ; }
   Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity() ; }
   bool IsFilled() const { return GetRemainingQuantity == 0 ; }


   // Lowest quantity between both orders = quantity to fill both orders
   void Fill(Quantity quantity){
       if(quantity > remaining_quantity_){
           throw std::logic_error(std::format("Order ({}) Fill quantity exceeds remaining quantity", GetOrderId()));
       }
       remaining_quantity_ -= quantity;
   }
 private:
 OrderType order_type_;
 OrderId id_;
 Side side_;
 Price price_;
 Quantity initial_quantity_;
 Quantity remaining_quantity_;
};

using OrderPointer = std::shared_ptr<Order>; 

// Pointer Aliasing:
 // Single orders will be managed through multiple Data structures, so shared_ptr is appropriate to avoid dangling pointers.
 //shared_ptr is a smart pointer with shared ownership;
 //copies increment a reference count, and the Order is deleted when the last shared_ptr goes away.


using OrderPointers = std::list<OrderPointer>;   // Uses a doubly linked list to store orders at the same price level (can use a vector if we want to optimize for cache locality
 
class ModifyOrder{
 ModifyOrder(OrderId id, Quantity new_quantity):
  OrderId {id},
  side_ {side},
  price_ {price},
  Quantity {quantity},
  { }
  
  OrderId GetOrderId() const { return id_ ; }
  Price GetPrice() const { return price_ ; }
  side_ GetOrderSide() const { return side_ ; }
  Quantity GetQuantity() const { return quantity_ ; }


  OrderPointer toOrderPointer() const { 
   return std::make_shared<Order>(id_, side_, price_, quantity_, OrderType::Goodtillcancel);}

 private:
 OrderId id_;
 price price_;
 side side_;
 Quantity quantity_;
};


// Reperesent an matched object between two orders with a trade object: use object for bid and ask orders
struct TradeInfo {
    OrderId bid_order_id;
    OrderId ask_order_id;
    Price price;
    Quantity quantity;
};

class Trade{
 public:
  Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade):
   bid_trade_ {bidTrade},
   ask_trade_ {askTrade}
   { }

   const TradeInfo& GetBidTradeInfo() const { return bid_trade_ ; }
   const TradeInfo& GetAskTradeInfo() const { return ask_trade_ ; }

   private:
   TradeInfo bid_trade_;
   TradeInfo ask_trade_;
};

using Trades = sdtd::vector<Trade>;












// Use map to represent bids and asks 
// Access a given order by its OrderId in O(1) time
class OrderBook{
 // represent order / location in order book:
 private: 
  struct OrderEntry{
   OrderPointer order_pointer{nullptr};  // shared pointer to the order
   OrderPointers::iterator location_;  // iterator to the order's position in the list at its price level
  };

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
}












// Match method to match incoming orders against existing orders

Trades MatchOrders()
{
 Trades trades;
 trades.reserve(order_map_.size()); // Pre-allocate space for trades to minimize reallocations

 while(1){
  if (bids_.empty() || asks_.empty()){
   break; // No more possible matches
  }
  auto& [bid_price, bid_orders] = *bids_.begin(); // Highest bid price level
  auto& [ask_price, ask_orders] = *asks_.begin(); // Lowest ask price level

  if (bid_price < ask_price){
   break; // No more matches possible

  while (bids_.size() && asks_.size()){
   auto& bid = bids.front();
   auto& ask = asks.front();

   Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity()); //? 

   bid.fill(quantity);
   ask.fill(quantity);

    if (bid.IsFilled())
     {
      bids.pop_front(); // Remove filled bid order from queue 

      }
    if (ask.IsFilled())
     {
      asks.pop_front(); // Remove filled ask order from queue 
     }

    if (bids_.empty())
     {
      bids_.erase(bid_price);
     }
    if (asks_.empty())
     {
      asks_.erase(ask_price);
     
     }
     trades.push_back(Trade(
      TradeInfo{bid->GetOrderId(), ask->GetOrderId(), ask_price, quantity}));
      TradeInfo{ask->GetOrderId(), bid->GetOrderId(), bid_price, quantity})); 
   }
  }
 }
}  


int main(){


    return 0;
}