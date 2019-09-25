
# MyDNS https 化

https://mattn.kaoriya.net/software/lego-mydns.htm

~~~
curl -OL https://github.com/go-acme/lego/releases/download/v3.0.2/lego_v3.0.2_linux_armv7.tar.gz
~~~

~~~
export MYDNSJP_MASTER_ID=
export MYDNSJP_PASSWORD=

# renew で更新
./lego -m main@yahoo.co.jp -a --dns mydnsjp -s https://acme-v02.api.letsencrypt.org/directory -d xxx.mydns.jp -d *.rpi-home1983.mydns.jp run

~~~
