from flask import Flask, jsonify
import os
import subprocess
from flask import Response, request
import traceback
from functools import reduce
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

@app.route("/", methods=["POST"])
def process():

    try:
        data = request.get_json()
        code = data["code"]
        password = data["password"]
        grammar = data["grammar"]

        if password != "skyleaworlder":
            return jsonify({ "tree_str": "error" })

        f = open("code.txt", "w")
        f.write(code)
        f.close()
        f = open("gramma.txt", "w")
        f.write(grammar)
        f.close()
        

        # execute .exe file
        # TODO: wait path
        #cmd = os.getcwd()+"/src/CCC.exe"
        cmd = "a.out"
        p = subprocess.call(
            cmd, shell=True,
            stdout=subprocess.PIPE
        )

        # process output
        f = open("tree.txt", "r")
        tree_lst = f.readlines()

        # tree_str: 'Program\n0->1:GlbDefList\n1->2:GlbDef 3:GlbDefList\n
        # tree_lst: ['Program', '0->1:GlbDefList', '1->2:GlbDef 3:GlbDefList']
        tree_str = reduce(lambda sum, line: sum+line, tree_lst)
        tree_lst = [line.strip('\n') for line in tree_lst]

        print(tree_str)

        return jsonify({ "tree_str": tree_str })

    except Exception as e:
        traceback.print_exc()
        return jsonify({ "tree_str": "failed" })


if __name__ == "__main__":
    app.run()
