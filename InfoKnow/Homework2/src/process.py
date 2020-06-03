import os
import re
from sklearn.feature_extraction.text import CountVectorizer
from collections import defaultdict

path = 'data'
files = os.listdir(path)


def get_text_list():
    text_list = []

    for i in files:
        f = open(path + '\\' + i, encoding='utf-8')
        text = f.read()
        text_list.append(text)
    return text_list


def get_bag(texts):
    bag = CountVectorizer(token_pattern='\\b[A-Za-z]+\\b')
    count = bag.fit_transform(texts)
    return bag, count


def generate_inverse_index(text_list, bag, array):
    result = defaultdict(list)
    words = bag.get_feature_names()
    for index, value in enumerate(text_list):
        for i, word in enumerate(words):
            if array[index][i] != 0:
                position_list = [m.span() for m in re.finditer(
                    r'\b' + word + r'\b', value)]
                result[word].append((index, array[index][i], position_list))
    return result
