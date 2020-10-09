#include "SensorReadThread.hpp"

void sensor_read_thread(sense_read::SensorReadThread *self_thread){
    while(1){
        // just calls this function periodically
        self_thread->_internal_thread_func();
    }
}

/**************************************************************************/
/*!
    @brief Does everything required 
*/
/**************************************************************************/
void sense_read::SensorReadThread::begin(void){
    this->init_sensors(); 
    this->init_thread(); 
    
}

/**************************************************************************/
/*!
    @brief Sets up all of our sensors on our Nano 33 BLE board. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::init_sensors(void){

    // Setting up the barometrix pressure sensor. 
    if (!BARO.begin()) {
        Serial.println("Failed to initialize pressure sensor!");
        while (1)
            rtos::ThisThread::sleep_for(1000);
    }
 
    // Setting up our 9dof imu!
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1)
            rtos::ThisThread::sleep_for(1000);
    }

    // Setting up our tempurature and humidity sensor.
    if(!HTS.begin()){
        Serial.println("Failed to initialize our Tempurature and Humidity Sensor");
        while(1)
            rtos::ThisThread::sleep_for(1000);
    }
}

/**************************************************************************/
/*!
    @brief method that allows us to un initialize all of our sensors!
*/
/**************************************************************************/
void sense_read::SensorReadThread::deinit_sensors(void){
    BARO.end();
    IMU.end(); 
    HTS.end();
}

/**************************************************************************/
/*!
    @brief method that starts up our sensor read thread. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::init_thread(void){
    this->thread_handler.start(sensor_read_thread, this);
}

/**************************************************************************/
/*!
    @brief method that allows us to stop and deconstruct our thread.
*/
/**************************************************************************/
void sense_read::SensorReadThread::deinit_thread(void){
    this->thread_handler.terminate();
}

/**************************************************************************/
/*!
    @brief function to be called internally for threading purposes
*/
/**************************************************************************/
void sense_read::SensorReadThread::_internal_thread_func(void){
    
    while(!this->resource_mutx.trylock_for(1000)); 
    
    this->read_pressure_sensor();
    this->read_imu();
    this->read_temp_humd();
    this->resource_mutx.unlock(); 
    
    rtos::ThisThread::sleep_for(this->delay_ms);
}

/**************************************************************************/
/*!
    @brief internal method that reads/filters our pressure data. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::read_pressure_sensor(void){
    this->barometric_pressure = BARO.readPressure();
}

/**************************************************************************/
/*!
    @brief method that reads and filters our imu data. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::read_imu(void){
    if(IMU.accelerationAvailable())
        IMU.readAcceleration(this->accel_data.x, this->accel_data.y, this->accel_data.z);
    if(IMU.gyroscopeAvailable())
        IMU.readGyroscope(this->gyro_data.x, this->gyro_data.y, this->gyro_data.z);
    if(IMU.magneticFieldAvailable())
        IMU.readMagneticField(this->mag_data.x, this->mag_data.y, this->mag_data.z);
}

/**************************************************************************/
/*!
    @brief method that reads and filters our proximity and color data
*/
/**************************************************************************/
void sense_read::SensorReadThread::read_prox_color(void){
    if(APDS.colorAvailable())
        APDS.readColor(this->color_data.r, this->color_data.g, this->color_data.b);
    if(APDS.gestureAvailable())
        this->gesture_prox_data.gesture = APDS.readGesture();
    if(APDS.proximityAvailable())
        this->gesture_prox_data.proximity = APDS.readProximity();
}

/**************************************************************************/
/*!
    @brief method that reads/filters our tempurature and humidity data. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::read_temp_humd(void){
    this->temp_hum_data.tempurature = HTS.readTemperature(FAHRENHEIT);
    this->temp_hum_data.humidity = HTS.readHumidity();
}

/**************************************************************************/
/*!
    @brief get that accelerometer data
*/
/**************************************************************************/
sense_read::AccelData sense_read::SensorReadThread:: get_accel_data(void){
    if(!mutex_taken){
        sense_read::AccelData ac_dat;
        return ac_dat;
    }

    return this->accel_data;
}

/**************************************************************************/
/*!
    @brief get that gyroscope data
*/
/**************************************************************************/
sense_read::GyroData sense_read::SensorReadThread::get_gyro_data(void){
    if(!mutex_taken){
        sense_read::GyroData gyr_dat; 
        return gyr_dat; 
    }
        
    return this->gyro_data;
}

/**************************************************************************/
/*!
    @brief get that magnetometer data
*/
/**************************************************************************/
sense_read::MagData sense_read::SensorReadThread::get_mag_data(void){
    if(!mutex_taken){
        sense_read::MagData ma_dat; 
        return ma_dat; 
    }

    return this->mag_data;
}

/**************************************************************************/
/*!
    @brief get that color data
*/
/**************************************************************************/
sense_read::ColorData sense_read::SensorReadThread::get_color_data(void){
    if(!mutex_taken){
        sense_read::ColorData col_dat;
        return col_dat;
    }
        
    return this->color_data;
}

/**************************************************************************/
/*!
    @brief get that gesture data
*/
/**************************************************************************/
sense_read::GestureProxData sense_read::SensorReadThread::get_gesture_prox_data(void){
    if(!mutex_taken){
        sense_read::GestureProxData ges_dat;
        return ges_dat;
    }
        
    return this->gesture_prox_data;
}

/**************************************************************************/
/*!
    @brief get that tempurature and humidity data. 
*/
/**************************************************************************/
sense_read::TempHumData sense_read::SensorReadThread::get_temp_hum_data(void){
    if(!mutex_taken){
        sense_read::TempHumData te_hd;
        return te_hd;
    }

    return this->temp_hum_data;
}

/**************************************************************************/
/*!
    @brief Call this when you need to get the data from the thread. It pauses the thread so you can have access to everything
*/
/**************************************************************************/
void sense_read::SensorReadThread::acquire(void){
    this->resource_mutx.lock();
    this->mutex_taken = true; 
}

/**************************************************************************/
/*!
    @brief Call this when you are doing getting data from the thread, it resumes the thread to continue getting data. 
*/
/**************************************************************************/
void sense_read::SensorReadThread::release(void){
    this->resource_mutx.unlock();
    this->mutex_taken = false; 
}