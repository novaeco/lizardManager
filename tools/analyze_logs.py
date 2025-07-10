#!/usr/bin/env python3
"""Generate graphs from feeding and reading logs."""
import csv
import sys
from datetime import datetime

try:
    import matplotlib.pyplot as plt
except ImportError:
    sys.exit("This tool requires 'matplotlib'. Install with 'pip install matplotlib'.")

FEEDINGS_PATH = 'spiffs/feedings.csv'
READINGS_PATH = 'spiffs/readings.csv'


def read_csv(path):
    try:
        with open(path, 'r') as f:
            return list(csv.reader(f))
    except FileNotFoundError:
        return []


def plot_growth(feedings, out_prefix):
    dates = []
    weights = []
    total = 0.0
    for row in feedings:
        if len(row) < 4:
            continue
        ts = datetime.fromtimestamp(int(row[0]))
        wt = float(row[2])
        total += wt
        dates.append(ts)
        weights.append(total)
    if not dates:
        print('No feeding data found')
        return
    plt.figure()
    plt.plot(dates, weights, marker='o')
    plt.xlabel('Date')
    plt.ylabel('Cumulative Weight Change (g)')
    plt.title('Growth Curve')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(out_prefix + '_growth.png')
    plt.close()


def plot_frequency(feedings, out_prefix):
    if len(feedings) < 2:
        print('Not enough feedings for frequency graph')
        return
    timestamps = [int(row[0]) for row in feedings if len(row) >= 4]
    timestamps.sort()
    intervals = []
    prev = timestamps[0]
    for ts in timestamps[1:]:
        intervals.append((ts - prev) / 86400.0)
        prev = ts
    plt.figure()
    plt.hist(intervals, bins=range(1, 31), edgecolor='black')
    plt.xlabel('Days Between Feedings')
    plt.ylabel('Count')
    plt.title('Feeding Frequency')
    plt.tight_layout()
    plt.savefig(out_prefix + '_frequency.png')
    plt.close()


def main(prefix):
    feedings = read_csv(FEEDINGS_PATH)
    plot_growth(feedings, prefix)
    plot_frequency(feedings, prefix)
    print('Graphs written with prefix', prefix)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: analyze_logs.py OUTPUT_PREFIX')
        sys.exit(1)
    main(sys.argv[1])
