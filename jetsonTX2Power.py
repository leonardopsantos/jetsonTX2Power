# -*- coding: utf-8 -*-

import os
import argparse
import datetime
import csv
from collections import OrderedDict

class PowerReadingValue(object):
	def __init__(self, name, path):
	    self.name = name
	    self.path = path
	    self.value = ""

	def update(self):
		with open(self.path, 'r') as myfile:
		    self.value=myfile.read().rstrip()

	def __str__(self):
	    return "{0} : {1}".format(self.path, self.value)

	def get_name(self):
		return self.name

	def get_value(self):
		return self.value

class PowerReadingVoltage(PowerReadingValue):
	def __init__(self, path, number):
	    PowerReadingValue.__init__(self, "voltage", path+"/in_voltage{0}_input".format(number))

class PowerReadingCurrent(PowerReadingValue):
    def __init__(self, path, number):
	PowerReadingValue.__init__(self, "current", path+"/in_current{0}_input".format(number))

class PowerReadingRail(object):
	def __init__(self, path, num):
	    with open(path+"/rail_name_{0}".format(num), 'r') as myfile:
		self.name = myfile.read().rstrip()
	    self.voltage = PowerReadingVoltage(path, num)
	    self.current = PowerReadingCurrent(path, num)

	def __str__(self):
	    s = self.name + "\n"
	    s += "\t" + str(self.voltage) + "\n"
	    s += "\t" + str(self.current) + "\n"
	    return s

	def get_name(self):
	    return self.name

	def update(self):
	    self.voltage.update()
	    self.current.update()

	def to_csv(self):
	    d=OrderedDict()
	    d[self.name+' '+self.voltage.get_name()]=self.voltage.get_value()
	    d[self.name+' '+self.current.get_name()]=self.current.get_value()
	    return d

	def get_csv_header(self):
	    return [self.name+' '+self.voltage.get_name(), self.name+' '+self.current.get_name()]

class PowerReadingDevice(object):
	def __init__(self, path):
		self.rails = []
		for i in range(3):
		    self.rails.append(PowerReadingRail(path, i))

	def __str__(self):
	    s=""
	    for r in self.rails:
		s += str(r)
	    return s

	def update(self):
		for r in self.rails:
		    r.update()

	def get_csv_header(self):
		s=[]
		for r in self.rails:
		    s.extend(r.get_csv_header())
		return s

	def to_csv(self):
		s=OrderedDict()
		for r in self.rails:
			s.update(r.to_csv())
		return s


def to_csv(csv_file, devices, comment='', xtra_fields=None):

	open_flags='w+'

	if os.path.isfile(csv_file):
	    open_flags='a+'

	csv_row=OrderedDict()

	now = datetime.datetime.now()
	csv_row['date']=now.strftime("%Y-%m-%d")
	csv_row['time']=now.strftime("%H:%M:%S")+':'+str(now.microsecond/1000)

	for d in devices:
		d.update()
		csv_row.update(d.to_csv())

	if xtra_fields is not None:
		csv_row.update(xtra_fields)
	else:
		csv_row['comments']=comment

	with open(csv_file, open_flags) as csvfile:
		writer = csv.DictWriter(csvfile, fieldnames=csv_row.keys())
		if open_flags=='w+':
			writer.writeheader()
		writer.writerow(csv_row)


def print_power(devices):
	for d in devices:
		d.update()
		print d

def create_devices():
	devices = []

	paths= ["/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device",
		"/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device",
		"/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device",
		"/sys/devices/3160000.i2c/i2c-0/0-0043/iio_device"]
	for p in paths:
		devices.append(PowerReadingDevice(p))

	return devices

def main():
	parser = argparse.ArgumentParser(description='Reads the Jetson TX2 voltage and current sensors')
	parser.add_argument('-f', '--file', help='File output for the CSV output')

	args = parser.parse_args()

	devices = create_devices()

	if args.file == None:
	    print_power(devices)
	else:
	    to_csv(args.file, devices)

if __name__ == "__main__":
	main()
