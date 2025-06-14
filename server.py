# server.py
from flask import Flask, request, redirect, url_for
import paramiko

app = Flask(__name__)

PI_HOST   = "192.168.1.143"
PI_USER   = "bogdan"
PI_PASS   = "1234"
# ← use the full, exact path to your built binary:
REMOTE_BIN = "/home/bogdan/Desktop/Hexapod/cmake-build-debug/cmake-build-debug/cmake-build-debug/HexapodRobot"

@app.route('/')
def index():
    return '''
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="utf-8">
      <title>Hexapod Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          display: flex;
          justify-content: center;
          align-items: center;
          height: 100vh;
          background-color: #f8f9fa;
        }
        .control-box {
          background: #ffffff;
          border: 2px solid #ccc;
          border-radius: 12px;
          padding: 30px;
          box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
          text-align: center;
        }
        .btn-row {
          display: flex;
          justify-content: space-between;
          margin-top: 20px;
        }
        .vertical-gap {
          margin-top: 20px;
        }
        button {
          font-size: 32px;
          padding: 10px 20px;
          margin: 5px;
          width: 80px;
          height: 80px;
          border: none;
          border-radius: 8px;
          background-color: #007bff;
          color: white;
          cursor: pointer;
        }
        button:hover {
          background-color: #0056b3;
        }
      </style>
    </head>
    <body>
      <div class="control-box">
        <form method="POST" action="/cmd">
          <div>
            <button name="cmd" value="forward">^</button>
          </div>
          <div class="btn-row">
            <button name="cmd" value="rotate_left">&lt;</button>
            <button name="cmd" value="rotate_right">&gt;</button>
          </div>
          <div class="vertical-gap">
            <button name="cmd" value="move_backward">v</button>
          </div>
        </form>
      </div>
    </body>
    </html>
    '''

@app.route('/cmd', methods=['POST'])
def cmd():
    action = request.form.get('cmd')
    if action not in ("forward", "rotate_left", "rotate_right", "move_backward"):
        return "Invalid", 400

    # run in background on the Pi so Flask returns immediately
    remote_cmd = f"nohup {REMOTE_BIN} {action} > /dev/null 2>&1 &"

    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(PI_HOST, username=PI_USER, password=PI_PASS)

        ssh.exec_command(remote_cmd)
        ssh.close()

        return redirect(url_for('index'))

    except Exception as e:
        return f"<pre>SSH failed: {e}</pre>", 500

if __name__=="__main__":
    app.run(host="0.0.0.0", port=8080)
