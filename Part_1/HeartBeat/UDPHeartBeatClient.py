import socket
import time

print("The client is running\n")

serverAddressPort = ("192.168.111.130", 12000)
bufferSize = 1024

# Notice the use of SOCK_DGRAM for UDP packets
UDPClientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Initialize the sequence number to start from one
sequence_number = 1

while True:
    # The time that the packet is sending to the server 
    timestamp = time.time()

    # Convert 
    timestamp_to_send = str(timestamp).encode('utf8')
    sequence_number_to_send = str(sequence_number).encode('utf8')

    # Send to server using created UDP socket
    UDPClientSocket.sendto(sequence_number_to_send, serverAddressPort)
    UDPClientSocket.sendto(timestamp_to_send, serverAddressPort)

    # Get feedback from the server
    time_diffrence, address  = UDPClientSocket.recvfrom(bufferSize)
    packets_lost, address  = UDPClientSocket.recvfrom(bufferSize)

    # Decode to unicode string 
    time_diffrence_string = time_diffrence.decode('utf8')
    # Get the num
    time_diffrence_converted = float(time_diffrence_string)
    # Decode to unicode string 
    packets_lost_string = packets_lost.decode('utf8')
    # Get the num
    packets_lost_converted = int(packets_lost_string)

    # Print the data the received from the server
    print("The time that took the packet to arrive is: " + "{:.3f}".format(time_diffrence_converted) + " ms")
    print("The number of packets that have been lost is :", packets_lost_string + "\n")

    # Increament the sequence number by one
    sequence_number = sequence_number + 1

    # Go to sleep(in seconds)
    time.sleep(1)
