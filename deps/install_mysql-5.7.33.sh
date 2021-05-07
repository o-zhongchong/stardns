#/bin/sh

CWD=$(dirname $(readlink -f $0))  
cd $CWD

yum -y install numactl
service mysql stop

userdel -f mysql
useradd -r mysql

rm -rf /data/mysql
rm -rf /usr/local/mysql

tar -xzvf mysql-5.7.33-el7-x86_64.tar.gz -C /usr/local/
mv /usr/local/mysql-5.7.33-el7-x86_64 /usr/local/mysql
mkdir -p /var/lib/mysql
mkdir -p /data/mysql
chown -R mysql:mysql /usr/local/mysql
chown -R mysql:mysql /data/mysql
chown -R mysql:mysql /var/lib/mysql
chmod u+x /usr/local/mysql/bin/*

sed -i 's/^basedir/basedir=\/usr\/local\/mysql/g' /usr/local/mysql/support-files/mysql.server
sed -i 's/^datadir/datadir=\/data\/mysql/g' /usr/local/mysql/support-files/mysql.server

cp ./my.cnf /etc/
cp /usr/local/mysql/support-files/mysql.server /etc/init.d/
mv /etc/init.d/mysql.server /etc/init.d/mysql

echo "/usr/local/mysql/lib" > /etc/ld.so.conf.d/mysql.conf
sed -i '/\/usr\/local\/mysql\/bin/d' /etc/profile
echo 'export PATH="/usr/local/mysql/bin:$PATH"' >> /etc/profile

/usr/local/mysql/bin/mysqld --defaults-file=/etc/my.cnf \
--basedir=/usr/local/mysql \
--datadir=/data/mysql \
--user=mysql \
--initialize-insecure \

service mysql start
