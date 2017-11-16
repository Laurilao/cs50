from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    
    # check if user has a portfolio
    check = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"])
    if len(check) >= 1:
    
        # find users portfolio
        if not db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"]):
            return apology("oops")
        portf = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"])
        username = db.execute("SELECT username FROM users WHERE id=:user_id", user_id=session["user_id"])
    
        # query the price of owned stocks and refresh portfolio with current prices in yahoo and update value
        for stock in portf:
            queried = lookup(stock["symbol"])
            if queried:
                db.execute("UPDATE portfolio SET price=:price WHERE user_id=:user_id AND symbol=:symbol", price=queried["price"], user_id=session["user_id"], symbol=queried["symbol"])
                newValue = stock["price"]*float(stock["amount"])
                db.execute("UPDATE portfolio SET value=:newValue WHERE user_id=:user_id AND symbol=:symbol", newValue=format(newValue, ",.2f"), user_id=session["user_id"], symbol=queried["symbol"])
            else:
                return apology("queryfail")
                
        # update portf variable after refreshing prices 
        portf = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"])
        
        # user balance (liquid)
        liquid = db.execute("SELECT cash FROM users WHERE id=:user_id", user_id=session["user_id"])
        balance = format(liquid[0]["cash"], ",.2f")
        
        # total balance (liquid + holdings)
        total = liquid[0]["cash"]
        for stock in portf:
            total += stock["price"]*float(stock["amount"])
        totalvalue = format(total, ",.2f")
    
        return render_template("index.html", portf=portf, username=username[0]["username"].capitalize(), balance=balance, totalvalue=totalvalue)
    
    # user just registered and has no portfolio
    else:
        
        # find username from users
        username = db.execute("SELECT username FROM users WHERE id=:user_id", user_id=session["user_id"])
        
        # user balance
        liquid = db.execute("SELECT cash FROM users WHERE id=:user_id", user_id=session["user_id"])
        balance = format(liquid[0]["cash"], ",.2f")
        
        return render_template("index.html", username=username[0]["username"].capitalize(), balance=balance, totalvalue=balance)
    

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    
    if request.method == 'POST':
        
        # validate input
        if not request.form.get("stock"):
            return apology("Enter valid stock")
        elif not request.form.get("amount"):
            return apology("Invalid amount")
        # check if integer, or try to convert to integer
        try:
            requestAmount = int(request.form.get("amount"))
        except ValueError:
            return apology("enter positive integer")
        # if integer not positive
        if requestAmount < 1:
            return apology("enter positive integer")
            
        
        # return the stocks user requested (if valid)
        # and the total price of requested amount of stocks
        buy = lookup(request.form.get("stock"))
        if not buy:
            return apology("Could not find stocks for {}".format(request.form.get("stock")))
            
        # save variables
        amountRequested = int(request.form.get("amount"))
        stockPrice = buy["price"]
        priceOfStocks = float(amountRequested) * stockPrice
    
        # get users current balance
        cash = db.execute("SELECT cash FROM users WHERE id=:user_id", user_id=session["user_id"])
            
        # check if user can afford the stocks
        if cash[0]["cash"] - priceOfStocks < 0:
            return apology("You cannot afford that many stocks")
            
        ## UPDATE DATABASES ##
        
        # check user portfolio (if stock not found, insert a new entry)
        portf = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"])
        updated = False
        
        # iterate over portfolio to find matching symbol
        for row in portf:
            # if stock found in portfolio, update the amount of shares and value
            if row["symbol"] == buy["symbol"]:
                db.execute("UPDATE portfolio SET amount=:newAmount WHERE symbol=:symbol AND user_id=:user_id", newAmount=row["amount"]+amountRequested, symbol=buy["symbol"], user_id=session["user_id"])
                db.execute("UPDATE portfolio SET value=:newValue WHERE user_id=:user_id AND symbol=:symbol", newValue=format(row["value"]+priceOfStocks, ",.2f"), user_id=session["user_id"], symbol=buy["symbol"])
                updated = True
                
        # if not found in portfolio insert new stock info to portfolio
        if updated == False:
            db.execute("INSERT INTO portfolio (user_id, company, symbol, price, amount, value) VALUES(:user_id, :company, :symbol, :price, :amount, :value)",
            user_id=session["user_id"], company=buy["name"], symbol=buy["symbol"], price=stockPrice, amount=amountRequested, value=format(priceOfStocks, ",.2f"))
            updated = True
                
        if updated == True:
            # transactions update (if portfolio updated successfully)
            db.execute("INSERT INTO transactions (user_id, action, amount, price, company, symbol) VALUES(:user_id, :action, :amount, :price, :company, :symbol)", 
            user_id=session["user_id"], action="BUY", amount=amountRequested, price=stockPrice, company=buy["name"], symbol=buy["symbol"])
            
            # update user balance (if portfolio updated successfully)
            db.execute("UPDATE users SET cash=:deducted WHERE id=:user_id", deducted=cash[0]["cash"]-priceOfStocks, user_id=session["user_id"])

        # generic error
        if updated == False:
            return apology("Something went wrong")
                
        # return to index
        return redirect(url_for("index"))
        
    else:
        return render_template("buy.html")

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    
    if request.method == 'POST':
        
        # validate input fields
        if not request.form.get("stock"):
            return apology("Enter valid stock")
        elif not request.form.get("amount"):
            return apology("Enter amount")
            
        # find current price of stocks
        find = lookup(request.form.get("stock"))
        currPrice = find["price"]
        
        #save variables
        amountToSell = int(request.form.get("amount"))
        stockSymbol = find["symbol"]
        
        # check if user has that stock in his/her portfolio
        portf = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id AND symbol=:symbol", user_id=session["user_id"], symbol=stockSymbol)
        if len(portf) != 1:
            return apology("You do not own any stocks for {}".format(stockSymbol))
        # if yes, does the user have the amount he/she wants to sell
        elif portf[0]["amount"] < amountToSell:
            return apology("You dont have that many stocks to sell")
        
        
        
        ## UPDATE DATABASE ##
        
        # update portfolio
        db.execute("UPDATE portfolio SET amount=:newAmount WHERE user_id=:user_id AND symbol=:symbol", newAmount=portf[0]["amount"]-amountToSell, user_id=session["user_id"], symbol=stockSymbol)
        
        # if user ends up selling all of companys stock, remove from portfolio
        checkAmount = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id AND symbol=:symbol", user_id=session["user_id"], symbol=stockSymbol)
        if checkAmount[0]["amount"] == 0:
            db.execute("DELETE FROM portfolio WHERE user_id=:user_id AND symbol=:symbol", user_id=session["user_id"], symbol=stockSymbol)
        
        # get current balance for updating
        cash = db.execute("SELECT cash FROM users WHERE id=:user_id", user_id=session["user_id"])
        # add balance according to sold stocks
        db.execute("UPDATE users SET cash=:newCash WHERE id=:user_id", newCash=cash[0]["cash"]+currPrice*amountToSell, user_id=session["user_id"])
        
        # transactions update (if portfolio updated successfully)
        db.execute("INSERT INTO transactions (user_id, action, amount, price, company, symbol) VALUES(:user_id, :action, :amount, :price, :company, :symbol)", 
        user_id=session["user_id"], action="SELL", amount=amountToSell, price=currPrice, company=find["name"], symbol=stockSymbol)
        
        return redirect(url_for("index"))
        
    # user arrived via GET    
    else:
        
        # display stocks owned by user
        owned = db.execute("SELECT * FROM portfolio WHERE user_id=:user_id", user_id=session["user_id"])
        
        return render_template("sell.html", owned=owned)

@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    
    history = db.execute("SELECT * FROM transactions WHERE user_id=:user_id", user_id=session["user_id"])
    
    return render_template("history.html", history=history)

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username (stored in lowercase)
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username").lower())

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    
    # is user reached route via POST
    if request.method == 'POST':
        
        # get quote via lookup (in helpers.py)
        quote = lookup(request.form.get("symbol"))
        # check if quote was successful
        if not quote:
            return apology("Could not quote {}".format(request.form.get("symbol")))
        
        # display stockinfo
        return render_template("stockinfo.html", symbol=quote["symbol"], company=quote["name"], price=quote["price"])
    
    # if user reached route via GET
    else:
        return render_template("quote.html")

@app.route("/stockinfo", methods=['GET'])
@login_required
def stockinfo():
    
    return render_template("stockinfo.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user and create template portfolio."""
    
    # if user submits this form
    if request.method == 'POST':
        
        # ensure username was submitted
        if not request.form.get("username"):
            return apology("Missing username!")
        
        # ensure password and confimation were submitted
        elif not request.form.get("password"):
            return apology("Missing password")
        elif not request.form.get("confirmation"):
            return apology("Missing password confirmation!")
            
        # ensure password and confirmation match
        elif request.form.get("password") != request.form.get("confirmation"):
            return apology("Passwords do not match")
            
        # hash the password
        pw_hashed = pwd_context.hash(request.form.get("password"))
        
        # add user to database (converted into lowercase to prevent same usernames with different casings)
        result = db.execute("INSERT INTO users (username, hash) VALUES(:username, :password)", username=request.form.get("username").lower(), password=pw_hashed)
        if not result:
            return apology("User already exists!")
        
        # query database for added username for autologin
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username").lower())
        # autologin the user
        session["user_id"] = rows[0]["id"]
        # enter default balance
        db.execute("UPDATE users SET cash=:formatted WHERE id=:user_id", user_id=session["user_id"], formatted=10000.00)
        
        # redirect user to home page
        return redirect(url_for("index"))
        
    # else if user reached route via GET, display register.html
    else:
        return render_template("register.html")

@app.route("/change_password", methods=["GET", "POST"])
@login_required
def change_password():
    
    if request.method == 'POST':
        
        # check for valid input
        if not request.form.get("password"):
            return apology("enter current password")
        elif not request.form.get("confirmation"):
            return apology("enter current password confirmation")
        elif not request.form.get("new_password"):
            return apology("enter new password")
            
        # make sure password and confirmation match
        if request.form.get("password") != request.form.get("confirmation"):
            return apology("password does not match confirmation")
        # make sure new password is different from old
        if request.form.get("password") == request.form.get("new_password"):
            return apology("new password cannot be the same")
        
            
        # find user table from database
        user = db.execute("SELECT * FROM users WHERE id=:user_id", user_id=session["user_id"])
        
        # ensure username exists and password is correct
        if len(user) == 1 and pwd_context.verify(request.form.get("password"), user[0]["hash"]):
            
            # update db with new password hash
            pwHash_new = pwd_context.hash(request.form.get("new_password"))
            db.execute("UPDATE users SET hash=:newHash WHERE id=:user_id", newHash=pwHash_new, user_id=session["user_id"])
            
            return apology("password changed successfully")
        
        # password is not current users password
        else:    
            return apology("entered password does not match your current password")
            
    else:
        return render_template("change_password.html")