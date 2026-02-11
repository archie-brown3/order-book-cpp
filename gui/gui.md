# Building the Order Book GUI - Practical Guide

## Understanding ImGui's Immediate Mode

### How ImGui Actually Works

Every frame (60 times per second), you tell ImGui what to draw:

```cpp
while (window is open) {
    ImGui::NewFrame();  // Start drawing
    
    // You write this code:
    ImGui::Begin("My Window");
    ImGui::Text("Price: $100");
    if (ImGui::Button("Buy")) {
        // This runs when clicked
    }
    ImGui::End();
    
    ImGui::Render();  // Display everything
}
```

**Key Concept:** You don't create widgets once and update them. You recreate them every frame. ImGui handles all the state internally.

---

## Building the Order Book Table - Step by Step

### Step 1: Display a Single Price

Open `test.cpp` and replace the demo window code with:

```cpp
// Inside your main loop, after ImGui::NewFrame()
ImGui::Begin("Order Book");

ImGui::Text("11232.5");  // Just show one price

ImGui::End();
```

Run it. You'll see a window with one number.

### Step 2: Add Three Columns (Price, Quantity, Total)

```cpp
ImGui::Begin("Order Book");

ImGui::Columns(3);  // Split into 3 columns

// Headers
ImGui::Text("Price");    ImGui::NextColumn();
ImGui::Text("Quantity"); ImGui::NextColumn();
ImGui::Text("Total");    ImGui::NextColumn();

ImGui::Separator();  // Line under headers

// One row of data
ImGui::Text("11232.5"); ImGui::NextColumn();
ImGui::Text("0.9018");  ImGui::NextColumn();
ImGui::Text("17.75");   ImGui::NextColumn();

ImGui::Columns(1);  // Back to single column

ImGui::End();
```

Run it. You now have a table with headers and one row.

### Step 3: Add Multiple Rows with a Loop

```cpp
ImGui::Begin("Order Book");

ImGui::Columns(3);
ImGui::Text("Price");    ImGui::NextColumn();
ImGui::Text("Quantity"); ImGui::NextColumn();
ImGui::Text("Total");    ImGui::NextColumn();
ImGui::Separator();

// Array of data (hardcoded for now)
float prices[] = {11232.5, 11232.4, 11232.0, 11231.6};
float quantities[] = {0.9018, 0.0060, 0.8910, 1.0970};
float totals[] = {17.75, 16.85, 16.21, 15.32};

// Loop through and display
for (int i = 0; i < 4; i++) {
    ImGui::Text("%.1f", prices[i]);      ImGui::NextColumn();
    ImGui::Text("%.4f", quantities[i]);  ImGui::NextColumn();
    ImGui::Text("%.2f", totals[i]);      ImGui::NextColumn();
}

ImGui::Columns(1);
ImGui::End();
```

Run it. You have a table with 4 rows of data.

### Step 4: Color the Text Red (for Asks)

```cpp
ImGui::Begin("Order Book");

// Set text color to red
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
//                                           R     G     B     A

ImGui::Columns(3);
ImGui::Text("Price"); ImGui::NextColumn();
ImGui::Text("Quantity"); ImGui::NextColumn();
ImGui::Text("Total"); ImGui::NextColumn();
ImGui::Separator();

for (int i = 0; i < 4; i++) {
    ImGui::Text("%.1f", prices[i]); ImGui::NextColumn();
    ImGui::Text("%.4f", quantities[i]); ImGui::NextColumn();
    ImGui::Text("%.2f", totals[i]); ImGui::NextColumn();
}

ImGui::Columns(1);
ImGui::PopStyleColor();  // Restore original color

ImGui::End();
```

Run it. Red text for sell orders.

### Step 5: Add the Current Price Display

```cpp
ImGui::Begin("Order Book");

// Red asks section (from above)
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
// ... asks table code ...
ImGui::PopStyleColor();

// Current price in the middle
ImGui::Separator();
ImGui::SetWindowFontScale(2.0f);  // Make it bigger
ImGui::Text("11224.9 USD");
ImGui::SetWindowFontScale(1.0f);  // Back to normal size
ImGui::Separator();

// Green bids section
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
// ... bids table code (same as asks but with different data) ...
ImGui::PopStyleColor();

ImGui::End();
```

Run it. You have asks (red), current price (large), bids (green).

---

## Connecting Real Data

### Step 6: Create a Simple Data Structure

At the top of your `test.cpp` file, before `main()`:

```cpp
struct Order {
    double price;
    double quantity;
    double total;
};

// Sample data (you'll replace this with your order book later)
std::vector<Order> asks = {
    {11232.5, 0.9018, 17.75},
    {11232.4, 0.0060, 16.85},
    {11232.0, 0.8910, 16.21},
    {11231.6, 1.0970, 15.32}
};

std::vector<Order> bids = {
    {11224.9, 9.1303, 9.13},
    {11224.8, 0.6682, 9.79},
    {11224.2, 0.5310, 10.32},
    {11223.9, 0.6683, 10.99}
};
```

### Step 7: Use the Data Structure in Your GUI

```cpp
// Inside your main loop
ImGui::Begin("Order Book");

// Asks (Red)
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
ImGui::Columns(3);
ImGui::Text("Price"); ImGui::NextColumn();
ImGui::Text("Quantity"); ImGui::NextColumn();
ImGui::Text("Total"); ImGui::NextColumn();
ImGui::Separator();

for (const auto& ask : asks) {  // Loop through vector
    ImGui::Text("%.1f", ask.price); ImGui::NextColumn();
    ImGui::Text("%.4f", ask.quantity); ImGui::NextColumn();
    ImGui::Text("%.2f", ask.total); ImGui::NextColumn();
}

ImGui::Columns(1);
ImGui::PopStyleColor();

// Current price
ImGui::Separator();
ImGui::SetWindowFontScale(2.0f);
ImGui::Text("11224.9 USD");
ImGui::SetWindowFontScale(1.0f);
ImGui::Separator();

// Bids (Green)
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
ImGui::Columns(3);
ImGui::Text("Price"); ImGui::NextColumn();
ImGui::Text("Quantity"); ImGui::NextColumn();
ImGui::Text("Total"); ImGui::NextColumn();
ImGui::Separator();

for (const auto& bid : bids) {  // Loop through vector
    ImGui::Text("%.1f", bid.price); ImGui::NextColumn();
    ImGui::Text("%.4f", bid.quantity); ImGui::NextColumn();
    ImGui::Text("%.2f", bid.total); ImGui::NextColumn();
}

ImGui::Columns(1);
ImGui::PopStyleColor();

ImGui::End();
```

Run it. Same result, but now using actual data structures.

---

## Making the Data Update

### Step 8: Simulate Live Updates

Add this inside your main loop, BEFORE the ImGui code:

```cpp
// Simulate price changes
static double time_counter = 0.0;
time_counter += 0.016;  // ~60 FPS

if ((int)(time_counter * 10) % 10 == 0) {  // Update every 10 frames
    // Randomly modify the first ask price
    asks[0].price += (rand() % 3 - 1) * 0.1;  // -0.1, 0, or +0.1
}

// Now draw the GUI (ImGui code from above)
```

Run it. The top ask price will change randomly.

---

## Adding a Second Window for Charts

### Step 9: Create a Chart Window

After your order book window code, add:

```cpp
ImGui::Begin("Price Chart");

// Placeholder - we'll add real chart later
ImGui::Text("Chart will go here");
ImGui::Text("Size: 800x600");

ImGui::End();
```

Run it. You now have two windows you can move around.

### Step 10: Draw a Simple Line

```cpp
ImGui::Begin("Price Chart");

// Create some fake price data
static float price_history[100];
static int history_size = 0;

// Add a point every frame
if (history_size < 100) {
    price_history[history_size] = 11200.0f + (rand() % 100);
    history_size++;
}

// Draw it as a line graph
ImGui::PlotLines("Price", price_history, history_size, 
                 0, nullptr, 11150.0f, 11250.0f, ImVec2(0, 200));

ImGui::End();
```

Run it. You'll see a simple line chart filling up with random prices.

---

## Making It Look Like the Screenshot

### Step 11: Window Positioning and Sizing

In your main loop, after `ImGui::NewFrame()`:

```cpp
// Set order book window position and size
ImGui::SetNextWindowPos(ImVec2(600, 20), ImGuiCond_FirstUseEver);
ImGui::SetNextWindowSize(ImVec2(400, 700), ImGuiCond_FirstUseEver);
ImGui::Begin("Order Book");
// ... order book code ...
ImGui::End();

// Set chart window position and size
ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
ImGui::SetNextWindowSize(ImVec2(560, 700), ImGuiCond_FirstUseEver);
ImGui::Begin("Price Chart");
// ... chart code ...
ImGui::End();
```

### Step 12: Dark Background

In your initialization (before main loop):

```cpp
ImGui::StyleColorsDark();  // Use dark theme

// Customize further
ImGuiStyle& style = ImGui::GetStyle();
style.WindowRounding = 0.0f;  // Square corners
style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
```

### Step 13: Right-Align Numbers

For the quantity and total columns:

```cpp
// In your table loop
ImGui::Text("%.1f", ask.price); 
ImGui::NextColumn();

// Right-align the quantity
ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 
                     ImGui::GetColumnWidth() - 
                     ImGui::CalcTextSize("0.00000000").x - 10);
ImGui::Text("%.8f", ask.quantity); 
ImGui::NextColumn();

// Right-align the total
ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 
                     ImGui::GetColumnWidth() - 
                     ImGui::CalcTextSize("00.00000000").x - 10);
ImGui::Text("%.8f", ask.total); 
ImGui::NextColumn();
```

---

## Connecting Your Order Book Class

### Step 14: Replace Mock Data with Your Order Book

Assuming you have an existing `OrderBook` class:

```cpp
// At the top of test.cpp
#include "../src/OrderBook.h"  // Your existing order book

// In main(), before the loop
OrderBook book;
// ... populate book with initial data ...

// In the main loop
ImGui::Begin("Order Book");

// Get current data from your order book
auto asks = book.getAsks(15);  // Get top 15 asks
auto bids = book.getBids(15);  // Get top 15 bids

// Draw asks
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
ImGui::Columns(3);
// ... headers ...
for (const auto& ask : asks) {
    ImGui::Text("%.1f", ask.getPrice()); ImGui::NextColumn();
    ImGui::Text("%.8f", ask.getQuantity()); ImGui::NextColumn();
    ImGui::Text("%.8f", ask.getTotal()); ImGui::NextColumn();
}
ImGui::Columns(1);
ImGui::PopStyleColor();

// Current price
double current_price = book.getMidPrice();
ImGui::Separator();
ImGui::SetWindowFontScale(2.0f);
ImGui::Text("%.1f USD", current_price);
ImGui::SetWindowFontScale(1.0f);
ImGui::Separator();

// Draw bids (same pattern)
// ...

ImGui::End();
```

---

## Quick Reference: Common ImGui Patterns

### Creating a Window
```cpp
ImGui::Begin("Window Title");
// ... content ...
ImGui::End();
```

### Text Display
```cpp
ImGui::Text("Static text");
ImGui::Text("Number: %.2f", 123.456);  // Formatted
```

### Buttons
```cpp
if (ImGui::Button("Click Me")) {
    // Do something
}
```

### Columns
```cpp
ImGui::Columns(3);
ImGui::Text("Col 1"); ImGui::NextColumn();
ImGui::Text("Col 2"); ImGui::NextColumn();
ImGui::Text("Col 3"); ImGui::NextColumn();
ImGui::Columns(1);  // End columns
```

### Colors
```cpp
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(R, G, B, A));
// ... colored content ...
ImGui::PopStyleColor();
```

### Sliders
```cpp
static float value = 0.5f;
ImGui::SliderFloat("Zoom", &value, 0.0f, 1.0f);
```

---

## Build and Run Checklist

1. Make sure your Makefile is correct (IMGUI_DIR = ../external/imgui)
2. From the `gui/` directory, run: `make`
3. Run the executable: `./example_glfw_opengl3`
4. See your changes immediately
5. Press `Ctrl+C` in terminal to stop

## Debugging Tips

**Window not showing?**
- Check you called `ImGui::Begin()` and `ImGui::End()`

**Colors wrong?**
- RGBA format: Red, Green, Blue, Alpha (1.0 = opaque)
- Make sure you called `ImGui::PopStyleColor()` after `Push`

**Data not updating?**
- Print to console first: `printf("Price: %.2f\n", price);`
- Make sure you're modifying data BEFORE drawing

**Crash?**
- Check array bounds in loops
- Make sure pointers aren't null

---

## Next Steps

1. Start with Step 1 - display one price
2. Work through each step, running after each change
3. Once you have the basic table, connect your real order book
4. Add the chart using ImPlot (separate guide needed)
5. Polish with colors, sizing, and controls

The key is **iterate quickly** - make small changes, run, see results, repeat.