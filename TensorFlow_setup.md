
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
