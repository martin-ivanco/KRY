import argparse
import base64

class Message:
    def __init__(self, values):
        if type(values) is list:
            self.values = values
        if type(values) is str:
            self.values = [ord(v) for v in values]

    def __add__(self, other):
        minlen = min(len(self), len(other))
        newvals = []
        for i in range(minlen):
            if self.values[i] < 0 or other.values[i] < 0:
                newvals.append(ord("_"))
            else:
                newvals.append(self.values[i] ^ other.values[i])
        return Message(newvals)

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

class Crib:
    def __init__(self, crib_word, messages):
        self.word = crib_word
        self.message_matches = {}
        for i, m in enumerate(messages):
            self.message_matches[i] = [0] * len(m)
        self.search(messages)

    def search(self, messages):
        for i1, m1 in enumerate(messages):
            for i2, m2 in enumerate(messages):
                if i2 <= i1:
                    continue
                self.match(i1, m1, i2, m2)

    def match(self, i1, m1, i2, m2):
        xored = m1 + m2
        for j in range(len(xored) - len(self.word)):
            other = Message(xored[j:]) + self.word
            if str(other).lower() in OTPCracker.crib_words:
                self.message_matches[i1][j] += 1
                self.message_matches[i2][j] += 1


class OTPCracker:
    # crib_words = ["pro", "ova", "ost", "pri", "sta", "pre", "ter", "eni", "pod", "kte", "pra",
    #               "eho", "sti", "red", "kon", "nos", "ick", "sou", "ist", "edn", "ske", "odn",
    #               "tel", "ani", "ent", "str", "ove", "nov", "pol", "spo", "vat", "nim", "jak",
    #               "val", "dni", "sto", "tak", "lov"]
    crib_words = ["st", "pr", "ne", "ni", "po", "ov", "ro", "en", "na", "je", "te", "le", "ko",
                  "od", "ra", "to", "ou", "no", "la", "li", "ho", "do", "os", "se", "ta", "al",
                  "ed", "an", "ce", "va", "at", "re", "er", "ti", "em", "in", "sk", "lo"]

    def __init__(self, messages):
        self.messages = messages
        self.key = {}
    
    def crack(self):
        cribs = []
        for cw in self.crib_words:
            cribs.append(Crib(Message(cw), self.messages))
        
        for c in cribs:
            print("-----------------------------------------")
            print(str(c.word))
            print("-----------------------------------------")
            for idx, match in c.message_matches.items():
                for i, v in enumerate(match):
                    if v < 2:
                        continue
                    subkey = Message(self.messages[idx][i:]) + c.word
                    self.add_subkey(subkey, i)
                    print("Message: {}; Position: {}; Subkey: {}".format(idx, i, subkey.values))
                    for m in self.messages:
                        print(str(Message(m[i:]) + subkey))
            print("-----------------------------------------")
        print(self.key)

        xor_key = [0] * max([len(m) for m in self.messages])
        for i, vs in self.key.items():
            if len(vs) < 2:
                xor_key[i] = vs.pop()
        
        return xor_key
    
    def add_subkey(self, subkey, idx):
        for i, k in enumerate(subkey):
            if idx + i in self.key:
                self.key[idx + i].add(k)
            else:
                self.key[idx + i] = set([k])

def main():
    parser = argparse.ArgumentParser(description="Weakend One-Time Pad Algorithm Cracker")
    parser.add_argument("messages", type = str, metavar = "<filename>",
        help = "path to file with encrypted messages encoded in base64 and separated by new line")
    args = parser.parse_args()

    with open(args.messages) as file:
        lines = file.readlines()
    lines = [list(base64.b64decode(l.strip())) for l in lines]
    cracker = OTPCracker([Message(l) for l in lines])
    key = cracker.crack()
    with open("key.txt", "wb") as file:
        file.write(base64.b64encode(bytes(key)))
        file.write(bytes("\n".encode()))

if __name__ == "__main__":
    main()
