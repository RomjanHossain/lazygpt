#!/bin/bash

# Function to check if the system is Debian-based
is_debian_based() {
    [[ -f /etc/debian_version ]]
}

# Function to install dependencies on Debian-based systems
install_dependencies() {
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev libjsoncpp-dev
}

# Function to install APE
install_ape() {
    sudo wget -O /usr/bin/ape https://cosmo.zip/pub/cosmos/bin/ape-$(uname -m).elf
    sudo chmod +x /usr/bin/ape
}

# Function to register APE as a binfmt
register_ape() {
    echo ':APE:M::MZqFpD::/usr/bin/ape:' | sudo tee /proc/sys/fs/binfmt_misc/register
    echo ':APE-jart:M::jartsr::/usr/bin/ape:' | sudo tee /proc/sys/fs/binfmt_misc/register
}

# Function to prompt the user to choose a model and run it
choose_and_run_model() {
    local models_dir="../models"
    cd "$models_dir" || { echo "Models directory not found. Exiting."; exit 1; }

    echo "Choose a model to run:"
    echo "1. Mistral 7B"
    echo "2. Tiny Llama 1.1B"
    read -p "Enter the number of the model you want to run (1 or 2): " model_choice

    case $model_choice in
        1) model_file="mistral-7b-instruct-v0.2.Q4_K_M.llamafile" ;;
        2) model_file="TinyLlama-1.1B-Chat-v1.0.Q5_K_M.llamafile" ;;
        *) echo "Invalid choice. Exiting."; exit 1 ;;
    esac

    chmod +x "$model_file"
    ./"$model_file" --server --nobrowser
}

# Main script execution
if is_debian_based; then
    echo "System is Debian-based. Installing dependencies..."
    install_dependencies
else
    echo "System is not Debian-based. Please install the necessary dependencies manually."
    # exit 1
fi

install_ape
register_ape
choose_and_run_model
