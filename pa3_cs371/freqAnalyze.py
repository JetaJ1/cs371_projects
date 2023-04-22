from collections import Counter

test_str = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \
            Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \
            Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. \
            Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.'


# Get bigrams frequency and store into bigram_freq dict
bigram_freq = Counter(map(''.join, zip(test_str, test_str[1:])))

print(bigram_freq)

# Sort bigram freqencies in descending order
bigram_freq_sorted = list(bigram_freq.items())
bigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# Print top 30 bigram frequencies
for pair in bigram_freq_sorted[0:30]:
    print(pair)
    # print(pair[0] + ": " + str(pair[1]))

# Get trigrams frequency and store into trigram_freq dict
trigram_freq = Counter(map(''.join, zip(test_str, test_str[1:], test_str[2:])))

print(trigram_freq)

# Sort trigram freqencies in descending order
trigram_freq_sorted = list(trigram_freq.items())
trigram_freq_sorted.sort(reverse=True, key=(lambda x: x[1]))

# Print top 30 trigram frequencies
for pair in trigram_freq_sorted[0:30]:
    print(pair)
