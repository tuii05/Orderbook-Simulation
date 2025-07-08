## Order Book Simulation in C++

The program can be started with `Main.exe`. You can modify anything in the code, but after that you need to start `CreateNewMain.bat` to verify your new changes.

The order book has the following built-in features:

- Supports Market and Limit orders and handles them properly based on FIFO algorithm (price, time priority).
- Contains the following implemented Time-in-Force types: GoodTillCancel, FillAndKill.
- Real-time display of bids and asks orders, new orders are displayed with real-time.
- User interaction with a simple terminal-based menu.
- Should work with not too large data size limits as well. (Tested with 1 million data in the container, there were some lags with this much data when doing 10^6/10^7 buys/sells at once for instance)

To do:
- Improve time complexity -> for instance make map<OrderKey, LevelInfos> instead of map<Quantity, LevelInfos> and define a new operator> for the OrderKey so it would be sorted by both quantity and price and remove minimum findings.
