#!/usr/bin/env python3
"""Generate legally compliant PDF documents.

Usage: generate_pdf_docs.py TYPE OUTPUT.pdf [ARG] [--storage spiffs|sdcard]
TYPE may be 'breeding', 'capacity', 'sales', or 'cession'. For 'cession', provide an item ID as ARG.
"""
import csv
import os
import sys
from datetime import datetime

try:
    from fpdf import FPDF
except ImportError:
    sys.exit("This tool requires the 'fpdf' package. Install with 'pip install fpdf'.")

TEMPLATES = {
    'breeding': 'tools/templates/breeding_log.txt',
    'capacity': 'tools/templates/capacity_certificate.txt',
    'sales': 'tools/templates/sales_certificate.txt',
    'cession': 'tools/templates/cession_certificate.txt',
}

def read_csv(path):
    try:
        with open(path, 'r') as f:
            return list(csv.reader(f))
    except FileNotFoundError:
        return []

def load_template(path):
    try:
        with open(path, 'r') as f:
            return f.read()
    except FileNotFoundError:
        return ''

def generate_breeding(pdf):
    animals = read_csv('spiffs/animals.csv')
    feedings = read_csv('spiffs/feedings.csv')
    text = load_template(TEMPLATES['breeding'])
    pdf.add_page()
    pdf.set_font('Arial', 'B', 16)
    pdf.cell(0, 10, 'Breeding Log', ln=True)
    pdf.set_font('Arial', '', 12)
    pdf.multi_cell(0, 10, text)
    pdf.ln(5)
    for row in animals:
        if len(row) >= 2:
            pdf.cell(0, 10, f'Animal {row[0]} - {row[1]}', ln=True)
    pdf.ln(5)
    pdf.cell(0, 10, f'Total feedings: {len(feedings)}', ln=True)

def generate_capacity(pdf):
    text = load_template(TEMPLATES['capacity'])
    pdf.add_page()
    pdf.set_font('Arial', 'B', 16)
    pdf.cell(0, 10, 'Certificate of Capacity', ln=True)
    pdf.set_font('Arial', '', 12)
    pdf.multi_cell(0, 10, text)

def generate_sales(pdf):
    text = load_template(TEMPLATES['sales'])
    pdf.add_page()
    pdf.set_font('Arial', 'B', 16)
    pdf.cell(0, 10, 'Sales Certificate', ln=True)
    pdf.set_font('Arial', '', 12)
    pdf.multi_cell(0, 10, text)

def generate_cession(pdf, item_id):
    text = load_template(TEMPLATES['cession'])
    ledger = read_csv('spiffs/ledger.csv')
    entry = None
    for row in reversed(ledger):
        if len(row) >= 6 and row[2] == item_id and row[1] == '1':
            entry = row
            break
    pdf.add_page()
    pdf.set_font('Arial', 'B', 16)
    pdf.cell(0, 10, 'Cession Certificate', ln=True)
    pdf.set_font('Arial', '', 12)
    pdf.multi_cell(0, 10, text)
    if entry:
        pdf.ln(5)
        ts = datetime.fromtimestamp(int(entry[0])).strftime('%Y-%m-%d')
        pdf.cell(0, 10, f'Date: {ts}', ln=True)
        pdf.cell(0, 10, f'Animal: {entry[2]}', ln=True)
        pdf.cell(0, 10, f'Buyer: {entry[3]}', ln=True)
        pdf.cell(0, 10, f'Seller: {entry[4]}', ln=True)
        pdf.cell(0, 10, f'Price: {entry[5]} EUR', ln=True)

def main(doc_type, out_pdf, storage, arg=None):
    pdf = FPDF()
    if doc_type == 'breeding':
        generate_breeding(pdf)
    elif doc_type == 'capacity':
        generate_capacity(pdf)
    elif doc_type == 'sales':
        generate_sales(pdf)
    elif doc_type == 'cession':
        if not arg:
            sys.exit('Cession requires an item ID argument')
        generate_cession(pdf, arg)
    else:
        sys.exit('Unknown document type: ' + doc_type)
    os.makedirs(f'{storage}/docs', exist_ok=True)
    out_path = os.path.join(storage, 'docs', out_pdf)
    pdf.output(out_path)
    print('Written', out_path)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: generate_pdf_docs.py TYPE OUTPUT.pdf [ARG] [--storage spiffs|sdcard]')
        sys.exit(1)
    doc_type = sys.argv[1]
    out_pdf = sys.argv[2]
    storage = 'spiffs'
    arg = None
    if '--storage' in sys.argv:
        idx = sys.argv.index('--storage')
        if idx + 1 < len(sys.argv):
            storage = sys.argv[idx + 1]
        sys.argv.pop(idx)  # remove flag
        sys.argv.pop(idx)  # remove value
    if len(sys.argv) >= 4:
        arg = sys.argv[3]
    main(doc_type, out_pdf, storage, arg)
