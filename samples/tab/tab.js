
cv = document.getElementById('cv')
cx = cv.getContext("2d")
cx.fillStyle = 'black'
url = "ws://" + window.location.hostname + ":8002/tab";
ws = new WebSocket(url);

ws.onmessage = function(event) {
  //console.log(event.data);
  Function(event.data)();
};

document.body.addEventListener("keyup", (event) => {
  let cmd = (event.ctrlKey ? "Ctrl" : "") + event.key;
  console.log(cmd);
  ws.send(cmd);
});


