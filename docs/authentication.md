# Authentication and Roles

User accounts are stored in `/spiffs/users.csv` on the device. Each line
contains the username, a SHA‑256 hash of the password and the assigned role.
Available roles are **owner**, **caretaker** and **vet**.

Use the `auth` component APIs to add users and to verify credentials at run time.

User actions are appended to `/spiffs/audit.csv` via the `audit` component.
Each log entry records the Unix timestamp, the username and a short action
description.
