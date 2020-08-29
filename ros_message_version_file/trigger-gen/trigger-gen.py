print('import libs, it takes a while on Yun')
from time import sleep
import subprocess
import roslibpy
print('libs imported, initial sys')

subprocess.call('echo 1 > /sys/class/gpio/gpio21/value', shell=True)
subprocess.call('echo 26 > /sys/class/gpio/export', shell=True)
subprocess.call('echo "high" > /sys/class/gpio/gpio26/direction', shell=True)
subprocess.call('echo 0 > /sys/class/gpio/gpio26/value', shell=True)

i = 10
print('start ptp slave')
subprocess.call('ptp4l -A -2 -S -i eth1 &', shell=True)
while (i>0):
    sleep(1)
    print('PTP ready in %d s' % i)
    i = i-1

print('Connecting to ROS...')
client = roslibpy.Ros(host='192.168.0.48', port=9090)
client.run()
print('Is ROS connected?', client.is_connected)

talker = roslibpy.Topic(client, '/chatter', 'std_msgs/String')


while client.is_connected:
    result = subprocess.check_output('/root/trigger-gen/get_time', shell=True)
    pps_time = result[:-1]
    print(pps_time)
    talker.publish(roslibpy.Message({'data': pps_time}))
client.terminate()