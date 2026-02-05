# Order Book (C++)

A high-performance **limit order book** implementation in C++17. The engine maintains sorted bid/ask levels, matches orders in price–time priority, and supports Good-till-Cancel (GTC) and Fill-and-Kill (FOK) order types. The design is tuned for predictable complexity and clear ownership semantics, with a scaffold for replaying [LOBSTER](http://LOBSTER.wiwi.hu-berlin.de) academic order-book data.

---

## Table of Contents

- [Overview](#overview)
- [Technical Architecture](#technical-architecture)
- [Design Choices](#design-choices)
- [Order Types & Matching](#order-types--matching)
- [Build & Run](#build--run)
- [Project Layout](#project-layout)
- [Future Scope](#future-scope)

---

## Overview

The order book keeps two sides (bids and asks), each organised by **price levels**. Within a level, orders are kept in **time order** (FIFO). The engine:

- **Inserts** new orders and runs matching immediately.
- **Matches** when bid price ≥ ask price; execution uses the **maker price** (resting order’s price).
- **Cancels** by `OrderId` in constant time via an index.
- **Rejects** duplicate `OrderId`s and FOK orders that cannot be fully filled.

Trade output is a vector of `Trade` objects, each carrying both sides’ view of the fill (bid trade info and ask trade info).

---

## Technical Architecture

### Core Data Structures

Three structures work together to get the right complexity and behaviour:

| Structure | Role | Type | Complexity |
|-----------|------|------|------------|
| **Price levels** | Sorted bid/ask levels | `std::map<Price, OrderPointers, Compare>` | Best price: O(1) via `begin()`; insert/erase level: O(log N) |
| **Orders at a level** | FIFO queue per price | `std::list<OrderPointer>` | Enqueue: O(1); remove by iterator: O(1) |
| **Order index** | OrderId → order + list position | `std::unordered_map<OrderId, OrderEntry>` | Lookup / cancel: O(1) average |

- **Bids**: `std::map<Price, OrderPointers, std::greater<Price>>` so the best bid is the highest price (`bids_.begin()`).
- **Asks**: `std::map<Price, OrderPointers, std::less<Price>>` so the best ask is the lowest price (`asks_.begin()`).

`OrderEntry` holds a `shared_ptr<Order>` and the **list iterator** for that order at its price level. Cancel is O(1): lookup in the hash map, then `list::erase(iterator)` and optional `map::erase` if the level becomes empty.

### Type System

- **Fixed-width types**: `Price = std::int32_t`, `Quantity = std::uint32_t`, `OrderId = std::uint64_t` for stable layout and portability.
- **Order ownership**: `OrderPointer = std::shared_ptr<Order>`. The same order is referenced from both the level list and `OrderEntry`; no duplication of order state and no dangling references when orders are removed.
- **Trade representation**: Each match is a `Trade` with two `TradeInfo` structs (bid-side view and ask-side view), giving symmetric reporting (price, quantity, counterparty order id) for both participants.

### Matching Algorithm

- **Price–time priority**: Best bid vs best ask; within a level, `std::list` order is FIFO (`front()` is the oldest).
- **Execution price**: Trades are recorded at the **resting (maker) order’s price** (e.g. when a buy hits an ask, the trade price is the ask price).
- **Fill semantics**: `Quantity fill = min(bid.remaining, ask.remaining)`; both orders are updated via `Order::Fill()`, which enforces `fill ≤ remaining` and throws on overfill.
- **Cleanup**: Filled orders are popped from the list; empty levels are erased from the map. After each matching run, any **FOK** order that still has quantity left at the best level is **cancelled** (FOK must fill immediately or not at all).

### Robustness

- **Duplicate OrderIds**: `ProcessNewOrder` checks `order_map_` and returns an empty `Trades` vector if the id already exists; the order is not inserted.
- **FOK pre-check**: Before inserting an FOK order, `CanMatch(side, price)` ensures the book can match at that price. If not, the FOK is rejected (empty trades, order not added).
- **Cancel**: `CancelOrder(order_id)` throws `std::logic_error` if the order does not exist, so misuse is explicit.

---

## Design Choices

### Why `std::map` for price levels?

- Sorted order is required; best bid/ask are always at `begin()`.
- Insert/erase of a level is O(log N); no need to manually sort or rebalance.
- Explicit comparators (`std::greater` / `std::less`) make bid vs ask ordering clear at the type level.

### Why `std::list` for orders at a price?

- Cancels and fills remove orders from the **middle** of the queue; `list::erase(iterator)` is O(1) and invalidates only that iterator (the one stored in `OrderEntry`).
- With `std::vector`, erasing in the middle would be O(n) and would invalidate many iterators, breaking the stored iterators in `order_map_`.

Trade-off: list has worse cache locality than vector; the codebase notes that a vector could be considered later if optimising for locality and cancel patterns are acceptable.

### Why `std::unordered_map` for OrderId → OrderEntry?

- Cancel and “find order” need to be O(1) by id; hash map gives average O(1) lookup and erase.
- No need for ordered iteration by id, so `std::map` would add log cost without benefit here.

### Why `shared_ptr<Order>`?

- The same logical order lives in (1) one of the level lists and (2) the `OrderEntry` in `order_map_`. Shared ownership avoids copying order state and ensures the order is only destroyed when both references are gone. Fills are applied in place on the same object both sides reference.

### Why two `TradeInfo` per `Trade`?

- A single match is one economic event but two views (buyer vs seller). Storing both views in one `Trade` keeps reporting symmetric and avoids ambiguity (e.g. whose price, which side was aggressor) when consuming trade output.

---

## Order Types & Matching

| Type | Behaviour |
|------|-----------|
| **Good-till-Cancel (GTC)** | Inserted into the book; any unfilled quantity remains until matched or cancelled. |
| **Fill-and-Kill (FOK)** | If `CanMatch(side, price)` is false, the order is rejected (not added). If added, it is matched immediately; any remainder at the best level is then cancelled so no FOK quantity rests in the book. |

Matching is **reactive**: it runs after each new order is inserted. There is no separate “match sweep”; the book is always consistent after `ProcessNewOrder` returns.

---

## Build & Run

**Requirements:** C++17 compiler (e.g. `g++` or `clang++`).

```bash
# Build and run the in-program tests
make test

# Build only
make all

# Remove binary
make clean
```

The current `main()` in `orderbook.cpp` runs four scenarios: partial fill, no match (prices don’t cross), FOK rejected (insufficient liquidity), and cancel by id.

---

## Project Layout

| Path | Purpose |
|------|--------|
| `orders.h` | Core types: `Order`, `OrderBookLevelInfos`, `Trade`/`TradeInfo`, type aliases, `LevelInfo`. |
| `ordersApi.h` | `OrderBook` (book state, matching, `ProcessNewOrder`, `CancelOrder`), `ModifyOrder` DTO. |
| `orderbook.cpp` | `main()` and built-in test cases. |
| `lobster_parser.h` | LOBSTER event enum and `lobster_order` struct (scaffold only). |
| `context.md` | Short notes on the three main data structures. |
| `data/` | LOBSTER sample data and readme describing message/order book CSV format. |

---

## Future Scope

- **LOBSTER replay**: Implement a parser for LOBSTER message (and optionally order book) CSV files. Map LOBSTER event types (new order, partial/full cancel, visible/hidden execution, trading halt) onto `OrderBook` operations and optionally use `OrderBookLevelInfos` or similar to validate against the provided order book snapshots.
- **Modify order**: Use the existing `ModifyOrder` type and “cancel + re-insert” semantics: cancel by id, then `ProcessNewOrder(modify.toOrderPointer())` with the new price/quantity/side. Expose this as a single API (e.g. `ModifyOrder(modify)` or `ReplaceOrder(...)`) and define behaviour when the modified order would match (e.g. allow immediate match vs treat as a new limit order).
- **Depth and snapshots**: Add a method to build `OrderBookLevelInfos` from the current `bids_`/`asks_` (e.g. top N levels or full depth) for analytics, logging, or validation against LOBSTER order book files.
- **Order types**: Extend with Immediate-or-Cancel (IOC), Iceberg (display quantity), or other types as needed; matching and FOK-style cleanup patterns can be generalised.
- **Testing**: Move the current ad-hoc tests into a test framework (e.g. Google Test) and add cases for: multiple partial fills across levels, FOK that partially fills then remainder cancelled, cancel of an order that has been partially filled, and duplicate/failed cancel.
- **Performance and structure**: If needed, consider a vector-based level representation for better cache locality (with a strategy for iterator stability on cancel), or dedicated allocators for orders/trades to reduce allocations in hot paths.

This README reflects the current design and intended direction; the codebase is in a state where LOBSTER integration and richer APIs can be added on top of the existing engine without changing its core invariants.
