from flask import Flask, render_template

from flask_mysqldb import MySQL

app = Flask(__name__)

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = ""
app.config['MYSQL_DB'] = 'csmicro_db'

mysql = MySQL(app)

@app.route('/', methods = ['GET', 'POST'])
def index():
    
    if request.method == 'POST':
        username = request.form['username']
        email = request.form['email']
        
        cur = mysql.connection.cursor()
        cur.execute('INSERT INTO env_values () VALUES (%s, %s)', (username, email))
        
        mysql.connection.commit()
        
        cur.close()
        
        return "successfully added to the database"
        
    return render_template('index.html')

@app.route('/dataTable')
def table():
    cur = mysql.connection.cursor()
    
    values = cur.execute('SELECT * FROM env_values')
    
    if values > 0:
        env_details = cur.fetchall()
        
        return render_template('dataTable.html',env_details = env_details )

if __name__ == "__main__":
    app.run(debug=True)