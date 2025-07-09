# Contributing to Lizard Manager

Thank you for considering a contribution! The following guidelines help keep the
project consistent and easy to maintain.

## Coding Style

* Use **4 spaces** per indentation level; do not use tabs.
* Limit lines to **100 characters** where possible.
* Follow typical ESP-IDF C conventions (lowercase `snake_case` for functions and variables).
* Document public functions with brief comments explaining parameters and return values.

## Pull Request Process

1. Fork the repository and create a new branch for your feature or fix.
2. Make your changes with clear, descriptive commit messages.
3. Ensure that no unrelated changes are included in the branch.
4. Open a pull request against the `main` branch and describe your changes.
5. One or more maintainers will review your submission and may request updates.

## Testing Requirements

Before submitting a pull request, verify that the firmware and unit tests build
successfully:

```bash
idf.py build
idf.py -C tests build
```

The CI workflow will run the same commands automatically on GitHub Actions.
