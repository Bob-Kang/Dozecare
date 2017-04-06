/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef MPU9250_H
#define MPU9250_H

/*lint ++flb "Enter library region" */

#include <stdbool.h>
#include <stdint.h>
 
#define MPU9250_REG_WHO_AM_I (0x75U)    //!< WHO_AM_I register identifies the device. Expected value is 0x75.
#define AK8963_REG_WHO_AM_I (0x00U)     //!< WHO_AM_I register identifies the device. Expected value is 0x00.
#define ADDRESS_SIGNAL_PATH_RESET (0x68U) //!< 
#define MPU9250_ADDRESS         0x68

#define ADDRESS_SIGNAL_PATH_RESET (0x68U)   //!<   //number 104 register "signal path reset"
#define  SMPLRT_DIV    0X19
#define  CONFIG        0X1A								
#define  PWR_MGMT_1    0X6B
#define  GYRO_CONFIG   0X1B
#define  USER_CONTROL  0X6A
#define  INT_PIN       0X37 
#define  INT_ENABLE    0X38
#define  ACC_CONFIG    0X1C

#define MPU9250_GYRO_OUT        0x43
#define MPU9250_ACC_OUT         0x3B

#define AK8963_MAG_OUT             0X03
#define AK8963_ADDRESS             0x0C<<1
#define AK8963_CTL1                0x0A
#define AK8963_MODE_POWER_DOWN     0X00
#define AK8963_MODE_SINGLE_MEAS    0X01
#define AK8963_MODE_CONTI_MEAS1    0X02
#define AK8963_MODE_CONTI_MEAS2    0x06 //0X03


/** @file
* @brief MPU9250 gyro/accelerometer driver.
*
*
* @defgroup nrf_drivers_mpu6050 MPU6050 gyro/accelerometer driver
* @{
* @ingroup nrf_drivers
* @brief MPU9250 gyro/accelerometer driver.
*/

/**
 * @brief Function for initializing MPU6050 and verifies it's on the bus.
 *
 * @param device_address Device TWI address in bits [6:0].
 * @return
 * @retval true MPU9250 found on the bus and ready for operation.
 * @retval false MPU9250 not found on the bus or communication failure.
 */
bool mpu9250_init(uint8_t device_address,uint8_t device_address2);

/**
  @brief Function for writing a MPU6050 register contents over TWI.
  @param[in]  register_address Register address to start writing to
  @param[in] value Value to write to register
  @retval true Register write succeeded
  @retval false Register write failed
*/
bool mpu9250_register_write(uint8_t register_address, const uint8_t value);

bool ak8963_register_write(uint8_t register_address, const uint8_t value);



/**
  @brief Function for reading MPU6050 register contents over TWI.
  Reads one or more consecutive registers.
  @param[in]  register_address Register address to start reading from
  @param[in]  number_of_bytes Number of bytes to read
  @param[out] destination Pointer to a data buffer where read data will be stored
  @retval true Register read succeeded
  @retval false Register read failed
*/
bool mpu9250_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes);

bool ak8963_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes);

/**
  @brief Function for reading and verifying MPU6050 product ID.
  @retval true Product ID is what was expected
  @retval false Product ID was not what was expected
*/
bool mpu9250_verify_product_id(void);

/**
 *@}
 **/


void MPU9250_ReadAcc( int16_t *pACC_X , int16_t *pACC_Y , int16_t *pACC_Z );
void AK8963_ReadMag( int16_t *pMAG_X , int16_t *pMAG_Y , int16_t *pMAG_Z );
void MPU9250_ReadGyro(int16_t *pGYRO_X , int16_t *pGYRO_Y , int16_t *pGYRO_Z );


/*lint --flb "Leave library region" */

#endif /* MPU6050_H */
