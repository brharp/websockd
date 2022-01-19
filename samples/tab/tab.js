
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
  switch (event.key) {
    case 'ArrowLeft':
    case 'h':
      ws.send('h');
      break;
    case 'ArrowRight':
    case 'l':
      ws.send('l');
      break;
    case 'ArrowDown':
    case 'j':
      ws.send('j');
      break;
    case 'ArrowUp':
    case 'k':
      ws.send('k');
      break;
    default:
      ws.send(event.key);
      break;
  }
});


