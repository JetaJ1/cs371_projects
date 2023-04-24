from collections import Counter
import sys

def filterAlpha(c):
    if c.isalpha() and c.isascii():                     # Must be both alpha (a letter) and ascii (English)
        return True
    else:
        return False

n = len(sys.argv)

print("Total arguments passed:", n)
print("Name of Python script:", sys.argv[0])

assert n <= 2, f"Please provide only one file path argument at a time."
if (n == 2):
    file_path = sys.argv[1]
    print("Input File Path:", file_path)
else:
    file_path = "./PA3support/ciphertext.txt"
    print("Default File Path:", file_path)

####################################
# Get string from file input
####################################

# f = open("./PA3support/ciphertext.txt", "r")
# f = open("./PA3support/hamlet.txt", "r")
# f = open("./PA3support/merchantofvenice.txt", "r")

f = open(file_path, "r", errors="ignore")               # Ignore errors, specifically in `merchantofvenice.txt`
in_str = f.read() 
f.close()


total_chars = len(in_str)
in_str = in_str.lower()
# print(in_str)

####################################
# Letter Frequency
####################################

# Get frequency
letters = map(''.join, zip(in_str))                     # Get all characters
letters = list(filter(filterAlpha, letters))            # Filter characters for only letters
letter_freq = Counter(letters)                          # Count occurances of letters

# Sort char freqencies in descending order
letter_freq = list(letter_freq.items())
letter_freq.sort(reverse=True, key=(lambda x: x[1]))
    
####################################
# Bigram Frequency
####################################

# Get frequency
bigrams = map(''.join, zip(in_str, in_str[1:]))         # Get all bigrams
bigrams = list(filter(filterAlpha, bigrams))            # Filter bigrams for those containing only letters
bigram_freq = Counter(bigrams)                          # Count occurances of bigrams

# print(bigram_freq)

# Sort bigram freqencies in descending order
bigram_freq_sorted = list(bigram_freq.items())
bigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

####################################
# Trigram Frequency
####################################

# Get bigrams frequency and store into dict
trigrams = map(''.join, zip(in_str, in_str[1:], in_str[2:]))    # Get all trigrams
trigrams = list(filter(filterAlpha, trigrams))                  # Filter trigrams for those containing only letters
trigram_freq = Counter(trigrams)                                # Count occurances of trigrams

# print(trigram_freq)

# Sort trigram freqencies in descending order
trigram_freq_sorted = list(trigram_freq.items())
trigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

print()
while len(letter_freq) < 30:            # For printing
    letter_freq.append(("-",0))       

# print all top 30 frequencies
for i, (letter, bigram, trigram) in enumerate(zip(letter_freq[0:30], 
                                    bigram_freq_sorted[0:30], trigram_freq_sorted[0:30])):
    print(i+1, letter, bigram, trigram)
