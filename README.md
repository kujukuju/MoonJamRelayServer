## Installation

* create `secretkey.txt` in the root project folder, and put your secret key in there

## Installation on linux

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
