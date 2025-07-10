# Purchase and Sales Ledger

The firmware records transactions in `/spiffs/ledger.csv`. Each line contains:

- Unix timestamp
- `0` for a purchase or `1` for a sale
- Item or animal identifier
- Buyer details
- Seller details
- Price in euros

Use the `ledger` component APIs to add entries and retrieve the log for
accounting or tax purposes.
