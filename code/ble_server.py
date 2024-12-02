import asyncio
from bleak import BleakScanner, BleakClient
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
from bleak.backends.characteristic import BleakGATTCharacteristic
import datetime, matplotlib.pyplot as pl

UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

async def loop():	
	device = await BleakScanner.find_device_by_name("UART Service")
    
	if device is None:
		print("no UART matching device found")
		sys.exit(1)	

	start = datetime.datetime.now()
	with open("static/db_file.csv","w") as file:
		file.write("# ")
		file.write(start.isoformat())
		file.write("\n")

	fig, ax = pl.subplots()
	xdata, ydata = [], []
	ln, = ax.plot([], [], 'ro')
	ax.set_ylim(2100,2800)
	pl.ion()
	
	def handle_disconnect(_: BleakClient):
		print("Device was disconnected, goodbye.")
		for task in asyncio.all_tasks():
			task.cancel()

	def handle_rx(_: BleakGATTCharacteristic, data: bytearray):
		time = (datetime.datetime.now() - start).seconds
		rec = str(time) + ","+str(data.decode()).strip()
		with open("static/db_file.csv","a") as file:
			file.write(rec)
			file.write("\n")
		t,r,h = rec.split(',')
		xdata.append(int(t))
		ydata.append(int(r))
		if len(xdata) > 60:
			xdata.pop(0)
			ydata.pop(0)
		ln.set_data(xdata,ydata)
		ax.set_xlim(min(xdata),max(xdata))
		fig.canvas.draw()
		fig.canvas.flush_events()
		pl.show()
		
	async with BleakClient(device, disconnected_callback=handle_disconnect) as client:
		await client.start_notify(UART_TX_CHAR_UUID, handle_rx)
		try:
			while True:
				await asyncio.sleep(2)
		except asyncio.CancelledError:
			print("Collection cancelled")

if __name__ == "__main__":
	try:
		asyncio.run(loop())
	except asyncio.CancelledError:
        # task is cancelled on disconnect, so we ignore this error
		pass
