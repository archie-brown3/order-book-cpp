
## Order book:
 - Maintains two organised lists of limit orders (buy/sell at specific price):
 - Bids (Buy): Sorted from highest price to lowest (the most aggressive buyer is at the top).
 - Asks (Sell): Sorted from lowest price to highest (the most aggressive seller is at the top).

- std::map	Stores Price Levels	Keeps prices sorted automatically so you always know the "Best Bid" and "Best Ask" (O(logN) access).

- std::list	Stores Orders at a Price	Allows you to add/remove orders in the middle of a price level in O(1) time without moving other orders.

- std::unordered_map	The "Lookup" Table	Maps OrderId to the actual order. This lets you cancel or modify an order instantly (O(1)) without searching the whole book.

