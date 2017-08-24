
def main():
    
    while True:
        print("O hai! How much change is owed?")
        cash = float(input())
        if cash > 0:
            break
    
    # Convert dollars to cents and round
    cash *= 100
    cash = round(cash)
    change = int(cash)
    
    
    # Initialize amount of return coins
    coins = 0
    
    while change >= 25:
        coins += 1
        change -= 25
    while change >= 10:
        coins += 1
        change -= 10
    while change >= 5:
        coins += 1
        change -= 5
    while change >= 1:
        coins += 1
        change -= 1
        
    if change < 1:
        print(coins)
    
    
if __name__ == "__main__":
    main()