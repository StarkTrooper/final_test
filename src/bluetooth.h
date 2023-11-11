#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_nvic.h"
#include "ble.h"
#include "ble_gap.h"
#include "sensor_driver.h"

#define DEVICE_NAME "mainBoard" 
#define APP_ADV_INTERVAL 64       // Advertising interval (units of 0.625 milliseconds)

// Define UUIDs for custom service and characteristics
#define CUSTOM_SERVICE_UUID 0x1234
#define CUSTOM_CHAR_UUID    0x5678

// Define UUIDs for OTA firmware update service and characteristics
#define OTA_SERVICE_UUID 0x9000
#define OTA_DATA_CHAR_UUID 0x9001
#define OTA_CONTROL_CHAR_UUID 0x9002

#include "sensor_driver.h"  // Include the sensor driver/header file

// Function to read sensor data
// This depends on the sensor, should work with I2C and SPI
// Functions like sensor_init(), sensor_read() depends on the sensor driver or commiunication protocol (They´re hypothetical)
bool read_sensor_data(uint8_t* sensor_data, uint8_t data_length) {
    // Define buffer for the sensor data
    uint8_t sensor_buffer[8];  // Change the size as per the sensor's data length

    // Initialize the sensor (if required)
    sensor_init();

    // Read data from the sensor into the buffer
    bool read_success = sensor_read(sensor_buffer, data_length);

    if (read_success) {
        // Copy the data into the provided sensor_data buffer
        for (int i = 0; i < data_length; i++) {
            sensor_data[i] = sensor_buffer[i];
        }
    } else {
        // Handle sensor read failure
        // You might log an error or take other recovery actions
    }

    // Shutdown or put the sensor to sleep (if required)
    sensor_sleep();

    return read_success;  // Return whether the read was successful
}

// Structure to handle the sensor data characteristic value
uint8_t sensor_data[20] = {0}; // Assuming a maximum of 20 bytes of sensor data
static ble_gatts_char_handles_t sensor_char_handles;

// Function to update the sensor data and send it over BLE
void update_and_send_sensor_data(void) {
    // Read sensor data
    read_sensor_data(sensor_data);

    // Send the sensor data over BLE
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));
    hvx_params.handle = sensor_char_handles.value_handle;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.p_data = sensor_data;
    hvx_params.p_len = sizeof(sensor_data);

    // Send the sensor data as a BLE notification
    sd_ble_gatts_hvx(0, &hvx_params);
}


// Function to handle OTA firmware updates
static void ota_fw_update_handler(ble_evt_t * p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE:
            if (p_ble_evt->evt.gatts_evt.params.write.handle == ota_data_char_handles.value_handle) {
                // Handle incoming firmware update data
                // Store the received data for firmware update
                // ...
            }
            // Implement control characteristics write handling (start, pause, resume, etc.)
            // ...
            break;
        // Add read events and other necessary logic for OTA service
        // ...
    }
}

// Function to start advertising
static void advertising_start(void) {
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
	
	// Advertising data for the sensor service
	// Service data for Sensor 1 and Sensor 2
    uint8_t service_data_sensor1[] = {0x12, 0x34, 0x56}; // Example service data for Sensor 1 replace with the sensor data
    uint8_t service_data_sensor2[] = {0xAB, 0xCD, 0xEF}; // Example service data for Sensor 2 replace with the sensor data

    ble_advdata_t advdata;
    ble_advdata_service_data_t service_data_array[2];
	
    // Sensor 1 service data
    service_data_array[0].service_uuid = SENSOR_SERVICE_UUID_1;  // Define sensor 1 service UUID
    service_data_array[0].data.size = sizeof(service_data_sensor1);
    service_data_array[0].data.p_data = service_data_sensor1;

    // Sensor 2 service data
    service_data_array[1].service_uuid = SENSOR_SERVICE_UUID_2;  // Define sensor 2 service UUID
    service_data_array[1].data.size = sizeof(service_data_sensor2);
    service_data_array[1].data.p_data = service_data_sensor2;

    // Set up advertising data with the sensor service
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.p_service_data_array = &service_data_array;
    advdata.service_data_count = 2; //set the count of service data items


    // Set advertising parameters
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp = BLE_GAP_ADV_FP_ANY;
    adv_params.interval = APP_ADV_INTERVAL;
    adv_params.timeout = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;
	
	// Set the advertising data
    err_code = sd_ble_gap_adv_data_set(NULL, &advdata, NULL);
    if (err_code != NRF_SUCCESS) {
        // Handle error setting advertising data
        NRF_LOG_ERROR("Error setting advertising data. Error code: %d\n", err_code);
        // Implement error handling
    }

// Start advertising
	err_code = sd_ble_gap_adv_start(&adv_params);
	if (err_code != NRF_SUCCESS) {
		if (err_code == NRF_ERROR_INVALID_STATE) {
			// Advertising is already in progress, or there are pending flash operations.
			// Consider stopping advertising and then starting it again.
			sd_ble_gap_adv_stop();  // Stop the advertising
			err_code = sd_ble_gap_adv_start(&adv_params);  // Retry starting advertising
			if (err_code != NRF_SUCCESS) {
				// Handle error after retry
				NRF_LOG_ERROR("Error: Failed to start advertising after retry. Error code: %d\n", err_code);
				// Implement other actions if needed
				// For example: Attempt a different operation or restart the device
			}
		} else if (err_code == NRF_ERROR_RESOURCES) {
			// Not enough resources to start advertising.
			// Consider stopping some non-critical operations to free up resources.
			// Add resource management or alternative actions
			NRF_LOG_ERROR("Error: Insufficient resources to start advertising. Error code: %d\n", err_code);
			// Implement freeing resources or try the operation again after some delay
		} else {
			// Handle other errors
			NRF_LOG_ERROR("Error: Unhandled error occurred. Error code: %d\n", err_code);
			// Implement the necessary actions based on the specific error
		}
	}

}

// Function to initialize the OTA firmware update service
static void ota_fw_service_init(void) {
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize a service structure for OTA service
    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;
    ble_uuid.uuid = OTA_SERVICE_UUID;

    // Add the OTA service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize a characteristic structure for OTA data
    ble_uuid.uuid = OTA_DATA_CHAR_UUID;

    // Define metadata and properties for OTA data characteristic
    // ...

    // Add the OTA data characteristic
    // ...

    // Initialize a characteristic structure for OTA control
    ble_uuid.uuid = OTA_CONTROL_CHAR_UUID;

    // Define metadata and properties for OTA control characteristic
    // ...

    // Add the OTA control characteristic
    // ...
}

// Function to start advertising OTA service
static void ota_fw_advertising_start(void) {
	advertising_start(); // Call the advertising function directly
}

// Structure to handle the custom characteristic value
uint8_t custom_char_value = 0;

// Define the handles for the custom characteristics
static ble_gatts_char_handles_t custom_char_handles;

// Function to initialize the custom service and characteristics
static void services_init(void) {
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize a service structure
    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;
    ble_uuid.uuid = CUSTOM_SERVICE_UUID;

    // Add the custom service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize a characteristic structure
    ble_uuid.uuid = CUSTOM_CHAR_UUID;

	// Define the characteristic metadata
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;  // Characteristic is readable
    char_md.p_char_user_desc = NULL;
    char_md.init_len = sizeof(uint8_t);
    char_md.init_offs = 0;

    // Define attribute metadata
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    // Set characteristic presentation format metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_USER;

    // Define attribute value
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t);

    // Add the characteristic to the custom service
    err_code = sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID, &char_md, &attr_char_value, &custom_char_handles);
    APP_ERROR_CHECK(err_code);
}

// Structure for handling BLE events
static void ble_evt_handler(ble_evt_t * p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            // Handle connected event
			NRF_LOG_INFO("Connected\r\n");
			ble_gap_sec_params_t sec_param;
			memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));
			sec_param.bond = SEC_PARAM_BOND;
			sec_param.mitm = SEC_PARAM_MITM;
			sec_param.lesc = SEC_PARAM_LESC;
			sec_param.keypress = SEC_PARAM_KEYPRESS;
			sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
			sec_param.oob = SEC_PARAM_OOB;
			sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
			sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
			err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, 
												BLE_GAP_SEC_STATUS_SUCCESS, &sec_param);
			APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            // Handle disconnected event
			NRF_LOG_INFO("Disconnected\r\n");
			// Manage a state to allow application to know if a central is connected
			// Manage any required cleanup based on the application requirements
            break;
			
		 case BLE_GATTS_EVT_WRITE:
            if (p_ble_evt->evt.gatts_evt.params.write.handle == custom_char_handles.value_handle) {
                // Data written to the custom characteristic
                // Update custom_char_value with the received data
                custom_char_value = p_ble_evt->evt.gatts_evt.params.write.data[0];
                // Process the received data from the characteristic
            }
			break;
			
		case BLE_GATTS_EVT_READ:
            if (p_ble_evt->evt.gatts_evt.params.read.handle == custom_char_handles.value_handle) {
                // Data read from the custom characteristic
                // Provide data to be read by the central device
                // You can update the response data in p_ble_evt->evt.gatts_evt.params.read.len and p_ble_evt->evt.gatts_evt.params.read.offset
            }
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Handle security parameters request event
			NRF_LOG_INFO("Security parameter request\r\n");
			err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle,
                                         BLE_GAP_SEC_STATUS_SUCCESS,
                                         &sec_param);
			APP_ERROR_CHECK(err_code);
            break;

        // Add other event cases as needed
    }
}

// Function to initialize the sensor service and characteristics
static void sensor_service_init(void) {
    // Initialize a service structure for sensor service
    // Define a new UUID for the sensor service

    // Add the sensor service
    // Add characteristics for sensor data

    /* // Example of adding a sensor data characteristic
    ble_uuid_t sensor_uuid;
    ble_gatts_char_md_t char_md;
	*/
	
    // Initialize characteristic metadata
    // Set properties for the sensor data characteristic (e.g., notify)
    // ...

    // Define attribute metadata for the sensor data characteristic
    // ...

    // Add the sensor data characteristic
    // ...

    // Assign handles for sensor characteristics
    // Update sensor_char_handles with the respective characteristic handles
    // ...
}


int main(void) {
    // Initialize the SoftDevice
    uint32_t err_code = sd_softdevice_enable(NRF_SD_BLE_API_VERSION, NULL, NULL);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("Error: Failed to initialize SoftDevice. Error code: %d\n", err_code);
        // Implement error recovery or specific actions
        // For example, restart the initialization or shut down the device
    }

    // Enable BLE stack
    err_code = sd_ble_enable(NULL);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("Error: Failed to enable BLE stack. Error code: %d\n", err_code);
        // Implement the necessary error recovery steps
    }

    // Set the device name
    err_code = sd_ble_gap_device_name_set(NULL, (uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("Error: Failed to set device name. Error code: %d\n", err_code);
        // Implement the required error handling actions
    }
	
    // Initialize OTA firmware update service
    ota_fw_service_init();

    // Start advertising the OTA service
    ota_fw_advertising_start();

	 // Initialize services and characteristics
    services_init();
	
	 // Initialize sensor service and characteristics
    sensor_service_init();

    // Start advertising
    advertising_start();

    while (1) {
        // Main application loop
        // Application logic here?
		// Read and send sensor data over BLE
        update_and_send_sensor_data();

        // Sleep or do other tasks as needed
        __WFE();
    }
}
