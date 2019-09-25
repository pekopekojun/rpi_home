# Link

- [Setup Raspberyy pi](Raspberrypi_setup.md)
- [Setup TensorFlow](TensorFlow_setup.md)
- [Setup Let's Encrypt and Lego](Lets_encrypt_and_lego.md)

# Tips

## Shutdown
~~~
sudo shutdown -h now
~~~

## Flask
~~~
sudo pip3 install flask
~~~

## npm forever
~~~
npm install -g forever

forever start speach_server.js
~~~

## Systemd

Refer to https://qiita.com/molchiro/items/ee32a11b81fa1dc2fd8d

```
sudo nano /etc/systemd/system/cat_det.service
```

```
[Unit]
Description=Cat image server

[Service]
WorkingDirectory= /home/pi/
ExecStart=/usr/bin/python3 /home/pi/image_server.py
EnvironmentFile=/home/pi/userenv

[Install]
WantedBy=multi-user.target
```

systemd
```
#eneble
sudo systemctl enable cat_det.service

#stop
sudo systemctl stop cat_det.service

#start
sudo systemctl start cat_det.service

# reload
sudo systemctl daemon-reload

#log 
journalctl
journalctl -u cat_det.service
```

## 間違ってpushしたファイルの取り消し方法

~~~
git filter-branch --index-filter "git rm -rf --cached --ignore-unmatch ひみつ.txt" --prune-empty -- --all
git push -f
~~~
