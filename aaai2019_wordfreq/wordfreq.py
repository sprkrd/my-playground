#!/usr/bin/env python3

import re
import matplotlib.pyplot as plt

titles = []

with open("aaai2019.html") as f:
    for line in f:
        m = re.match(r"(.+)</a><br />", line)
        if m:
            titles.append(m.group(1).lower())

titles.pop()

word_count = {}

for title in titles:
    for word in title.split():
        word_count[word] = word_count.get(word,0) + 1

word_ranking = sorted(((v,w) for w,v in word_count.items()), reverse=True)


FORBIDDEN = set(["for", "with", "and", "a", "of", "in", "to", "via", "on",
    "using", "from", "the", "based", "an", "by"])

word_ranking_filtered = [(v,w) for v,w in word_ranking if w not in FORBIDDEN]


counts, words = zip(*word_ranking_filtered)

N = 20
print(words[:N])
plt.barh(list(reversed(words[:N])), list(reversed(counts[:N])))
plt.tight_layout()
plt.show()


deep_learning_papers = 0
non_deep_learning_papers = 0
deep_kw = ["deep", "neural", "adversarial", "autoencoder"]
for t in titles:
    if any(kw in t for kw in deep_kw):
        deep_learning_papers += 1
    else:
        print(t)
        non_deep_learning_papers += 1

print(deep_learning_papers)
print(non_deep_learning_papers)
N_papers = deep_learning_papers + non_deep_learning_papers
p_deep = deep_learning_papers / N_papers
p_non = non_deep_learning_papers / N_papers

plt.pie([deep_learning_papers, non_deep_learning_papers], labels=[f"{p_deep:.1%}", f"{p_non:.1%}"])
plt.legend(["deep learning", "¬deep learning"])
plt.show()




