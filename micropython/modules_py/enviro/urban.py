from enviro.base import *

##################################################
# Enviro Urban
##################################################

class EnviroUrban(EnviroBase):
    MICROPHONE_PIN = 26
    SENSOR_RESET_PIN = 9

    PM1_UGM3 = 2
    PM2_5_UGM3 = 3
    PM10_UGM3 = 4
    PM1_UGM3_ATHMOSPHERIC = 5
    PM2_5_UGM3_ATHMOSPHERIC = 6
    PM10_UGM3_ATHMOSPHERIC = 7
    PM0_3_PER_LITRE = 8
    PM0_5_PER_LITRE = 9
    PM1_PER_LITRE = 10
    PM2_5_PER_LITRE = 11
    PM5_PER_LITRE = 12
    PM10_PER_LITRE = 13

    def __init__(self):
        """
            BME280 bme280;

        protected:
            float temperature_c; // temperate in celcius
            float humidity_pct; // humidty as a percentage
            float pressure_hpa; // pressure in hectopascals
            uint16_t noise_vpp; // noise adc channel peak to peak
            uint8_t particulate_reading[32];
        """
        raise NotImplementedError("This function is not yet implemented.")

    def take_reading(self):
        raise NotImplementedError("This function is not yet implemented.")

    def particulates(self, measure):
        raise NotImplementedError("This function is not yet implemented.")

    def noise_level(self):
        raise NotImplementedError("This function is not yet implemented.")

    def noise_level(self, sample_time_ms=1000):
        raise NotImplementedError("This function is not yet implemented.")

    def temperature(self, unit=CELCIUS):
        raise NotImplementedError("This function is not yet implemented.")

    def humidity(self):
        raise NotImplementedError("This function is not yet implemented.")

    def pressure(self, unit=HECTOPASCAL):
        raise NotImplementedError("This function is not yet implemented.")
