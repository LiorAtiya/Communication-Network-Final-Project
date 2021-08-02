from socket import *
import time

print("The server is running\n")

# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(("192.168.111.130", 12000))

# Set the time out(in seconds)
TIME_OUT = 2
serverSocket.settimeout(TIME_OUT)

last_sequence_number = 0

while True:
    try:
        # Receive the client packet along with the address it is coming from
        sequence_number, address = serverSocket.recvfrom(1024)
        sent_time, address = serverSocket.recvfrom(1024)

        # Decode to unicode string 
        sequence_number_string = sequence_number.decode('utf8')
        # Get the num
        sequence_number_converted = int(sequence_number_string)
        # Decode to unicode string 
        sent_time_string = sent_time.decode('utf8')
        # Get the num
        sent_time_converted = float(sent_time_string)

        # Calaulates
        received_time = time.time()
        time_diffrence = (received_time - sent_time_converted) * 1000
        packets_lost = sequence_number_converted - last_sequence_number - 1

        # Update the last sequence number
        last_sequence_number = sequence_number_converted

        # Sends the data back to the client
        serverSocket.sendto(str(time_diffrence).encode('utf8'), address)
        serverSocket.sendto(str(packets_lost).encode('utf8'), address)

    # Checks for a connection timed out
    except Exception as e:
        print("The client is died" ,e)

