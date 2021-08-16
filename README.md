## API Tutorial

### Simple How To Use

1. Navigate to the moonjam website and generate your secret multiplayer keys. [https://jam.moon2.tv/dashboard](https://jam.moon2.tv/dashboard)
2. **IMPORTANT** Create a text file in the root directory of your project named `relaykey.txt`.
3. **IMPORTANT** Inside `relaykey.txt` paste your moon key for now.
4. **IMPORTANT** Read your secret key from this file. Erros will use this file to compile your two projects (moon project and pleb project).
5. Create a websocket connection to `wss://relay.moonjam.dev/v1`. If your language/engine requires you to configure your websocket, you should be sending binary data.
7. Once the connection is established, send your key to connect to your relay server room.
8. Send your data to the server with your secret key prepended to every message.
9. That's it! Continue sending data to the server as your normally would, just be sure you always prepend your secret key.

An example project of how to use this API can be found here:
[https://github.com/kujukuju/MoonJamRelayGodotDemo](https://github.com/kujukuju/MoonJamRelayGodotDemo)

Here is the one important file that handles all of the networking:
[https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs](https://github.com/kujukuju/MoonJamRelayGodotDemo/blob/master/Scene.cs)

### Limitations

The maximum message size is `16384` bytes. Which is huge, so you shouldn't have to worry about this. But exceeding this quota will ignore your message.

The server has a tickrate of `30` ticks per second. As such, you should only be sending packets at a tickrate of `30`, or about every `33` milliseconds. Exceeding this quota will disconnect your client.

## Understanding The Server

### Simple Design Introduction

The moonjam relay server is a very simple websocket server that simply accepts messages from a client and then sends those messages to all other clients. You can see that it just "relays" the messages to everyone who is currently connected.

You can imagine the entire server as this:
```c
void on_message(uint8_t* bytes, uint32_t length) {
    for (Connection& connection : m_connections) {
        connection.send(bytes, length);
    }
}
```

[Simple Server Diagram](https://github.com/kujukuju/MoonJamRelayServer/chart1.png)

That is to say that for every message this server receives it just relays that message to every other client.

### More Complex Design Introduction

Because this server is meant to be used by multiple people as an API designed for the moonjam, the server expands on the above relay idea and adds "rooms".

Each room is a set of players who receive each others packets.

To use the API you need two separate keys, an authoritative key for moonmoon, and a pleb key for the viewers. You can generate these keys here: [https://jam.moon2.tv/dashboard](https://jam.moon2.tv/dashboard)

Each gamejam game will have it's own unique room so that other people can't interfere. The moon key and the pleb key will be linked to the same room. In order to reduce unnecessary traffic rooms will only be created after moonmoon has connected and sent a message.

Similarly if you try to send a message to a room that doesn't yet exist as a pleb your message will be ignored and you may be disconnected.

[Complex Server Diagram](https://github.com/kujukuju/MoonJamRelayServer/chart2.png)

So if you have the moon key "**moon**" and the pleb key "**pleb**", the way you connect to your room is that every message you send must be prepended with your key.

If you want to send the websocket message "**hey**" as moon, then you should actually send "**moonhey**", and every other client connected to your room will receive this message "**hey**".

Likewise, if you want to send the message "**hey**" as a pleb, then you should actually send "**plebhey**".

In reality here is what this will look like as data:
```c
hey -> [0x68, 0x65, 0x79]
moonhey -> [0x6d, 0x6f, 0x6f, 0x6e, 0x68, 0x65, 0x79]
plebhey -> [0x70, 0x6c, 0x65, 0x62, 0x68, 0x65, 0x79]
```

To reinforce this I'll mention it again; in order to actually establish your connection to a room you must send at least one message so that the server can identify your secret moon/pleb key and place you into the corresponding room. It's fine if this message is just simply "**moon**" or "**pleb**".

### Ending Notes

That's about all there is to the server. Although conceptually the server is simple, the details specified here might seem a little confusing. However, actually using the server isn't complex. So if this is overwhelming then ignore everything you read here and just look at the "How To Use" section.

---


## Installation Stuff

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
