import sys
import json

def extract(n: int) -> None:
    filename = '../dataset/hotpot_qa.json'
    
    # json data load
    with open(filename, 'r', encoding='utf-8') as f:
        data = json.load(f)

    # processing
    data = data['questions'][:n]
    temp = {'questions': data}

    # save
    with open(f'../dataset/hotpot_qa_{n}.json', 'w', encoding='utf-8') as f:
        json.dump(temp, f, ensure_ascii=False, indent=4)



if __name__ == '__main__':
    args = sys.argv
    size = 20 if len(args) == 1 else int(args[1])
    extract(size)