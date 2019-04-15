class Room {
  constructor(roomname) {
    this._first = null;
    this._second = null;
    this._name = roomname;
    this._pingAknReceivedFromFirst = 0;
    this._pingAknReceivedFromSecond = 0;
    this._firstScore = 0;
    this._secondScore = 0;
    this._turn = 0; //1-first, 2-second
    this._turnEndedAtFirst = 1;
    this._turnEndedAtSecond = 1;
    this._lastTurnGoal = 0;
    console.log("Room created "+this.name);
  }
  // set name(name) {
  //   this._name = name.charAt(0).toUpperCase() + name.slice(1);
  // }
  get name() {
    return this._name;
  }

  get first() {
    return this._first;
  }

  get second() {
    return this._second;
  }
  get pingAknReceivedFromFirst() {
    return this._pingAknReceivedFromFirst;
  }
  get pingAknReceivedFromSecond() {
    return this._pingAknReceivedFromSecond;
  }

  get isPingAknReceivedFromBothPlayers() {
    return this._pingAknReceivedFromFirst && this._pingAknReceivedFromSecond;
  }

  set first(connectionPtr) {
    this._first = connectionPtr;
    console.log("first set " + this._first.remoteAddress);
  }

  set second(connectionPtr) {
    this._second = connectionPtr;
    console.log("second set " + this._second.remoteAddress);
  }

  get isWaiting() {
    return this._first == null || this._second == null;
  }

  isMemberOfThisRoom(remoteAddress) {
    // console.log("search for "+remoteAddress);
    // console.log("first "+this._first.remoteAddress);
    // console.log("second "+this._second.remoteAddress);

    
    if (this._first !=null && getIP(this._first) == remoteAddress) {
      return 1;
    }
    if (this._second !=null && getIP(this._second) == remoteAddress) {
      return 2;
    }
    return 0;
  }

  startTurn(id) {
    //turn
    var cmd_turn = {};
    cmd_turn['cmd'] = {};
    cmd_turn['cmd']['id'] = 'turn';
    var jturncmd = JSON.stringify(cmd_turn);

    var cmd_turn_op = {};
    cmd_turn_op['cmd'] = {};
    cmd_turn_op['cmd']['id'] = 'turn_op';
    var jturnopcmd = JSON.stringify(cmd_turn_op);

    this._lastTurnGoal = 0;
    if (id ==1) {
      this._turn = 1;
      this._turnEndedAtFirst = 0;
      this._turnEndedAtSecond = 0;
      this._first.send(jturncmd);
      this._second.send(jturnopcmd);
    } else if (id ==2){
      this._turn = 2;
      this._turnEndedAtFirst = 0;
      this._turnEndedAtSecond = 0;
      this._second.send(jturncmd);
      this._first.send(jturnopcmd);
    } else {
      console.warn('invalid id '+id);
    }
  }

  startGame() {
    console.log("startGame for room "+this.name);
    var cmd_startgame = {};
    cmd_startgame['cmd'] = {};
    cmd_startgame['cmd']['id'] = 'startgame';
    var jcmd = JSON.stringify(cmd_startgame);
    this._first.send(jcmd);
    this._second.send(jcmd);
    this.startTurn(1);
  }

  initGame() {
    console.log("initGame for room "+this.name);
    var cmd_first = {};
    cmd_first['cmd'] = {};
    cmd_first['cmd']['id'] = 'playerid';
    cmd_first['cmd']['value'] = 'first';
    var cmd_second = {};
    cmd_second['cmd'] = {};
    cmd_second['cmd']['id'] = 'playerid';
    cmd_second['cmd']['value'] = 'second';
    this._first.send(JSON.stringify(cmd_first));
    this._second.send(JSON.stringify(cmd_second));
  }

  sendScoreUpdate() {
    console.log("sendScoreUpdate for room "+this.name);
    var cmd_score = {};
    cmd_score['cmd'] = {};
    cmd_score['cmd']['id'] = 'score';
    cmd_score['cmd']['value'] = {};
    cmd_score['cmd']['value']['first'] = this._firstScore;
    cmd_score['cmd']['value']['second'] = this._secondScore;
    var score_str = JSON.stringify(cmd_score);
    this._first.send(score_str);
    this._second.send(score_str);

    console.log("<==="+JSON.stringify(score_str));
  }

  stopGame(connection) {
    console.log("stopGame for room "+this.name);
    var cmd_stopgame = {};
    cmd_stopgame['cmd'] = {};
    cmd_stopgame['cmd']['id'] = 'stopgame';
    var jcmd = JSON.stringify(cmd_stopgame);

    if (connection != this._first) {
      if (this._first) {
        this._first.send(jcmd);
        this._first.close();
      }
    } else {
      if (this._second) {
        this._second.send(jcmd);
        this._second.close();
      }
    }
  }
  // sayHello() {
  //   console.log('Hello, my name is ' + this.name + ', I have ID: ' + this.id);
  // }
}

// var justAGuy = new Person();
// justAGuy.name = 'martin'; // The setter will be used automatically here.
// justAGuy.sayHello(); // Will output 'Hello, my name is Martin, I have ID: id_1'

var rooms = [];

var WebSocketServer = require('websocket').server;
var http = require('http');

var server = http.createServer(function(request, response) {
  // process HTTP request. Since we're writing just WebSockets
  // server we don't have to implement anything.
});
server.listen(3000, function() { });

// create the server
wsServer = new WebSocketServer({
  httpServer: server
});

// WebSocket server
wsServer.on('request', function(request) {
  var connection = request.accept(null, request.origin);
  console.log("new connection "+getIP(connection));
  // check for any unfilled rooms
  var r = waitingRoom();
  if (r) {
    console.log("some one waiting ... ");
    r.second = connection;
    r.initGame();
  } else {
    var newRoom = new Room("room"+rooms.length);
    newRoom.first = connection;
    rooms.push(newRoom);
  }

  // This is the most important callback for us, we'll handle
  // all messages from users here.
  connection.on('message', function(message) {
    if (message.type === 'utf8') {
      // process WebSocket message
      // console.log(message.utf8Data);
      // console.log("connection msg "+getIP(connection));
      messagePass(connection, message.utf8Data);
      // setTimeout(function() {
      //   messagePass(connection, message.utf8Data);
      // }, 150);
    }
  });

  connection.on('close', function(socket) {
    // close user connection
    console.log("connection closed "+getIP(connection));
    stopGame(connection);
  });
});

function getIP(connection) {
  if (connection) {
    return connection.remoteAddress + ":" + connection.socket.remotePort;
  }
  return "";
}

function waitingRoom() {
  for (i=0;i<rooms.length;i++) {
    if (rooms[i].isWaiting) {
      return rooms[i];
    }
  }
  return null;
}

function stopGame(connection) {
  var roomNameToRemove = '';
  var index = -1;
  for (i=0;i<rooms.length;i++) {
    if (rooms[i].isMemberOfThisRoom(getIP(connection)) > 0) {
      rooms[i].stopGame(connection);
      roomNameToRemove = rooms[i].name;
      index = i;
      break;
    }
  }

  if (index>-1) {
    rooms.splice(index, 1);
    console.log("Room removed "+roomNameToRemove);
  }
}

function messagePass(connection, msg) {
  console.log('===> '+ msg);
  var jobj = JSON.parse(msg);
  var cmdID = jobj['cmd']['id'];
  for (i=0;i<rooms.length;i++) {
    var playerID = rooms[i].isMemberOfThisRoom(getIP(connection))
    if (playerID > 0) {
      if (playerID == 1) {
        if (cmdID == "ping_akn") {
          rooms[i]._pingAknReceivedFromFirst = 1;
          if (rooms[i]._pingAknReceivedFromSecond) {
            rooms[i].startGame();
          }
        } else if (cmdID == "goal") {
          if (rooms[i]._turn == playerID) {
            rooms[i]._firstScore++;
            rooms[i]._lastTurnGoal = playerID;
            rooms[i].sendScoreUpdate();
          } else {
            console.log('goal came from first. But its not my turn.');
          }
        } else if (cmdID == "turnend") {
          rooms[i]._turnEndedAtFirst = 1;
          if (rooms[i]._turnEndedAtSecond) {
            var nextTurn = (rooms[i]._turn==rooms[i]._lastTurnGoal)?rooms[i]._turn : (rooms[i]._turn==1)?2:1;
            console.log('next turn - '+nextTurn);
            rooms[i].startTurn(nextTurn);
          } else {
            console.log('waiting for second players turn to finish.');
          }
        } else {
          if (rooms[i].second) {
            rooms[i].second.send(msg);
          }
        }
      } else if (playerID == 2) {
        if (cmdID == "ping_akn") {
          rooms[i]._pingAknReceivedFromSecond = 1;
          if (rooms[i]._pingAknReceivedFromFirst) {
            rooms[i].startGame();
          }
        } else if (cmdID == "goal") {
          if (rooms[i]._turn == playerID) {
            rooms[i]._secondScore++;
            rooms[i]._lastTurnGoal = playerID;
            rooms[i].sendScoreUpdate();
          } else {
            console.log('goal came from second. But its not my turn.');
          }
        } else if (cmdID == "turnend") {
          rooms[i]._turnEndedAtSecond = 1;
          if (rooms[i]._turnEndedAtFirst) {
            var nextTurn = (rooms[i]._turn==rooms[i]._lastTurnGoal)?rooms[i]._turn : (rooms[i]._turn==1)?2:1;
            console.log('next turn - '+nextTurn);
            rooms[i].startTurn(nextTurn);
          } else {
            console.log('waiting for first players turn to finish.');
          }
        } else {
          if (rooms[i].first) {
            rooms[i].first.send(msg);
          }
        }
      }
      break;
    }
  }
}