import os
import sys


def lint_file(filename):

    messages = []

    with open(filename, 'r') as file:

        for i, line in enumerate(file):

            c = [ord(c) < 128 for c in line]

            if not all(c):
                marker = ''.join([' ' if x else '^' for x in c])
                messages.append((f"Non-ASCII characters found:\n{line[:-1]}\n{marker}", i))

            if len(line) > 1 and line[-2] in {' '}:
                messages.append(("Whitespace at the end of the line", i))

            if '\t' in line:
                messages.append(("Tab character found", i))

            if '\r' in line:
                messages.append(("Carriage return character found", i))

    return messages


total_problems = 0

top = os.path.abspath(__file__)
top = os.path.dirname(top)
top = os.path.dirname(top)

print(f"Linting *.adoc files in {top}")

for root, _, files in os.walk(top):

    for file in files:

        if file.endswith('.adoc'):

            filename = os.path.join(root, file)

            messages = lint_file(filename)

            if messages:

                print(f"{len(messages)} problems found in {filename}:")
                print()

                for message, line in messages:
                    print(f"line {line}: {message}")
                    print()

            total_problems += len(messages)

print("Total problems found: %d" % total_problems)

sys.exit(total_problems)
