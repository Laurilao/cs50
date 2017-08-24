import sys

def main():
    
    # Check for correct command-line arguments
    if len(sys.argv) != 2:
        print("Enter 1 command-line argument")
        exit(1)
        
    # Save command-line argument as cipherkey casted into int
    key = int(sys.argv[1])
    
    # Ask user for text to be ciphered
    plaintext = input("plaintext: ")
    
    # Create an empty ciphertext string to be filled
    ciphertext = ""
    
    for i in range(len(plaintext)):
        
        if str.isalpha(plaintext[i]):
            
            if str.isupper(plaintext[i]):
                temp = ord(plaintext[i])
                ciphered = ((temp - 65) + key) % 26 + 65
                ciphertext += chr(ciphered)
                    
            
            if str.islower(plaintext[i]):
                temp = ord(plaintext[i])
                ciphered = ((temp - 97) + key) % 26 + 97
                ciphertext += chr(ciphered)
        else:
            ciphertext += plaintext[i]
            
    print("ciphertext: {}".format(ciphertext))    
    exit(0)
    
    
if __name__ == "__main__":
    main()