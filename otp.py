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
