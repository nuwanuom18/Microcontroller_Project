from flask import Flask, request , render_template
import datetime
import time
from flask_mysqldb import MySQL
import sys
from extractCAP import extract_cap_

app = Flask(__name__)
'''
Name: N.M. Abeynayake
180003L
'''

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = "nuwansql"
app.config['MYSQL_DB'] = 'csmicro_db'

mysql = MySQL(app)

@app.route('/add', methods = ['GET', 'POST'])
def index():
    if request.method == 'POST':
        
        request_data = request.data
        
        extracted_values_dict = extract_cap_(request_data)
        
        
        # value init
        timestamp = extracted_values_dict['timestamp']
        
        # mean values
        temp = extracted_values_dict['temp']
        pressure = extracted_values_dict['pressure']
        humidity = extracted_values_dict['humidity']
        w_speed = extracted_values_dict['w_speed']
        l_level = extracted_values_dict['l_level']
        
        temp_std = extracted_values_dict['temp_std']
        pressure_std = extracted_values_dict['pressure_std']
        humidity_std = extracted_values_dict['humidity_std']
        w_speed_std = extracted_values_dict['w_speed_std']
        l_level_std = extracted_values_dict['l_level_std']
        
        cur = mysql.connection.cursor()
        
        try:
            # all values in the database are varchar
            cur.execute('''INSERT into env_values (timestamp, temp, pressure, humidity,w_speed, l_level, temp_std, pressure_std, humidity_std, w_speed_std, l_level_std) VALUES (%s, %s,  %s, %s, %s,  %s, %s,  %s, %s, %s,  %s)''', (timestamp, temp, pressure, humidity,w_speed, l_level, temp_std, pressure_std, humidity_std, w_speed_std, l_level_std ))
            
            mysql.connection.commit()
            
            cur.close()
            
            return "successfully added to the database"
        
        except:
            
            return "error when inserting data to the database"
        
    return 

# to show the values in the webpage (dashboard ) defalut url : http://localhost:5000/dataTable

@app.route('/dataTable')
def table():
    cursor = mysql.connection.cursor()
    
    values = cursor.execute('SELECT * FROM env_values')
    
    if values > 0:
        env_details = cursor.fetchall()
        print(values)
        return render_template('dataTable.html',env_details = env_details )
    
if __name__ == "__main__":
    app.run(debug=True)