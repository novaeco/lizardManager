# Registering Reptiles

The firmware keeps a record of each reptile in `/spiffs/animals.csv`. Every entry stores:

- Unique identifier
- Species
- Sex
- Birth date (`YYYY-MM-DD`)
- Legal status or permit info
- Photo file names separated by colons

Use the `animals` component APIs to manage this list. After calling `animals_init`, you can add animals with `animals_add`, read them with `animals_get`, modify them with `animals_update`, and remove them with `animals_remove`.

Changes are saved automatically to SPIFFS so the list persists across reboots.
