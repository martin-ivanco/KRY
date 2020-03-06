import argparse
import base64

class CipherString:
    def __init__(self, values):
        if type(values) is list:
            self.values = values
        if type(values) is str:
            self.values = [ord(v) for v in values]

    def __add__(self, other):
        minlen = min(len(self), len(other))
        return CipherString([self.values[i] ^ other.values[i] for i in range(minlen)])

    def __str__(self):
        return "".join([chr(v) for v in self.values])

    def hex(self):
        encoded = []
        for v in self.values:
            h = hex(v)[2:]
            encoded.append(h[:2] if len(h) > 1 else "0" + h[0])
        return "0x" + "".join(encoded)

    def __len__(self):
        return len(self.values)

    def __getitem__(self, key):
        return self.values[key]

    def __setitem__(self, key, value):
        if type(value) is int:
            self.values[key] = value
        if type(value) is str:
            self.values[key] = ord(value[0])

    def __iter__(self):
        return iter(self.values)

def main():
    parser = argparse.ArgumentParser(description="Weakend One-Time Pad Algorithm Cracker")
    parser.add_argument("messages", type = str, metavar = "<filename>",
        help = "path to file with encrypted messages encoded in base64 and separated by new line")
    args = parser.parse_args()

    with open(args.messages) as file:
        lines = file.readlines()
    lines = [list(base64.b64decode(l.strip())) for l in lines]
    messages = [CipherString(l) for l in lines]

if __name__ == "__main__":
    main()
