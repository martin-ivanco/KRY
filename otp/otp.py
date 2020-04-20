import argparse
import base64
import json
import os
import progressbar

class Message:
    placeholder = ord("_")

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
                newvals.append(self.placeholder)
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
    def __init__(self, crib_word):
        self.word = Message(crib_word)

    def search(self, messages):
        self.message_matches = {}
        for i, m in enumerate(messages):
            self.message_matches[i] = [1] * len(m)

        for i1, m1 in enumerate(messages):
            for i2, m2 in enumerate(messages):
                if i2 > i1:
                    self.match(i1, m1, i2, m2)

    def match(self, i1, m1, i2, m2):
        xored = m1 + m2
        for j in range(len(xored) - len(self.word)):
            if self.message_matches[i1][j] or self.message_matches[i2][j]:
                other = Message(xored[j:]) + self.word
                if all(v in OTPCracker.printable for v in other.values):
                    if self.message_matches[i1][j] > 0:
                        self.message_matches[i1][j] += 1
                    if self.message_matches[i2][j] > 0:
                        self.message_matches[i2][j] += 1
                else:
                    self.message_matches[i1][j] = 0
                    self.message_matches[i2][j] = 0

class OTPCracker:
    printable = [32, 44, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71,
                 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97,
                 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
                 114, 115, 116, 117, 118, 119, 120, 121, 122]

    def __init__(self, crib_words):
        self.cribs = [Crib(cw) for cw in crib_words]
        self.keys = []
        self.maxlen = 0
    
    def crack(self, messages, batch_file = None):
        # check the last key against messages
        if self.keys:
            last_key = self.keys[-1]
            for m in messages:
                decrypted = m + self.key2msg(last_key)
                for i, v in enumerate(decrypted.values):
                    if v not in self.printable and i in last_key:
                        last_key.pop(i)

        # set up progress bar
        widgets = []
        if batch_file is not None:
            widgets = ["Processing file '{}': ".format(batch_file)]
        widgets.extend([progressbar.Bar(marker = u"\u2588"), " ", progressbar.Timer(), " ", progressbar.Percentage()])
        bar = progressbar.ProgressBar(widgets = widgets) 

        # search for cribs in messages
        self.keys.append({})
        last_key = self.keys[-1]
        for c in bar(self.cribs):
            c.search(messages)
            for idx, match in c.message_matches.items():
                for i, v in enumerate(match):
                    if v >= len(messages):
                        subkey = Message(messages[idx][i:]) + c.word
                        self.add_subkey(last_key, subkey, i)

        # remove conflicts from key
        self.maxlen = max(self.maxlen, max([len(m) for m in messages]))
        for i in list(last_key):
            if len(last_key[i]) > 1:
                last_key.pop(i)
    
    def add_subkey(self, key, subkey, offset):
        for i, k in enumerate(subkey):
            if offset + i in key:
                key[offset + i].add(k)
            else:
                key[offset + i] = set([k])

    def key2msg(self, key):
        values = [-1] * self.maxlen
        for i, vs in key.items():
            for v in vs:
                break
            values[i] = v
        
        return Message(values)

    def save_keys(self, filename):
        j = []
        for k in self.keys:
            j.append(self.key2msg(k).values)
        with open(filename, "w") as file:
            json.dump(j, file)

    def merge_keys(self):
        key = {}
        for k in self.keys:
            for i, vs in k.items():
                for v in vs:
                    break
                if i in key:
                    key[i].add(v)
                else:
                    key[i] = set([v])

        for i in list(key):
            if len(key[i]) > 1:
                key.pop(i)
        return self.key2msg(key)

def main():
    args = parse_args()

    # set allowed characters and placeholder if requested
    if args.allowed:
        with open(args.allowed) as file:
            OTPCracker.printable = json.load(file)
    if args.placeholder:
        Message.placeholder = ord(args.placeholder[0])
    
    # load crib words dictionary and create OTP cracker
    with open(args.dictionary) as file:
        words = json.load(file)
    cracker = OTPCracker(words)

    # go through message files
    for f in os.listdir(args.msg_folder):
        if not f.endswith(".txt"):
            continue
        with open(os.path.join(args.msg_folder, f)) as file:
            lines = file.readlines()
        lines = [list(base64.b64decode(l.strip())) for l in lines]
        cracker.crack([Message(l) for l in lines], f)
    
    # get key and save it
    key = cracker.merge_keys()
    with open(args.output, "w") as file:
        json.dump(key.values, file)
    print("Key successfully cracked!")

def parse_args():
    parser = argparse.ArgumentParser(description="Weakend One-Time Pad Algorithm Cracker")
    parser.add_argument("msg_folder", type = str, metavar = "<messages path>",
        help = "path to folder with txt files containing encrypted messages encoded in base64 and "
               "separated by a new line")
    parser.add_argument("-d", "--dictionary", type = str, required = False, metavar = "<path>",
        default = "words.json",
        help = "path to a json file containing an array of frequently used words in the language "
               "of the encrypted messages - defaults to included dictionary of czech words")
    parser.add_argument("-o", "--output", type = str, required = False, metavar = "<path>",
        default = "key.txt", help = "path output file - defaults to 'key.txt'")
    parser.add_argument("-a", "--allowed", type = str, required = False, metavar = "<path>",
        help = "path to a json file containing an array of integer values representing allowed "
               "charcters in the messages - defaults to letters of english alphabet, numbers, "
               "dot, comma and space")
    parser.add_argument("-p", "--placeholder", type = str, required = False, metavar = "<char>",
        help = "character to print as a placeholder for places in the messages that the program "
               "couldn't crack - defaults to '_'")
    return parser.parse_args()

if __name__ == "__main__":
    main()
