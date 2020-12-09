from flask import Flask, jsonify
import os
import subprocess
from flask import Response, request
from functools import reduce

app = Flask(__name__)

# for debug
@app.route("/", methods=["GET"])
def test():

    # execute .exe file
    parent_dir = os.path.dirname(os.getcwd())
    cmd = os.getcwd()+"/src/test.exe"
    cmd = "test2.exe"
    p = subprocess.Popen(
        cmd, shell=True,
        stdout=subprocess.PIPE
    )

    # process output
    f = open(parent_dir+"/data/tree.txt", "r")
    tree_lst = f.readlines()

    # tree_str: 'Program\n0->1:GlbDefList\n1->2:GlbDef 3:GlbDefList\n
    # tree_lst: ['Program', '0->1:GlbDefList', '1->2:GlbDef 3:GlbDefList']
    tree_str = reduce(lambda sum, line: sum+line, tree_lst)
    tree_lst = [line.strip('\n') for line in tree_lst]

    return jsonify({ "tree_str": tree_str })


@app.route("/", methods=["POST"])
def process():

    data = request.get_json()
    code = data["code"]

    parent_dir = os.path.dirname(os.getcwd())
    f = open(parent_dir+"/data/code.txt", "w")
    f.write(code)
    f.close()

    # execute .exe file
    # TODO: wait path
    #cmd = os.getcwd()+"/src/test.exe"
    cmd = "../test2.exe"
    p = subprocess.Popen(
        cmd, shell=True,
        stdout=subprocess.PIPE
    )

    # process output
    f = open(parent_dir+"/data/tree.txt", "r")
    tree_lst = f.readlines()

    # tree_str: 'Program\n0->1:GlbDefList\n1->2:GlbDef 3:GlbDefList\n
    # tree_lst: ['Program', '0->1:GlbDefList', '1->2:GlbDef 3:GlbDefList']
    tree_str = reduce(lambda sum, line: sum+line, tree_lst)
    tree_lst = [line.strip('\n') for line in tree_lst]

    return jsonify({ "tree_str": tree_str })


if __name__ == "__main__":
    app.run()