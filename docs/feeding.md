# Feeding Logs

The firmware stores meal history in `/spiffs/feedings.csv`. Each line contains:

- Unix timestamp (seconds)
- Prey description
- Weight change in grams
- `1` if the animal refused the meal, otherwise `0`

Use the `feeding` component APIs to record feedings and to configure reminder intervals.
Run `tools/analyze_logs.py <prefix>` to generate PNG graphs of the growth curve and feeding frequency based on this log.
