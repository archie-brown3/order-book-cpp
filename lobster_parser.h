// #ifdef LOBSTER_PARSER_H
// #define LOBSTER_PARSER_H

#include "orders.h"
#include <vector>
#include <string>



enum class LobsterEventType {
    NewOrder = 1,           // Submission of new limit order
    PartialCancel = 2,      // Partial cancellation
    FullCancel = 3,         // Full deletion/cancellation
    VisibleExecution = 4,   // Execution of visible limit order
    HiddenExecution = 5,    // Execution of hidden limit order
    Cross = 6,              // Cross (matches multiple orders)
    TradingHalt = 7         // Trading halt indicator
};


struct lobster_order
{

    double timestamp;
    LobsterEventType type;     // Csv uses int 1-7 to represent order types
    OrderId order_id;
    Quantity size;
    Price price;
    int OrderSide;
    

}

#endif //LOBSTER_PARSER_H