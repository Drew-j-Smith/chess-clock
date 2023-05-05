

let chessClock = new ChessClock(async (state) => {
  console.log(`state: ${state} leftTime: ${await chessClock.readValue("leftTime")} rightTime: ${await chessClock.readValue("rightTime")}`);
});
document.getElementById("start").addEventListener("click", async () => { chessClock.pair(); });
