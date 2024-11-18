#!/bin/bash

# Ensure the script is executed with superuser privileges
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root (e.g., sudo ./setup.sh)"
  exit 1
fi

# Install necessary libraries
echo "Installing dependencies..."
if [ -x "$(command -v apt-get)" ]; then
  apt-get update && apt-get install -y libx11-dev build-essential
elif [ -x "$(command -v yum)" ]; then
  yum install -y libX11-devel gcc make
elif [ -x "$(command -v pacman)" ]; then
  pacman -Sy --needed libx11 base-devel
else
  echo "Unsupported package manager. Install X11 development libraries manually."
  exit 1
fi

echo "Dependencies installed."

