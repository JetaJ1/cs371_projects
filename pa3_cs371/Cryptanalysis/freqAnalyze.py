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

#test_str = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \
            # Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
            # Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. \
            # Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.'
# f = open("./PA3support/ciphertext.txt", "r")
# f = open("./PA3support/hamlet.txt", "r")
# f = open("./PA3support/merchantofvenice.txt", "r")

f = open(file_path, "r", errors="ignore")               # Ignore errors, specifically in `merchantofvenice.txt`
test_str = f.read() 
f.close()


total_chars = len(test_str)
test_str = test_str.lower()
# print(test_str)

####################################
# Letter Frequency
####################################

# Get frequency
letters = map(''.join, zip(test_str))                   # Get all characters
letters = list(filter(filterAlpha, letters))            # Filter characters for only letters
letter_freq = Counter(letters)                          # Count occurances of letters

# Sort char freqencies in descending order
letter_freq = list(letter_freq.items())
letter_freq.sort(reverse=True, key=(lambda x: x[1]))

# print("\nLetter Frequency: ")
# Print top 30 char frequencies
# for pair in char_freq[0:30]:
#     print(pair)
    
####################################
# Bigram Frequency
####################################

# Get frequency
bigrams = map(''.join, zip(test_str, test_str[1:]))     # Get all bigrams
bigrams = list(filter(filterAlpha, bigrams))            # Filter bigrams for those containing only letters
bigram_freq = Counter(bigrams)                          # Count occurances of bigrams

# print(bigram_freq)

# Sort bigram freqencies in descending order
bigram_freq_sorted = list(bigram_freq.items())
bigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# print("\nBigram Frequency: ")
# Print top 30 bigram frequencies
# for pair in bigram_freq_sorted[0:30]:
#     print(pair)

####################################
# Trigram Frequency
####################################

# Get bigrams frequency and store into dict
trigrams = map(''.join, zip(test_str, test_str[1:], test_str[2:]))      # Get all trigrams
trigrams = list(filter(filterAlpha, trigrams))                          # Filter trigrams for those containing only letters
trigram_freq = Counter(trigrams)                                        # Count occurances of trigrams

# print(trigram_freq)

# Sort trigram freqencies in descending order
trigram_freq_sorted = list(trigram_freq.items())
trigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# print("\nTrigram Frequency: ")
# Print top 30 trigram frequencies
# for pair in trigram_freq_sorted[0:30]:
#     print(pair)

print()
while len(letter_freq) < 30:            # For printing
    letter_freq.append(("-",0))       

# print all top 30 frequencies
for i, (letter, bigram, trigram) in enumerate(zip(letter_freq[0:30], 
                                    bigram_freq_sorted[0:30], trigram_freq_sorted[0:30])):
    print(i+1, letter, bigram, trigram)
