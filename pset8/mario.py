
def main():
    
    # Get user input
    while True:
        height = int(input("Height: "))
        if height < 24 and height > 0:
            break
    
    rownumber = 1  # Keep track of current row, initially 1st
    
    # Iterate over desired height
    for i in range(height):
        
        # Print the pyramid structure
        print(" " * (height - rownumber), end="")
        print("#" * (i + 1), end="")
        printgap()
        print("#" * (i + 1), end="")
        
        # Print a newline
        print()
        
        # Increment row number on each iteration
        rownumber += 1
    
    
def printgap():
    print("  ", end="")
    
if __name__ == "__main__":
    main()