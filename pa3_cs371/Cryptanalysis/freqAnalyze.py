from collections import Counter
import sys

n = len(sys.argv)

print("Total arguments passed:", n)
print("Name of Python script:", sys.argv[0])
print("Argument:", sys.argv[1])
print("")

assert n <= 2, f"Please provide only one file path argument at a time."
if (n == 2):
    file_path = sys.argv[1]


#test_str = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \
            # Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
            # Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. \
            # Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.'

####################################
# Get string from file input
####################################

# f = open("./PA3support/Cryptanalysis/ciphertext.txt", "r")
# f = open("./PA3support/Cryptanalysis/hamlet.txt", "r")
# f = open("./PA3support/Cryptanalysis/merchantofvenice.txt", "r")
f = open(file_path, "r")
test_str = f.read() 
f.close()

total_chars = len(test_str)

####################################
# Character Frequency
####################################
print("Character Frequency: ")

# Get char frequency and store into char_freq dict
char_freq = Counter(map(''.join, zip(test_str)))

# print(char_freq)

# Sort char freqencies in descending order
char_freq = list(char_freq.items())
char_freq.sort(reverse=True, key=(lambda x: x[1]))

# Print top 30 char frequencies
for pair in char_freq[0:30]:
    print(pair)
    
####################################
# Bigram Frequency
####################################
print("\nBigram Frequency: ")

# Get bigrams frequency and store into bigram_freq dict
bigram_freq = Counter(map(''.join, zip(test_str, test_str[1:])))

# print(bigram_freq)

# Sort bigram freqencies in descending order
bigram_freq_sorted = list(bigram_freq.items())
bigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# Print top 30 bigram frequencies
for pair in bigram_freq_sorted[0:30]:
    print(pair)
    # print(pair[0] + ": " + str(pair[1]))

####################################
# Trigram Frequency
####################################
print("\nTrigram Frequency: ")

# Get trigrams frequency and store into trigram_freq dict
trigram_freq = Counter(map(''.join, zip(test_str, test_str[1:], test_str[2:])))

# print(trigram_freq)

# Sort trigram freqencies in descending order
trigram_freq_sorted = list(trigram_freq.items())
trigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# Print top 30 trigram frequencies
for pair in trigram_freq_sorted[0:30]:
    print(pair)
