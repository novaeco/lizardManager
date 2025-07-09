# Contributing to Lizard Manager

Thank you for considering a contribution! The following guidelines help keep the
project consistent and easy to maintain. Please read our
[Code of Conduct](CODE_OF_CONDUCT.md) to understand the expectations for
participant behavior.

## Coding Style

* Use **4 spaces** per indentation level; do not use tabs.
* Limit lines to **100 characters** where possible.
* Follow typical ESP-IDF C conventions (lowercase `snake_case` for functions and variables).
* Document public functions with brief comments explaining parameters and return values.
## Branch Naming

* Use `feature/<description>` for new features.
* Use `fix/<description>` for bug fixes.
* Optionally include issue numbers, e.g. `feature/123-short-desc`.


## Pull Request Process

1. Fork the repository and create a dedicated branch for your changes.
2. Make your edits with clear, descriptive commit messages.
3. Keep commits focused on a single topic.
4. Push the branch to your fork and open a pull request against `main`.
5. Describe the motivation for the change and reference related issues if applicable.
6. One or more maintainers will review your submission and may request updates.

## Testing Requirements

Before submitting a pull request, verify that the firmware and unit tests build
successfully:

```bash
idf.py build
idf.py -C tests build
```

The CI workflow will run the same commands automatically on GitHub Actions.

## Good Practices

* Format new code consistently with the surrounding files.
* Include documentation or unit tests when adding features.
* Rebase your branch if it falls behind `main` to simplify review.
