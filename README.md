# Raspberry pi 3 B+ setup and Tpis

## Update OS and pip
~~~
sudo apt-get update
sudo apt-get -y install vim
sudo apt-get -y upgrade
sudo pip3 install --upgrade pip
lsb_release -a
pip3 --version
~~~


## Raspi config 
~~~
sudo raspi-config
~~~
- Camera: Enable
- SSH: Enable
- Timezone: Tokyo
- Locale: ja-UTF8
- Audio: Force jack
- Update

## Set Static IP

~~~
sudo vim /etc/dhcpcd.conf
~~~

### LAN
~~~
interface eth0
static ip_address=192.168.10.xxx/24
static routers=192.168.10.xxx
static domain_name_servers=192.168.10.xxx
~~~

### Wifi
~~~
interface wlan0
static ip_address=192.168.10.xxx/24
static routers=192.168.10.xxx
static domain_name_servers=192.168.10.xxx
~~~

> routers,domain_name_servers = Router IP or Bridge IP

## SSH Setup

- Keygen
  ~~~
  ssh-keygen -t rsa
  ~~~

- Send to Raspberry pi
  ~~~
  scp -P 22 .ssh/id_rsa.pub pi@192.168.xx.x:/home/pi/Documents  
  ~~~

- Register key
  ~~~
  mkdir ~/.ssh
  cat ~/Documents/id_rsa.pub >> ~/.ssh/authorized_keys
  chmod 700 ~/.ssh/authorized_keys
  ~~~

- Disable Password Authentication
  ~~~
  sudo nano /etc/ssh/sshd_config
  ~~~

  Set "PasswordAuthentication no"

# Python3 ,Tensor Flow and OpenCV

https://qiita.com/masaru/items/658b24b0806144cfeb1c
https://www.pyimagesearch.com/2018/09/19/pip-install-opencv/
https://qiita.com/mt08/items/e8e8e728cf106ac83218

## Python3 (OpenCV入らなくなる)
~~~
sudo apt update
sudo apt install libffi-dev libssl-dev openssl
wget https://www.python.org/ftp/python/3.7.4/Python-3.7.4.tgz
tar zxvf Python-3.7.4.tgz
cd Python-3.7.4
./configure
make
sudo make install
sudo reboot
python3 -V
~~~

## Tensor Flow 

~~~
sudo apt install libatlas-base-dev

sudo pip3 install setuptools

sudo pip3 install tensorflow 
sudo pip3 install matplotlib
sudo pip3 install scipy
~~~

## OpenCV
~~~
sudo apt update
sudo apt -y install libhdf5-dev libhdf5-serial-dev libhdf5-100
sudo apt -y install libqtgui4 libqtwebkit4 libqt4-test python3-pyqt5
sudo apt -y install libatlas-base-dev
sudo apt -y install libjasper-dev
sudo apt -y install libv4l-dev
#sudo reboot
sudo pip3 install opencv-python
sudo pip3 install opencv-contrib-python
~~~

## SSD-Tesonrflow
~~~
git clone https://github.com/balancap/SSD-Tensorflow.git
~~~
unzip ssd_300_vgg.ckpt.zip

## Flask
~~~
sudo pip3 install flask
~~~

# Tips

### Shut down

~~~
sudo shutdown -h now
~~~

### Install forever
~~~
npm install -g forever
~~~

### Start

~~~
forever start speach_server.js
~~~

### 常駐方法

Refer to https://qiita.com/molchiro/items/ee32a11b81fa1dc2fd8d

```
sudo nano /etc/systemd/system/cat_det.service

```

```
[Unit]
Description=do something

[Service]
WorkingDirectory= /home/pi/systemd
ExecStart=/usr/bin/python /home/pi/systemd/a.py

[Install]
WantedBy=multi-user.target
```

```
# reboot
sudo systemctl enable cat_det.service

# no reboot for test
sudo systemctl start cat_det.service
```