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
    <h1>Hexapod Control</h1>
    <form method="POST" action="/cmd">
      <button name="cmd" value="forward">Move Forward</button><br>
      <button name="cmd" value="rotate_left">Rotate Left</button>
      <button name="cmd" value="rotate_right">Rotate Right</button>
    </form>
    '''

@app.route('/cmd', methods=['POST'])
def cmd():
    action = request.form.get('cmd')
    if action not in ("forward","rotate_left","rotate_right"):
        return "Invalid",400

    # build the remote command string
    remote_cmd = f"{REMOTE_BIN} {action}"

    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(PI_HOST, username=PI_USER, password=PI_PASS)

        stdin, stdout, stderr = ssh.exec_command(remote_cmd)
        exit_code = stdout.channel.recv_exit_status()
        out = stdout.read().decode().strip()
        err = stderr.read().decode().strip()
        ssh.close()

        # Show you exactly what ran and any errors
        return f"<pre>Ran: {remote_cmd}\n\nExit code: {exit_code}\n\nSTDOUT:\n{out}\n\nSTDERR:\n{err}</pre>"

    except Exception as e:
        return f"<pre>SSH failed: {e}</pre>",500

if __name__=="__main__":
    app.run(host="0.0.0.0",port=8080)