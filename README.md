## Order Book Simulation in C++ - V1.1

The program can be started via `Main.exe`. In case of modifying the code,  `CreateNewMain.bat` should be started to verify the new changes.

The order book has the following built-in features:

- Supports Market and Limit orders and handles them properly based on FIFO algorithm (price, time priority).
- Contains the following implemented Time-in-Force types: GoodTillCancel, FillAndKill.
- Real-time display of bids and asks orders, new orders are displayed with real-time.
- User interaction with a simple terminal-based menu.
- Changeable limit on OrderBoook size.

To do:
- multithreading.
