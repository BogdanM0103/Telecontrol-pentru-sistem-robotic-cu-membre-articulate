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
    .control-box, .arrow-box {
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
    .arrow-box {
      width: 300px; height: 300px;
      position: relative;
      margin: 0 auto 20px;
      padding: 0;
      text-align: center;
    }
    #arrow {
      width: 0;
      height: 0;
      border-left: 25px solid transparent;
      border-right: 25px solid transparent;
      border-bottom: 100px solid #e74c3c;
      position: absolute;
      top: 50%; left: 50%;
      transform-origin: center top;
      transform: translate(-50%, -50%) rotate(0deg);
      cursor: grab;
    }
    #arrowAngleDisplay {
      text-align: center;
      margin-top: 10px;
      font-size: 18px;
    }
    #arrowControls {
      text-align: center;
      margin-top: 10px;
    }
    #arrowControls button {
      font-size: 16px;
      padding: 8px 16px;
      margin: 0 5px;
      border: none;
      border-radius: 6px;
      background-color: #28a745;
      color: white;
      cursor: pointer;
    }
    #arrowControls button.stop {
      background-color: #dc3545;
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

  <div class="arrow-box">
    <div id="arrow"></div>
  </div>
  <div id="arrowAngleDisplay">Arrow Angle: 0°</div>
  <div id="arrowControls">
    <button id="startArrow">Start</button>
    <button id="stopArrow" class="stop">Stop</button>
  </div>

  <script>
    // Arrow spin and control logic
    const arrow = document.getElementById('arrow');
    const arrowDisp = document.getElementById('arrowAngleDisplay');
    let arrowDragging = false;
    let currentAngle = 0;

    arrow.addEventListener('mousedown', () => {
      arrowDragging = true;
      arrow.style.cursor = 'grabbing';
    });
    document.addEventListener('mouseup', () => {
      if (arrowDragging) {
        arrowDragging = false;
        arrow.style.cursor = 'grab';
      }
    });
    document.addEventListener('mousemove', e => {
      if (!arrowDragging) return;
      const box = arrow.parentElement.getBoundingClientRect();
      const cx = box.left + box.width/2;
      const cy = box.top + box.height/2;
      const dx = e.clientX - cx;
      const dy = e.clientY - cy;
      let ang = Math.atan2(dy, dx) * 180/Math.PI + 90;
      ang = (ang + 360) % 360;
      currentAngle = Math.round(ang);
      arrow.style.transform = `translate(-50%, -50%) rotate(${currentAngle}deg)`;
      arrowDisp.textContent = `Arrow Angle: ${currentAngle}°`;
    });

    document.getElementById('startArrow').addEventListener('click', () => {
      fetch('/arrow', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ action: 'start', angle: currentAngle })
      }).catch(console.error);
    });
    document.getElementById('stopArrow').addEventListener('click', () => {
      fetch('/arrow', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ action: 'stop' })
      }).catch(console.error);
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

@app.route('/arrow', methods=['POST'])
def arrow_control():
    data = request.get_json()
    action = data.get('action')
    angle = data.get('angle', 0)
    if action == 'start':
        cmd = f"crab {angle}"
    elif action == 'stop':
        cmd = "stop"
    else:
        return jsonify(success=False, error="Invalid action"), 400

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(cmd.encode('utf-8'), (PI_HOST, PI_PORT))
        sock.close()
        return jsonify(success=True)
    except Exception as e:
        return jsonify(success=False, error=str(e)), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
