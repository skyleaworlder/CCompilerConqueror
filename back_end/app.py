from flask import Flask, jsonify
import os
import subprocess
from flask import Response, request
import traceback

app = Flask(__name__)

# for debug
@app.route("/", methods=["GET"])
def test():

    # execute .exe file
    cmd = os.getcwd()+"/src/CCC.exe"
    cmd = "CCC.exe"
    p = subprocess.Popen(
        cmd, shell=True,
        stdout=subprocess.PIPE
    )

    # process output
    byte_stream = p.stdout.read()
    str_stream = byte_stream.decode('gbk')
    line_lst = str_stream.split('\r\n')
    line_lst = [line.strip(" ") for line in line_lst if len(line) > 0]

    return jsonify({ "tree_str": line_lst })


@app.route("/", methods=["POST"])
def process():

    try:
        data = request.get_json()
        code = data["code"]
        password = data["password"]

        if password != "skyleaworlder":
            return jsonify({ "tree_str": "error" })

        f = open("code.txt", "w")
        f.write(code)

        # execute .exe file
        # TODO: wait path
        #cmd = os.getcwd()+"/src/CCC.exe"
        cmd = "../CCC.exe"
        p = subprocess.Popen(
            cmd, shell=True,
            stdout=subprocess.PIPE
        )

        # process output
        byte_stream = p.stdout.read()
        str_stream = byte_stream.decode('gbk')
        line_lst = str_stream.split('\r\n')
        line_lst = [line.strip(" ") for line in line_lst if len(line) > 0]

        return jsonify({ "tree_str": line_lst })

    except Exception as e:
        traceback.print_exc()
        return jsonify({ "tree_str": "failed" })


if __name__ == "__main__":
    app.run()