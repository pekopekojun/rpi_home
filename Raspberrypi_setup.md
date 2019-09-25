# Update OS and pip
~~~
sudo apt-get update
sudo apt-get -y install vim
sudo apt-get -y upgrade
sudo pip3 install --upgrade pip
lsb_release -a
pip3 --version
~~~

# Raspi config 

~~~
sudo raspi-config
~~~

- Camera: Enable
- SSH: Enable
- Timezone: Tokyo
- Locale: ja-UTF8
- Audio: Force jack
- Update

# Set RAM Disk

~~~
sudo nano /etc/fstab
~~~

~~~
tmpfs  /tmp      tmpfs defaults,size=32m,noatime,mode=1777 0 0
tmpfs  /var/tmp  tmpfs defaults,size=16m,noatime,mode=1777 0 0
tmpfs  /var/log  tmpfs defaults,size=16m,noatime,mode=0755 0 0
tmpfs  /home/pi/ramdisk tmpfs defaults,size=32m,noatime,mode=1777 0 0
~~~

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


## Set SSH

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


## Set CPU Clock

~~~
# Check clock
vcgencmd measure_clock arm

# Set performance mode
sudo cpufreq-set -g performance
cpufreq-info -p
cpufreq-info -f
~~~