import socket
import time
from datetime import datetime

msgFromClient = "Hello UDP Server"
bytesToSend = str.encode(msgFromClient)

serverAddressPort = ("127.0.0.1", 12000)
bufferSize = 1024

# Notice the use of SOCK_DGRAM for UDP packets
UDPClientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#List to store RTT of each ping
RTT = [] 
#Packets recieved
recieved = 0
#Packets lost
lost = 0

i = 0
UDPClientSocket.settimeout(1)
while i < 10:
    try:
        send_time_ms = time.time()
        #Send to server using created UDP socket
        UDPClientSocket.sendto(bytesToSend, serverAddressPort)
        now = datetime.now()

        #Get feedback from the server
        msg, address = UDPClientSocket.recvfrom(bufferSize)
        recv_time_ms = time.time()

        rtt_in_ms = round(recv_time_ms - send_time_ms, 3)*1000
        RTT.append(rtt_in_ms)

        current_time = now.strftime("%H:%M:%S")
        print("Ping",(i+1),current_time)
        recieved += 1

    except Exception as e:
        print("Request",e)
        lost += 1

    i = i + 1
        

print("\nPing statistics for",address[0],":")

percentage = "{:.0%}".format(lost/10)
print("     Packets: Sent =",10 ,", Recieved =", recieved,", Lost = ",lost,"(",percentage,"lost )")

print("Approximate round trip times in milli-seconds:")
print("     Minimum =",min(RTT),"ms, Maximum =",max(RTT),"ms, Average =",sum(RTT,0.0)/len(RTT),"ms")

UDPClientSocket.close
