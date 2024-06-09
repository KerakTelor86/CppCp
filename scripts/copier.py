import os
import sys

module_path = sys.argv[1]

included = set()


def expand(path):
    ret = []

    if path in included:
        return ret
    included.add(path)

    lines = []
    with open(os.path.join(module_path, path), "r", encoding="utf-8") as file:
        for line in file.readlines():
            lines.append(line)

    for line in lines:
        if line.startswith('#include "'):
            header_file = line.strip().removeprefix('#include "').removesuffix('"')
            for expanded_line in expand(header_file):
                ret.append(expanded_line)
        else:
            ret.append(line)

    return ret


lines = sys.stdin.readlines()

output = []
for line in lines:
    if line.startswith('#include "'):
        header_file = line.strip().removeprefix('#include "').removesuffix('"')
        for expanded_line in expand(header_file):
            output.append(expanded_line)
    else:
        output.append(line)

prev_empty = False

for line in output:
    cur_empty = line == "\n"
    if prev_empty and cur_empty:
        continue
    prev_empty = cur_empty
    print(line, end="")
