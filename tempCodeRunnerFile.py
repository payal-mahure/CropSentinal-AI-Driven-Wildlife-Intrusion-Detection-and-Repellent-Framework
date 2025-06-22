import base64
from flask import Flask, request, jsonify
import os
import json

app = Flask(__name__)
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
    data = request.get_json()
    if not data or 'image' not in data or 'distance' not in data or 'imagename' not in data:
        return jsonify({"error": "Missing fields"}), 400

    image_data = base64.b64decode(data['image'])
    image_name = data['imagename']
    distance = data['distance']

    # Save image file
    image_path = os.path.join(UPLOAD_FOLDER, image_name)
    with open(image_path, 'wb') as f:
        f.write(image_data)

    # Load existing log, append new entry, save back
    log = load_log()
    log.append({
        "imagename": image_name,
        "distance_cm": distance
    })
    save_log(log)

    response = {
        "status": "success",
        "distance": distance,
        "imagename": image_name,
        "logfile": LOG_FILENAME
    }

    return jsonify(response), 200

if __name__ == "__main__":
    print("Starting Flask server...")
    app.run(host="0.0.0.0", port=5000, debug=True)
