from sklearn.feature_extraction.text import CountVectorizer
import math


class ResultItem:
    def __init__(self, index, name, text):
        self.index = index
        self.name = name
        self.head = text.split('\n')[0]
        self.text = text
        self.rank = 0.0
        self.freq = 0
        self.count = 0
        self.occurrence = []
        self.similarity = 0.0

    def __str__(self):
        s = "file: " + self.name + \
            "\nhead: " + self.head + \
            "\nfreq: " + str(self.freq) + \
            "\nrank: " + str(self.rank) + \
            "\nsimilarity: " + str(self.similarity) + \
            "\n"
        for j in self.occurrence:
            s += "> ..." + self.text[max(0, j[0] - 50):j[0] + 50] + "...\n"
        return s


def get_similarity(a, b):
    dot = 0
    len_a = 0
    len_b = 0
    for i in range(len(a)):
        dot += a[i] * b[i]
        len_a += a[i] * a[i]
        len_b += b[i] * b[i]
    len_a = math.sqrt(len_a)
    len_b = math.sqrt(len_b)
    return dot / (len_a * len_b)


def run_search(search_str, inverse_index, file_names, texts, bag, array):
    temp = []
    freq = []
    s_list = search_str.split(' ')
    for s in s_list:
        temp.append(inverse_index[s].copy())
        freq.append(0)
        if inverse_index[s]:
            for i in inverse_index[s]:
                freq[-1] += i[1]
    result_dict = {}
    for index, i in enumerate(temp):
        if not i:
            continue
        for j in i:
            if j[0] not in result_dict:
                item = ResultItem(j[0], file_names[j[0]], texts[j[0]])
                item.count += 1
                item.freq += j[1]
                item.rank += j[1] * 100 / freq[index]
                item.occurrence.extend(j[2])
                result_dict[j[0]] = item
            else:
                result_dict[j[0]].count += 1
                result_dict[j[0]].freq += j[1]
                result_dict[j[0]].rank += j[1] * 100 / freq[index]
                result_dict[j[0]].occurrence.extend(j[2])
    result_list = [i for i in result_dict.values()]

    search_vec = CountVectorizer(vocabulary=bag.get_feature_names()).fit_transform([search_str]).toarray()
    for i in result_list:
        i.similarity = get_similarity(search_vec[0], array[i.index].A[0])

    result_list.sort(key=lambda x: -x.rank * x.count)
    return result_list
