# Permit Tracking

The firmware stores permit information in `/spiffs/permits.csv`. Each line contains:

- Animal identifier
- Permit type (e.g. `CITES`)
- Unix timestamp when the permit was issued
- Unix timestamp when the permit expires
- Unix timestamp of the last inspection
- Inspection interval in days

Use the `permits` component APIs to manage this list. After calling `permits_init`, you can check when renewals or inspections are due with `permits_due` and `permits_inspection_due`.
