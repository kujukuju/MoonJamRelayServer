## API Tutorial

### Simple How To Use

1. Navigate to the moonjam website and generate your secret multiplayer keys. [https://jam.moon2.tv/dashboard](https://jam.moon2.tv/dashboard)
2. **IMPORTANT** Put these keys into your game as hard coded strings. Example: `String moonkey = "moon"; String playerkey = "pleb";`
3. **IMPORTANT** Optionally you can have a variable called `bool moon = true;` or `bool moon = false;`.
4. **IMPORTANT** We will automatically find these key strings and variables _in the source code_, and swap them out when compiling your game.
5. **IMPORTANT** Check the `moon` variable to determine if the client is the moonmoon client, or the player client. Your game will be compiled twice, and the `moon` variable will be swapped in the source code.
6. Create a websocket connection to `wss://relay.moonjam.dev/v1`. If your language/engine requires you to configure your websocket, you should be sending binary data.
7. Once the connection is established, send your 4 character key to connect to your relay server room.
8. Send your data to the server with your secret 4 character key prepended to the beginning of every message.
9. If your client is the moon client, prepend your `moonkey`. If your client is the player client, prepend your `playerkey`.
9. That's it! Continue sending data to the server as your normally would, just be sure you always prepend your secret key.

An example godot project of how to use this API can be found here:
[https://github.com/kujukuju/MoonJamRelayGodotDemo](https://github.com/kujukuju/MoonJamRelayGodotDemo)

Here is the one important file that handles all of the networking:
[https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs](https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs)

### Limitations

The maximum message size is `16384` bytes. Which is huge, so you shouldn't have to worry about this. But exceeding this quota will ignore your message.

The server has a tickrate of `30` ticks per second. As such, you should only be sending packets at a tickrate of `30`, or about every `33` milliseconds. Exceeding this quota will disconnect your client.

In order to connect your client to your server room you need to send at least one message. It's fine if your first message only contains your secret key.

### Important Notes

You should test your game at maximum player capacity by simulating clients. For multiplayer games, when you start climbing above 10 players, and especially upwards of 100 to 600 players, you will notice performance issues that may not have been previously obvious.

For example, in the godot demo game linked above we lazily kept kinematic bodies for all players even though physics wasn't simulated. Due to this the game would become noticeably laggy around 200-400 players. Once we removed these unnecessary physics bodies it could easily support 800+ players. It would have been hard to identify this issue without simulating fake players.

See our python test script for an example of how to simulate these fake connections:
[https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs](https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs)

### Libraries

WebSocket support exists in all major engines. Here is a list of helpful links to get you started:

* Unity
    * https://github.com/endel/NativeWebSocket
* Unreal
    * https://docs.unrealengine.com/4.26/en-US/API/Runtime/WebSockets/IWebSocketsManager/
* Godot
    * https://docs.godotengine.org/en/stable/tutorials/networking/websocket.html
* C#
    * https://docs.microsoft.com/en-us/dotnet/api/system.net.websockets.clientwebsocket
* JavaScript
    * https://developer.mozilla.org/en-US/docs/Web/API/WebSocket
* Python
    * https://github.com/websocket-client/websocket-client

## Understanding The Server (NOT IMPORTANT)

### MoonJam Compilation Process

In order to streamline the process of compiling these games, be sure to follow the instructions above.

That is to say that you're required to submit your game with the moon key and the player key hard coded in your client source code.

You can optionally have a boolean variable named "moon" in your client to determine if it's moon or a normal player playing. For example: `bool moon = false;`.

Your game will automatically be compiled twice. Once with the `moon` variable set to true to generate moon's client, and a second time with the `moon` variable set to false to generate the player client.

Literally just put the boolean and keys anywhere in the source code. As long as `moon = false` or `moon = true` is found, it will be swapped during compilation.

The implication of this is that when your project has a variable named `moon` that's set to `true` it's expected to behave as moon's client. Otherwise when your project has a variable named `moon` that's set to `false` it's expected to behave as the player client.

The player client will be downloadable from the game jam website when your game is on stream.

### Simple Design Introduction (NOT IMPORTANT)

The moonjam relay server is a very simple websocket server that simply accepts messages from a client and then sends those messages to all other clients. You can see that it just "relays" the messages to everyone who is currently connected.

You can imagine the entire server as this:
```c
void on_message(uint8_t* bytes, uint32_t length) {
    for (Connection& connection : m_connections) {
        connection.send(bytes, length);
    }
}
```

![Simple Server Diagram](https://github.com/kujukuju/MoonJamRelayServer/raw/master/chart1.png)

That is to say that for every message this server receives it just relays that message to every other client.

### More Complex Design Introduction (NOT IMPORTANT)

Because this server is meant to be used by multiple people as an API designed for the moonjam, the server expands on the above relay idea and adds "rooms".

Each room is a set of players who receive each others packets.

To use the API you need two separate keys, an authoritative key for moonmoon, and a player key for the viewers. You can generate these keys here: [https://jam.moon2.tv/dashboard](https://jam.moon2.tv/dashboard)

Each gamejam game will have it's own unique room so that other people can't interfere. The moon key and the player key will be linked to the same room. In order to reduce unnecessary traffic rooms will only be created after moonmoon has connected and sent a message.

Similarly if you try to send a message to a room that doesn't yet exist as a player your message will be ignored and you may be disconnected.

![Complex Server Diagram](https://github.com/kujukuju/MoonJamRelayServer/raw/master/chart2.png)

So if you have the moon key "**moon**" and the player key "**pleb**", the way you connect to your room is that every message you send must be prepended with your key.

If you want to send the websocket message "**hey**" as moon, then you should actually send "**moonhey**", and every other client connected to your room will receive this message "**hey**".

Likewise, if you want to send the message "**hey**" as a player, then you should actually send "**plebhey**".

In reality here is what this will look like as data:
```c
hey -> [0x68, 0x65, 0x79]
moonhey -> [0x6d, 0x6f, 0x6f, 0x6e, 0x68, 0x65, 0x79]
plebhey -> [0x70, 0x6c, 0x65, 0x62, 0x68, 0x65, 0x79]
```

To reinforce this I'll mention it again; in order to actually establish your connection to a room you must send at least one message so that the server can identify your secret moon/player key and place you into the corresponding room. It's fine if this message is just simply "**moon**" or "**pleb**".

A limitation to also keep in mind with this relay is that your bottleneck will often be the outgoing bandwidth on the relay. As the number of players grow, the server has to send out more data at a geometric rate. You can use this [Desmos Graph](https://www.desmos.com/calculator/asi1qiv3fy) to get a better idea of what your packet size and tick rate should be for your game.

### Ending Notes

That's about all there is to the server. Although conceptually the server is simple, the details specified here might seem a little confusing. However, actually using the server isn't complex. So if this is overwhelming then ignore everything you read here and just look at the "How To Use" section.

---

## Installation Stuff (NOT IMPORTANT AT ALL) (DON'T EVEN READ THIS)

### Folders

* create `secretkey.txt` in the root project folder, and put your secret key in there.
* create a `keys` folder in the root project folder.

### Installation on linux

* as root
    * `sudo apt-get update`
    * `adduser moonjam`
    * `sudo apt-get install nginx`
    * `sudo apt-get install cmake`
    * `sudo apt-get install gcc clang gdb build-essential`
    * `sudo apt-get install ninja-build`
    * `sudo snap install --classic certbot`
    * `sudo certbot --nginx`
    * `crontab -e`
        * `0 0 1 * * certbot renew && systemctl reload nginx`
* as moonjam
    * `ssh-keygen -t rsa -b 4096 -C "kuju@veraegames.com"`
    * `cat ~/.ssh/id_rsa.pub`
    * `git clone git@github.com:kujukuju/MoonJamRelayServer.git`
    * `cd MoonJamRelayServer`
    * `mkdir keys`
    * `vim secretkey.txt`
    * `chmod +x deploy.sh`
    * `chmod +x restart.sh`
* as root
    * `ln -s /home/moonjam/MoonJamRelayServer/moonjam-relay.service /etc/systemd/system/moonjam-relay.service`
    * `systemctl daemon-reload`
    * `systemctl enable moonjam-relay`
    * `ln -s /home/moonjam/MoonJamRelayServer/relay.veraegames.com /etc/nginx/sites-enabled/relay.veraegames.com`
    * `systemctl reload nginx`
    * optionally just copy the exe over... I ran out of memory so thats what I did
* as moonjam
    * `./deploy.sh`
* as root
    * `chmod +x build/MoonJamRelayServer`
## Notes

* To connect to a room you must send at least one message. So sending an initial message with just your key might be desirable.
