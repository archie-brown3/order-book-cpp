#ifndef ORDERS_H
#define ORDERS_H

#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>
#include <vector>


// Type aliases
using Price =  std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;
using LevelInfos = std::vector<struct LevelInfo>;

// Enum for order types and sides
enum class OrderType{
    Goodtillcancel,
    Fillandkill,
};

enum class OrderSide{
    Buy,
    Sell,
};
// todo: Add a vector of OrderId to store multiple orders at the same price level? 


// Aggregates depth level information
struct LevelInfo {
 Price price;
 Quantity quantity;
};


class Order{
 public:
  Order(OrderId id, OrderSide side, Price price, Quantity quantity, OrderType type):
   id_ {id},
   side_ {side},
   price_ {price},
   initial_quantity_ {quantity},     // Remaining quantity = initial quantity at order creation
   remaining_quantity_ {quantity},
   order_type_ {type}
   { }

   OrderId GetOrderId() const { return id_ ; }
   OrderSide GetOrderSide() const { return side_ ; }
   Price GetPrice() const { return price_ ; }
   OrderType GetOrderType() const { return order_type_ ; } 
   Quantity GetInitialQuantity() const { return initial_quantity_ ; }
   Quantity GetRemainingQuantity() const { return remaining_quantity_ ; }
   Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity() ; }
   bool IsFilled() const { return GetRemainingQuantity() == 0 ; }

   // Lowest quantity between both orders = quantity to fill both orders
   void Fill(Quantity quantity){
       if(quantity > remaining_quantity_){
           throw std::logic_error("Order (" + std::to_string(GetOrderId()) + ") Fill quantity exceeds remaining quantity");
       }
       remaining_quantity_ -= quantity;
   }

 private:
 OrderType order_type_;
 OrderId id_;
 OrderSide side_;
 Price price_;
 Quantity initial_quantity_;
 Quantity remaining_quantity_;
};

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

// Pointer Aliasing:
// Single orders will be managed through multiple Data structures, so shared_ptr is appropriate to avoid dangling pointers.
//shared_ptr is a smart pointer with shared ownership;
//copies increment a reference count, and the Order is deleted when the last shared_ptr goes away.

using OrderPointer = std::shared_ptr<Order>; 
using OrderPointers = std::list<OrderPointer>;   // Uses a doubly linked list to store orders at the same price level (can use a vector if we want to optimize for cache locality


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

using Trades = std::vector<Trade>;

#endif // ORDERS_H