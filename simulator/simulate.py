"""Publish sample motor telemetry to MQTT."""
import argparse, json, random, time
import paho.mqtt.client as mqtt

parser = argparse.ArgumentParser()
parser.add_argument("--host", required=True); parser.add_argument("--port", type=int, default=1883)
parser.add_argument("--topic", required=True); args = parser.parse_args()
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2); client.connect(args.host, args.port, 60); client.loop_start()
try:
    while True:
        alert = random.random() < .15
        current = random.uniform(4.1,5.4) if alert else random.uniform(.8,2.5)
        vibration = random.uniform(18.5,26) if alert else random.uniform(9.5,16)
        data = {"current_a":round(current,2),"vibration_ms2":round(vibration,2),"overload":current>4,"excessive_vibration":vibration>18,"status":"ALERT" if alert else "HEALTHY"}
        client.publish(args.topic, json.dumps(data)); print(data); time.sleep(5)
except KeyboardInterrupt: pass
finally: client.loop_stop(); client.disconnect()

