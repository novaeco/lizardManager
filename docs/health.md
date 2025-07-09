# Health Records

The firmware stores weight, length and health events in `/spiffs/health.csv`.
Each measurement entry contains:

- Unix timestamp (seconds)
- Animal identifier
- Weight in grams
- Length in centimeters

Health events store:

- Unix timestamp
- Animal identifier
- Event description
- Attachment file names separated by colons

Copy attachment files such as photos or vet documents to `/spiffs/attachments`
using `idf.py spiffs_upload`. The log references these names so you can retrieve
the files later with `idf.py spiffs_download`.

Use the `health` component APIs to record data.
