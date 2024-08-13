#!/bin/bash

# Path to your input file
input_file="servers.txt"

# Initialize an array to hold client commands
client_commands=()

# Read the file line by line
while IFS= read -r line; do
  if [[ $line =~ ^[0-9]+$ ]]; then
    # If the line is a number, it's the number of clients
    num_clients=$line
  elif [[ $line =~ ^([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+):([0-9]+)$ ]]; then
    # If the line matches IP:port, add to client commands array
    client_commands+=("$line")
  fi
done < "$input_file"

# Run the server using the last port from the client commands array
server_port="${client_commands[-1]##*:}"
run-servidor "$server_port"

# Run the client commands for the specified number of clients
for ((i=0; i<num_clients; i++)); do
  address="${client_commands[$i]%%:*}"
  port="${client_commands[$i]##*:}"
  run-cliente "$address" "$port"
done
