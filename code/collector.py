from flask import Flask, render_template
import time,datetime

app = Flask("app")

#  structure of a record
# ts    id  v1  v2  v3  
# 1234561234123412341234
db = []
start = time.time()
stamp = datetime.datetime.now().isoformat()

with open("static/db_file.csv","w") as file:
    file.write("# ")
    file.write(stamp)
    file.write('\n')

@app.get("/collector/get_data")
def get_data():
    msg = "<h1> Collection server</h1>\n"
    msg += "<p>" + stamp + "</p>";
    for txt in db[-10:]:
        msg += txt+"<br>"
    return msg


@app.route("/collector")
def index(): return render_template('index.html')

@app.route("/collector/record/<value>")
def add_record(value):
    rec = str(int(time.time() - start)) + ',' + value.replace(':',',')
    db.append(rec)
    with open("static/db_file.csv","a") as file:
        file.write(rec)
        file.write("\n")
    return str(len(db)), 201    

