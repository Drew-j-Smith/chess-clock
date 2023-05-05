const serviceUuid = "00000000-0000-1000-8000-00805f9b34fb";
const characteristicsMap = new Map([
  ["15ee2281-96c3-47b9-8041-e4c3723d053d", "state"],
  ["3c5b2478-a783-4eeb-acfa-0a9c48171d30", "leftTime"],
  ["e46c0d94-a1bc-4e29-9ce6-c2b8899d7622", "rightTime"],
  ["ec8c5792-e5c7-485c-a1c5-8f43449ba0b5", "leftResetTime"],
  ["ade8080a-8d5e-48c4-b302-932b08c21bc1", "rightResetTime"],
  ["d9a4242e-2f41-471b-8bca-00b7650a053c", "leftIncrement"],
  ["a98cfae5-3fe4-452c-8c1f-7a15820b456e", "rightIncrement"],
  ["8dcd2bd0-e4d8-416e-b2e9-cf8231cb28de", "leftDelay"],
  ["084ed1a2-da8e-48d2-9dcc-c328753639c2", "rightDelay"],
]);

class ChessClock {
  #server: BluetoothRemoteGATTServer | undefined;
  #characteristics: Map<string, BluetoothRemoteGATTCharacteristic> | undefined;
  #stateChangeCallback: (state: number) => void;

  constructor(stateChangeCallback: (state: number) => void) {
    this.#stateChangeCallback = stateChangeCallback;
  }

  connected() {
    return (
      this.#server !== undefined &&
      this.#server.connected &&
      this.#characteristics !== undefined
    );
  }

  disconnect() {
    if (this.#server) {
      this.#server.disconnect();
    }
    this.#server = undefined;
    this.#characteristics = undefined;
  }

  async readValue(characteristic: string) {
    if (!this.connected() || !this.#characteristics) {
      throw new Error("Not connected");
    }
    const c = this.#characteristics.get(characteristic);
    if (!c) {
      throw new Error(`Invalid characteristic: ${characteristic}`);
    }
    return (await c.readValue()).getInt32(0, true);
  }

  async writeValue(characteristic: string, value: number) {
    if (!this.connected() || !this.#characteristics) {
      throw new Error("Not connected");
    }
    const c = this.#characteristics.get(characteristic);
    if (!c) {
      throw new Error(`Invalid characteristic: ${characteristic}`);
    }
    const data = new ArrayBuffer(4);
    const view = new DataView(data);
    view.setInt32(0, value, true);
    await c.writeValue(data);
  }

  async pair() {
    console.log("Requesting Bluetooth Device...");
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: [serviceUuid] }],
    });

    console.log("Connecting to GATT Server...");
    if (!device.gatt) {
      throw new Error("GATT Server unavailable");
    }
    this.#server = await device.gatt.connect();

    console.log("Getting Service...");
    const service = await this.#server.getPrimaryService(serviceUuid);

    console.log("Getting Characteristics...");
    const characteristics = await service.getCharacteristics();

    this.#characteristics = new Map(
      characteristics.map((c) => [
        characteristicsMap.get(c.uuid) || "unknown",
        c,
      ])
    );
    const stateCharacteristic = this.#characteristics.get("state");
    if (!stateCharacteristic) {
      throw new Error("Characteristic unavailable");
    }
    await stateCharacteristic.startNotifications();
    this.#characteristics
      .get("state")
      ?.addEventListener("characteristicvaluechanged", async () => {
        this.#stateChangeCallback(
          stateCharacteristic.value?.getInt32(0, true) || 0
        );
      });
    console.log("Connected!");
  }
}
