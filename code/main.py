import machine, time, network, urequests, dht, pms5003

d = dht.DHT11(machine.Pin(10))

pms5003 = pms5003.PMS5003(
    uart=machine.UART(1, tx=machine.Pin(5), rx=machine.Pin(8), baudrate=9600),
    pin_enable=None, pin_reset=None
)

wlan = network.WLAN(network.WLAN.IF_STA)
wlan.active(True)

print("connecting to WiFi",end='')
#wlan.connect("TSU-STUDENT","")
wlan.connect("opossum","cine e buby?")
for i in range(10):
    time.sleep(0.4)
    print(".", end='')
    if wlan.isconnected():
        break
print("")
print("IP address:", wlan.ipconfig('addr4'))


for i in range(20):
    time.sleep(5)
    d.measure()
    data = (d.temperature(), d.humidity()) + pms5003.read().data[6:-2]
    payload = ":".join([str(x) for x in data])
    print(payload)
#    response = urequests.get("http://mac-mini/collector/record/"+payload, headers = headers)
#    print(response.status_code)
