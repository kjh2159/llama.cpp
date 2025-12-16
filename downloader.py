import json, subprocess

def json_loads(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data

def model_selecter(model_info):
    # select model family
    for i, fam in enumerate(model_info.keys()):
        print(f"{i+1:2>}. {fam}")
    fam_choice = int(input("> ")) - 1
    fam = list(model_info.keys())[fam_choice]

    # select model parameter
    print()
    for i, param in enumerate(model_info[fam].keys()):
        print(f"{i+1:2>}. {param}")
    print(f"Now: {fam}")
    param_choice = int(input("> ")) - 1
    param = list(model_info[fam].keys())[param_choice]

    # select bit level
    print()
    for i, bit in enumerate(model_info[fam][param].keys()):
        temp = model_info[fam][param][bit]
        print(f"{i+1:2>}. {bit} {temp["description"]} ({temp["size"]})")
    print(f"Now: {fam} - {param}")
    bit_choice = int(input("> ")) - 1
    bit = list(model_info[fam][param].keys())[bit_choice]

    model = model_info[fam][param][bit]
    return model

def model_downloader(url, output):
    cmd = f"curl -L {url} -o {output}".split(" ")
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Could not download the model: {e}")
        return
    except FileNotFoundError as e:
        print(f"Could not find a directory. {e}")
        return
    print(f"\nSelected model: {output.split('/')[-1]}")
    return

if __name__ == "__main__":
    model_info = json_loads("assets/model_src.json")
    model = model_selecter(model_info)
    model_downloader(model["url"], model["output"])