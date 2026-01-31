#include<iostream>
#include <map>
#include <list>
#include <stack>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>


enum class OrderType{
    Goodtillcancel,
    Fillandkill,
};

enum Class OrderSide{
    Buy,
    Sell,
};

using Price =  std::int32_t
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

// Add a vector of OrderId to store multiple orders at the same price level? 

struct LevelInfo {
    Price price;
    Quantity Quantity;
}

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

  std::map<Price, OrderPointers, std::greater<Price>> bids_ ; // Map of price levels to lists of buy orders (highest price first)
  

}








int main(){


    return 0;
}