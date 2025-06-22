import base64
from flask import Flask, request, jsonify
import os
import json
from flask_cors import CORS  # <--- NEW

app = Flask(__name__)
CORS(app)  # <--- Allow all cross-origin requests (useful for embedded clients)

UPLOAD_FOLDER = 'images'
LOG_FILENAME = 'images_log.json'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

log_path = os.path.join(UPLOAD_FOLDER, LOG_FILENAME)

def load_log():
    if os.path.exists(log_path):
        with open(log_path, 'r') as f:
            try:
                return json.load(f)
            except json.JSONDecodeError:
                return []
    return []

def save_log(log_data):
    with open(log_path, 'w') as f:
        json.dump(log_data, f, indent=4)

@app.route('/upload_interrupt', methods=['POST'])
def upload_interrupt():
    try:
        data = request.get_json(force=True)
    except Exception as e:
        return jsonify({"error": f"Invalid JSON - {str(e)}"}), 400

    required_fields = ['image', 'distance', 'imagename']
    if not all(field in data for field in required_fields):
        return jsonify({"error": "Missing fields"}), 400

    try:
        image_data = base64.b64decode(data['image'])
        image_name = data['imagename']
        distance = data['distance']

        # Save the image
        image_path = os.path.join(UPLOAD_FOLDER, image_name)
        with open(image_path, 'wb') as f:
            f.write(image_data)

        # Log data
        log = load_log()
        log.append({
            "imagename": image_name,
            "distance_cm": distance
        })
        save_log(log)

        return jsonify({
            "status": "success",
            "distance": distance,
            "imagename": image_name,
            "logfile": LOG_FILENAME
        }), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    print("Starting Flask server...")
    app.run(host="0.0.0.0", port=5000, debug=True)
