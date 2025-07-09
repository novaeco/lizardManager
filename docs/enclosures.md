# Enclosures

The firmware keeps a list of terrariums in `/spiffs/enclosures.csv`. Each line stores:

- Identifier
- Name
- Location description
- Minimum and maximum temperature (C)
- Minimum and maximum humidity (%)
- Sensor node identifiers separated by colons
- Unix timestamp of the last cleaning
- Cleaning interval in days
- Unix timestamp of the last substrate change
- Substrate change interval in days

Use the `enclosures` component APIs to manage this list. After calling
`enclosures_init`, you can add or retrieve enclosures and check when
cleaning or substrate changes are due with
`enclosures_cleaning_due` and `enclosures_substrate_due`.
