import os
import sys


def lint_file(filename):

    messages = []

    with open(filename, 'r', encoding='utf-8') as file:

        for i, line in enumerate(file):

            c = [ord(c) < 128 for c in line]

            if not all(c):
                marker = ''.join([' ' if x else '^' for x in c])
                message = "Non-ASCII characters found:\n%s\n%s" % (line[:-1], marker)
                messages.append((message, i))

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

print("Linting *.adoc, *.h, and *.xsd files in %s" % top)

num_files = 0

for root, _, files in os.walk(top):

    for file in files:

        if file.endswith(('.adoc', '.xsd', '*.h')):

            filename = os.path.join(root, file)

            messages = lint_file(filename)

            if messages:

                print("%d problems found in %s:" % (len(messages), filename))
                print()

                for message, line in messages:
                    print("line %d: %s" % (line + 1, message))
                    print()

            num_files += 1
            total_problems += len(messages)

print("Total problems found in %d files: %d" % (num_files, total_problems))

sys.exit(total_problems)
