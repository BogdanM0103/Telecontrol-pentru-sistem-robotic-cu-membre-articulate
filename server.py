# server.py
from flask import Flask, request, redirect, url_for, jsonify
import socket

app = Flask(__name__)

PI_HOST = "192.168.1.143"   # IP of your Pi
PI_PORT = 6006              # Must match udp_server.py UDP_PORT

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
          align-items: flex-start;
          height: 100vh;
          background-color: #f1f1f1;
          margin: 0;
          padding: 20px;
        }
        .control-box, .joystick-box {
          background: #fff;
          border: 2px solid #ccc;
          border-radius: 12px;
          padding: 20px;
          box-shadow: 0 4px 8px rgba(0,0,0,0.1);
        }
        .control-box {
          margin-right: 40px;
          text-align: center;
        }
        .control-box button {
          font-size: 32px;
          width: 80px; height: 80px;
          margin: 5px;
          border: none; border-radius: 8px;
          background-color: #007bff; color: white;
          cursor: pointer;
        }
        .control-box button:hover { background-color: #0056b3; }
        .joystick-box {
          width: 320px; height: 380px;
          position: relative;
        }
        #joystickZone {
          position: relative;
          width: 300px; height: 300px;
          margin: 0 auto;
          background: #e9ecef;
          border: 2px solid #ccc;
          border-radius: 50%;
          user-select: none;
        }
        #joystickHandle {
          position: absolute;
          width: 60px; height: 60px;
          background: #28a745; border-radius: 50%;
          left: 50%; top: 50%;
          transform: translate(-50%, -50%);
          pointer-events: none;
        }
        #angleDisplay {
          text-align: center;
          margin-top: 10px;
          font-size: 18px;
        }
      </style>
    </head>
    <body>
      <div class="control-box">
        <form method="POST" action="/cmd">
          <div><button name="cmd" value="forward">↑</button></div>
          <div style="margin-top:20px;">
            <button name="cmd" value="rotate_left">←</button>
            <button name="cmd" value="rotate_right">→</button>
          </div>
          <div style="margin-top:20px;">
            <button name="cmd" value="move_backward">↓</button>
          </div>
        </form>
      </div>

      <div class="joystick-box">
        <div id="joystickZone">
          <div id="joystickHandle"></div>
        </div>
        <div id="angleDisplay">Angle: –°, Power: –%</div>
      </div>

      <script>
        const UDP_PORT = 6006;

        const zone   = document.getElementById('joystickZone');
        const handle = document.getElementById('joystickHandle');
        const disp   = document.getElementById('angleDisplay');
        const radius = zone.offsetWidth / 2;
        let dragging = false;

        function sendJoystick(angle, power) {
          fetch('/joystick', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ angle, power })
          }).catch(console.error);
        }

        zone.addEventListener('mousedown', () => { dragging = true; });
        document.addEventListener('mouseup', () => {
          if (!dragging) return;
          dragging = false;
          handle.style.left = '50%';
          handle.style.top  = '50%';
          disp.textContent = 'Stopped';
          sendJoystick(-1, 0);
        });
        document.addEventListener('mousemove', e => {
          if (!dragging) return;
          const rect = zone.getBoundingClientRect();
          let x = e.clientX - rect.left;
          let y = e.clientY - rect.top;
          let dx = x - radius, dy = y - radius;
          let dist = Math.hypot(dx, dy);
          let maxDist = radius - handle.offsetWidth/2;
          if (dist > maxDist) {
            const r = maxDist / dist;
            dx *= r; dy *= r;
            dist = maxDist;
          }
          handle.style.left = `${dx + radius - handle.offsetWidth/2}px`;
          handle.style.top  = `${dy + radius - handle.offsetHeight/2}px`;

          let ang = Math.atan2(dy, dx) * 180/Math.PI;
          if (ang < 0) ang += 360;
          let fromVert = (ang + 90) % 360;
          let power = Math.round(dist / maxDist * 100);

          disp.textContent = `Angle: ${Math.round(fromVert)}°, Power: ${power}%`;
          sendJoystick(Math.round(fromVert), power);
        });
      </script>
    </body>
    </html>
    '''

@app.route('/cmd', methods=['POST'])
def cmd():
    action = request.form.get('cmd')
    if action not in ("forward", "rotate_left", "rotate_right", "move_backward"):
        return "Invalid", 400

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(action.encode('utf-8'), (PI_HOST, PI_PORT))
        sock.close()
        return redirect(url_for('index'))
    except Exception as e:
        return f"UDP send failed: {e}", 500

@app.route('/joystick', methods=['POST'])
def joystick():
    data = request.get_json()
    angle = data.get('angle', -1)
    power = data.get('power', 0)

    # crab when dragged, stop when released
    if power <= 0:
        cmd = "stop"
    else:
        cmd = f"crab {angle}"

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(cmd.encode('utf-8'), (PI_HOST, PI_PORT))
        sock.close()
        return jsonify(success=True)
    except Exception as e:
        return jsonify(success=False, error=str(e)), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
