from flask import Flask, request
import paramiko

app = Flask(__name__)

RASPBERRY_PI_HOST = "192.168.1.143"  # Replace with your Pi's IP
RASPBERRY_PI_USER = "bogdan"
RASPBERRY_PI_PASSWORD = "1234"
ROBOT_BINARY_PATH = r"/home/bogdan/Desktop/Hexapod/cmake-build-debug/cmake-build-debug/cmake-build-debug/HexapodRobot"


@app.route('/')
def index():
    return '''
        <h2>Hexapod Control</h2>
        <form method="POST" action="/cmd">
            <button name="command" value="run">Start Robot</button>
        </form>
    '''

@app.route('/cmd', methods=['POST'])
def cmd():
    command = request.form['command']
    if command != "run":
        return "Invalid command", 400

    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(RASPBERRY_PI_HOST, username=RASPBERRY_PI_USER, password=RASPBERRY_PI_PASSWORD)

        # Run the robot binary on the Pi
        stdin, stdout, stderr = ssh.exec_command(ROBOT_BINARY_PATH)
        output = stdout.read().decode()
        error = stderr.read().decode()

        ssh.close()

        return f"<pre>Output:\n{output}\nError:\n{error}</pre>"
    except Exception as e:
        return f"Error: {str(e)}", 500

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080)
