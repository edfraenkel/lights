from pylab import *
from numpy import *
import time
import serial


K = 0.0143877735 # m K

#        wavelength, relative intensity
blue =  (462E-9, 48*3, 'b')
green = (520E-9, 12*5, 'g')
red =   (632E-9, 24*2.5, 'r')

def planck(l, T):
    return 1/(l**5*(exp(K/(l*T))-1))

wavelengths = linspace(400, 700, 2000)*1E-9
temperatures = linspace(100, 7000, 2000)

f = figure(figsize=(12, 6))
ax1 = f.add_subplot(121)
ax2 = f.add_subplot(122)
ax2.set_yscale('log')
for T in (1000, 2000, 3000, 4000, 5000):
    for ax in ax1, ax2:
        ax.plot(wavelengths, planck(wavelengths, T), label="T={}".format(T))
        ax.set_xlabel("wavelength")
        ax.set_ylabel("intensity")

color_intensities = list()
f = figure(figsize=(12, 6))
ax1 = f.add_subplot(121)
ax2 = f.add_subplot(122)
ax2.set_yscale('log')
for color in (blue, green, red):
    wavelength, intensity, c = color
    color_intensity = planck(wavelength, temperatures)
    for ax in ax1, ax2:
        ax.plot(temperatures, color_intensity, color=c, label=c.upper())
        ax.set_xlabel("temperature")
        ax.set_ylabel("intensity")
    color_intensities.append(color_intensity)


relative_color_intensities = vstack(color_intensities)
#relative_color_intensities[0] /= blue[1]
#relative_color_intensities[1] /= green[1]
#relative_color_intensities[2] /= red[1]
max_intensities = amax(relative_color_intensities, 0)
relative_color_intensities = 255 * relative_color_intensities / max_intensities 

color_intensities = list()
f = figure(figsize=(12, 6))
ax1 = f.add_subplot(121)
ax2 = f.add_subplot(122)
ax2.set_yscale('log')
for i, color in enumerate((blue, green, red)):
    wavelength, intensity, c = color
    relative_color_intensity = relative_color_intensities[i]
    for ax in ax1, ax2:
        ax.plot(temperatures, relative_color_intensity, color=c, label=c.upper())
        ax.set_xlabel("temperature")
        ax.set_ylabel("relative intensity")
    color_intensities.append(color_intensity)


# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
	port='//./COM7',
	baudrate=9600,
	parity=serial.PARITY_ODD,
	stopbits=serial.STOPBITS_TWO,
	bytesize=serial.SEVENBITS
)

if not ser.isOpen:
    ser.open()

while True:
    t = time.time()
    for i, (b, g, r) in enumerate(transpose(relative_color_intensities)):
        r = min(temperatures[i]/3, r)
        command = bytes("[NeoIkeaA][color] {} {} {} [END]".format(int(r), int(g), int(b)), "UTF8")
#        print(b'>> ' + command)
        ser.write(command)
        out = b""
        while ser.inWaiting() > 0:
            out += ser.read(1)
#        print(b'<< ' + out)
    print(time.time()-t)


 



ax2.legend()

show()