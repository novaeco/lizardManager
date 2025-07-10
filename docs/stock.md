# Stock Management

The firmware stores inventory data in `/spiffs/stock.csv`. Each line contains:

- Item name
- Quantity (integer)
- Expiration date as Unix timestamp

Use the `stock` component APIs to add, update and query inventory items. Helper
functions report when an item is below a quantity threshold or expiring soon.
