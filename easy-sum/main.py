NUM_ALPHA = ["zero", "one", "two", "three", "four", "five", "six", "seven",
             "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen",
             "fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty"]

for i in range(1, 21):
    for j in range(i, 21-i):
        alpha_i = NUM_ALPHA[i]
        alpha_j = NUM_ALPHA[j]
        alpha_sum = NUM_ALPHA[i+j]
        if len(alpha_i) + len(alpha_j) == len(alpha_sum):
            print("{} + {} = {}".format(alpha_i, alpha_j, alpha_sum))

for i in range(1, 21):
    for j in range(i, 21-i):
        for k in range(j, 21-i-j):
            alpha_i = NUM_ALPHA[i]
            alpha_j = NUM_ALPHA[j]
            alpha_k = NUM_ALPHA[k]
            alpha_sum = NUM_ALPHA[i+j+k]
            if len(alpha_i) + len(alpha_j) + len(alpha_k) == len(alpha_sum):
                print("{} + {} + {} = {}".format(alpha_i, alpha_j, alpha_k, alpha_sum))

