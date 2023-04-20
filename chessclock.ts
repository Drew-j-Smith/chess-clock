const serviceUuid = "00000000-0000-1000-8000-00805f9b34fb";
const characteristicsMap = {
  "15ee2281-96c3-47b9-8041-e4c3723d053d": "stateCharacteristic",
  "3c5b2478-a783-4eeb-acfa-0a9c48171d30": "leftTimeCharacteristic",
  "e46c0d94-a1bc-4e29-9ce6-c2b8899d7622": "rightTimeCharacteristic",
  "ec8c5792-e5c7-485c-a1c5-8f43449ba0b5": "leftResetTimeCharacteristic",
  "ade8080a-8d5e-48c4-b302-932b08c21bc1": "rightResetTimeCharacteristic",
  "d9a4242e-2f41-471b-8bca-00b7650a053c": "leftIncrementCharacteristic",
  "a98cfae5-3fe4-452c-8c1f-7a15820b456e": "rightIncrementCharacteristic",
  "8dcd2bd0-e4d8-416e-b2e9-cf8231cb28de": "leftDelayCharacteristic",
  "084ed1a2-da8e-48d2-9dcc-c328753639c2": "rightDelayCharacteristic",
};

async function pair() {
  console.log("Requesting Bluetooth Device...");
  const device = await navigator.bluetooth.requestDevice({
    filters: [{ services: [serviceUuid] }],
  });

  console.log("Connecting to GATT Server...");
  if (!device.gatt) {
    return;
  }
  const server = await device.gatt.connect();

  console.log("Getting Service...");
  const service = await server.getPrimaryService(serviceUuid);

  const characteristics = await service.getCharacteristics();
  characteristics.forEach(async (c) => {
    console.log(
      `uuid: ${c.uuid} description: ${characteristicsMap[c.uuid]} value: ${(
        await c.readValue()
      ).getInt32(0, true)}`
    );
  });
}
