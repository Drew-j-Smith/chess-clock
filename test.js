var myCharacteristic;

function log(str) {
    console.log(str);
}

function onStartButtonClick() {
  let serviceUuid = '00000000-0000-1000-8000-00805f9b34fb';
  let characteristicUuid = '741c12b9-e13c-4992-8a5e-fce46dec0bff';

  log('Requesting Bluetooth Device...');
  navigator.bluetooth.requestDevice({filters: [{services: [serviceUuid]}]})
  .then(device => {
    log('Connecting to GATT Server...');
    return device.gatt.connect();
  })
  .then(server => {
    log('Getting Service...');
    return server.getPrimaryService(serviceUuid);
  })
  .then(service => {
    log('Getting Characteristic...');
    return service.getCharacteristic(characteristicUuid);
  })
  .then(characteristic => {
    myCharacteristic = characteristic;
    return myCharacteristic.startNotifications().then(_ => {
      log('> Notifications started');
      myCharacteristic.addEventListener('characteristicvaluechanged',
          handleNotifications);
    });
  })
  .catch(error => {
    log('Argh! ' + error);
  });
}

function onStopButtonClick() {
  if (myCharacteristic) {
    myCharacteristic.stopNotifications()
    .then(_ => {
      log('> Notifications stopped');
      myCharacteristic.removeEventListener('characteristicvaluechanged',
          handleNotifications);
    })
    .catch(error => {
      log('Argh! ' + error);
    });
  }
}

function onSetButtonClick() {
  if (!myCharacteristic) {
    return;
  }
  let encoder = new TextEncoder('utf-8');
  let value = document.querySelector('#state').value;
  log('Setting Characteristic User Description...');
  log(encoder.encode(value).length)
  myCharacteristic.writeValue(encoder.encode(value))
  .then(_ => {
    log('> Characteristic User Description changed to: ' + value);
  })
  .catch(error => {
    log('Argh! ' + error);
  });
}

function handleNotifications(event) {
  log(new TextDecoder("utf-8").decode(event.target.value.buffer));
}

document.getElementById("start").addEventListener("click", pair);
document.getElementById("stop").addEventListener("click", onStopButtonClick);
document.getElementById("set").addEventListener("click", onSetButtonClick);

