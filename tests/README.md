# Unit Tests

This directory contains sample unit tests for the sensor components. Tests are
written using the Unity framework provided by ESP-IDF and build as a standalone
application.

To build the tests locally:

```
idf.py -C tests build
```

Running the resulting firmware requires an ESP32 device and the unit test
infrastructure.
