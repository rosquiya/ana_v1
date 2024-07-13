from flask import Flask, render_template, request, jsonify
import serial
import time

app = Flask(__name__)
ser = None  # Variable para la conexión serial

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/open', methods=['POST'])
def open_connection():
    global ser
    if ser is None or not ser.is_open:
        try:
            ser = serial.Serial('COM2', 9600, timeout=1)  # Asegúrate de usar el puerto correcto
            time.sleep(3)  # Espera un momento para que el Arduino reinicie
            return jsonify({'response': 'Conexión abierta'})
        except serial.SerialException as e:
            return jsonify({'error': str(e)}), 500
    else:
        return jsonify({'response': 'Conexión ya está abierta'})

@app.route('/close', methods=['POST'])
def close_connection():
    global ser
    if ser is not None and ser.is_open:
        ser.close()
        return jsonify({'response': 'Conexión cerrada'})
    else:
        return jsonify({'response': 'Conexión ya está cerrada'})

@app.route('/send', methods=['POST'])
def send():
    global ser
    if ser is None or not ser.is_open:
        return jsonify({'error': 'Conexión no está abierta'}), 400

    data = request.json
    command = data.get('command')
    if command in ['11', '10', '01', '00']:
        try:
            ser.write(command.encode())
            time.sleep(3)
            response_lines = []
            while True:
                response = ser.readline().decode('utf-8').strip()
                if response:
                    response_lines.append(response)
                else:
                    break
            return jsonify({'response': response_lines})
        except serial.SerialException as e:
            return jsonify({'error': str(e)}), 500
    else:
        return jsonify({'error': 'Invalid command'}), 400

if __name__ == '__main__':
    app.run(debug=True)
