#!/usr/bin/env python3
"""Export logs and records with AES encryption."""
import argparse
import subprocess
import time
import os


def main():
    parser = argparse.ArgumentParser(description="Create an encrypted archive of logs for inspection")
    parser.add_argument("--input", default="spiffs", help="Directory containing log files")
    parser.add_argument("--output", help="Output filename (without extension)")
    parser.add_argument("--passphrase", help="Passphrase for openssl encryption")
    args = parser.parse_args()

    timestamp = time.strftime("%Y%m%d_%H%M%S")
    base = args.output or f"logs_{timestamp}"
    archive = base + ".tar.gz"

    subprocess.check_call(["tar", "czf", archive, "-C", args.input, "."])

    passphrase = args.passphrase or os.environ.get("EXPORT_PASSPHRASE")
    if not passphrase:
        raise SystemExit("No passphrase provided. Use --passphrase or EXPORT_PASSPHRASE env var")

    enc_file = archive + ".enc"
    subprocess.check_call([
        "openssl", "enc", "-aes-256-cbc", "-pbkdf2", "-salt",
        "-pass", f"pass:{passphrase}", "-in", archive, "-out", enc_file,
    ])

    os.remove(archive)
    print(f"Encrypted export written to {enc_file}")


if __name__ == "__main__":
    main()
