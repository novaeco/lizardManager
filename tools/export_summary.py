#!/usr/bin/env python3
"""Generate a simple PDF summary of health and feeding logs."""
import csv
import sys
from datetime import datetime

try:
    from fpdf import FPDF
except ImportError:
    sys.exit("This tool requires the 'fpdf' package. Install with 'pip install fpdf'.")

def read_csv(path):
    try:
        with open(path, 'r') as f:
            return list(csv.reader(f))
    except FileNotFoundError:
        return []

def main(out_pdf):
    animals = read_csv('spiffs/animals.csv')
    feedings = read_csv('spiffs/feedings.csv')
    health = read_csv('spiffs/health.csv')
    ledger = read_csv('spiffs/ledger.csv')

    pdf = FPDF()
    pdf.add_page()
    pdf.set_font('Arial', 'B', 16)
    pdf.cell(40, 10, 'Lizard Manager Summary')
    pdf.ln(20)

    pdf.set_font('Arial', '', 12)
    pdf.cell(40, 10, 'Animals: %d' % len(animals))
    pdf.ln(10)

    pdf.cell(40, 10, 'Feedings: %d' % len(feedings))
    pdf.ln(10)

    pdf.cell(40, 10, 'Health records: %d' % len(health))
    pdf.ln(10)

    purchases = sum(float(row[5]) for row in ledger if len(row) >= 6 and row[1] == '0')
    sales = sum(float(row[5]) for row in ledger if len(row) >= 6 and row[1] == '1')
    pdf.cell(40, 10, 'Purchases total: %.2f EUR' % purchases)
    pdf.ln(10)
    pdf.cell(40, 10, 'Sales total: %.2f EUR' % sales)
    pdf.ln(10)
    pdf.cell(40, 10, 'Net: %.2f EUR' % (sales - purchases))
    pdf.ln(10)

    pdf.output(out_pdf)
    print('Written', out_pdf)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: export_summary.py output.pdf')
        sys.exit(1)
    main(sys.argv[1])
