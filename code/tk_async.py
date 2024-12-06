import tkinter as tk
import tk_async_execute as tae
import asyncio, datetime
from bleak import BleakScanner, BleakClient
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
from bleak.backends.characteristic import BleakGATTCharacteristic

UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
ADDRESS = "D77450E0-B097-2719-A036-B77E03F58860"

start = datetime.datetime.now()

def handle_disconnect(client):
		print("Device was disconnected, goodbye.")
		tae.stop()

def handle_rx(_: BleakGATTCharacteristic, data: bytearray):
		time = (datetime.datetime.now() - start).seconds
		rec = str(time) + ","+str(data.decode()).strip()
		t.config(text = rec)
		
async def read():
	client = BleakClient(ADDRESS, disconnect_callback=handle_disconnect)
	try:
		t.config(text="Connecting ...")
		await client.connect()
		await client.start_notify(UART_TX_CHAR_UUID, handle_rx)
		while status.get():
			await asyncio.sleep(2)
		else:
			await client.disconnect()
			tae.stop()
	except asyncio.CancelledError:
		print("Collection cancelled")
	except Exception as e:
		print(e)
	finally:
		await client.disconnect()
		tae.stop()
			
async def button_click_async():
	print("Async function: button clicked")
	await asyncio.sleep(5)
	print("After sleep")
		
def button_click_command():
	if status.get() == True:
		status.set("False")
	else:
		status.set("True")
		tae.async_execute(read(), wait=True)

root = tk.Tk()
b = tk.Button(root, text="Click me", command=button_click_command, width=20)
b.pack()
t = tk.Label(root, text="Hello Tkinter")
t.pack()
status = tk.BooleanVar(root)
status.set(False)

tae.start()
root.mainloop()
tae.stop()