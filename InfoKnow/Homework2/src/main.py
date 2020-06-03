from process import *
from search import *

if __name__ == '__main__':
    print("Loading data...")
    text_list = get_text_list()
    print("Vectorizing...")
    bag, count = get_bag(text_list)
    print("Generating index...")
    inverse_index = generate_inverse_index(text_list, bag, count.toarray())
    print("Done. Type to search now.")
    while True:
        search_str = input("> ")

        if search_str == 'q':
            print('Bye :)')
            exit(0)

        result = run_search(search_str, inverse_index, files, text_list, bag, count)
        for i in result:
            print(i)
            s = input("n to next, q to quit, g to recommend, d to not recommend\n> ")
            if s == 'q':
                break
            else:
                continue


