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

#include <stdbool.h>
#include <stdint.h>

#include "twi_master.h"
#include "mpu9250.h"

/*lint ++flb "Enter library region" */

static const uint8_t mpu9250_expected_who_am_i = 0x71U;    //!< Expected value to get from MPU9250 WHO_AM_I register.
static const uint8_t ak8963_expected_who_am_i = 0x48U;     //!< Expected value to get from AK8963  WHO_AM_I register.
static uint8_t mpu9250_device_address;                           //!< Device address in bits [7:1]
static uint8_t ak8963_device_address;

bool mpu9250_init(uint8_t mpu9250_addr,uint8_t ak8963_addr)
{   
  bool transfer_succeeded = true;			
  mpu9250_device_address=(uint8_t)((mpu9250_addr<<1)&0xfe);		//IIC协议规定7BIT DEVICE ADD+W
	ak8963_device_address=(uint8_t)((ak8963_addr<<1)&0xfe);		  //IIC协议规定7BIT DEVICE ADD+W	
	
	
  //Do a reset on signal paths
  uint8_t reset_value = 0x04U | 0x02U | 0x01U; // Resets gyro, accelerometer and temperature sensor signal paths
  transfer_succeeded &= mpu9250_register_write(ADDRESS_SIGNAL_PATH_RESET, reset_value);
  
	mpu9250_register_write(SMPLRT_DIV,0X09);    //设置采样率   Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
	mpu9250_register_write(CONFIG,0X01);        //EXT_SYNC_SET 0 (禁用晶振输入脚) ; default DLPF_CFG = 0 => (低通滤波)ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
	mpu9250_register_write(PWR_MGMT_1 , 0x09);  //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference) 这个地方跟MPU9250不一样
	mpu9250_register_write(GYRO_CONFIG ,0x18);  //量程  0-1000度每秒
	mpu9250_register_write(USER_CONTROL ,0x00);	//I2C_MST_EN  设置成0  默认为0 6050  时能主IIC 	
	
	mpu9250_register_write(INT_PIN,  0X02);	    //推挽输出，高电平中断，一直输出高电平直到中断清除，任何读取操作都清除中断 使能 pass through 功能 

								
	
  mpu9250_register_write(ACC_CONFIG,0x19);    //ACC设置  量程 +-2G s 		
  ak8963_register_write(AK8963_CTL1,AK8963_MODE_SINGLE_MEAS);    //设置AK8963为单次模式

	
  // Read and verify product ID
  transfer_succeeded &= mpu9250_verify_product_id();

  return transfer_succeeded;
}



bool mpu9250_verify_product_id(void)
{
  uint8_t who_am_i;
  if (mpu9250_register_read(MPU9250_REG_WHO_AM_I, &who_am_i, 1))
  {
    if (who_am_i != mpu9250_expected_who_am_i)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

bool mpu9250_register_write(uint8_t register_address, uint8_t value)
{	
	uint8_t w2_data[2];
	
	w2_data[0] = register_address;
	w2_data[1] = value;
  return twi_master_transfer(mpu9250_device_address, w2_data, 2, TWI_ISSUE_STOP);	  
}



bool ak8963_register_write(uint8_t register_address, uint8_t value)
{	
	uint8_t w2_data[2];
	w2_data[0] = register_address;
	w2_data[1] = value;
  return twi_master_transfer(0x0c<<1, w2_data, 2, TWI_ISSUE_STOP);	
}


bool mpu9250_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes)
{
  bool transfer_succeeded;
  transfer_succeeded = twi_master_transfer(mpu9250_device_address, &register_address, 1, TWI_DONT_ISSUE_STOP);
  transfer_succeeded &= twi_master_transfer(mpu9250_device_address|TWI_READ_BIT, destination, number_of_bytes, TWI_ISSUE_STOP);
  return transfer_succeeded;
}


bool ak8963_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes)
{
  bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(0x0c<<1, &register_address, 1, TWI_DONT_ISSUE_STOP);
	transfer_succeeded &= twi_master_transfer((0x0c<<1)|0x001, destination, number_of_bytes, TWI_ISSUE_STOP);
  return transfer_succeeded;
}



void MPU9250_ReadGyro(int16_t *pGYRO_X , int16_t *pGYRO_Y , int16_t *pGYRO_Z )
{
  uint8_t buf[6];    		
  mpu9250_register_read(MPU9250_GYRO_OUT,  buf, 6);
	
  *pGYRO_X = (buf[0] << 8) | buf[1];
	if(*pGYRO_X & 0x8000) *pGYRO_X-=65536;
		
	*pGYRO_Y= (buf[2] << 8) | buf[3];
  if(*pGYRO_Y & 0x8000) *pGYRO_Y-=65536;
	
  *pGYRO_Z = (buf[4] << 8) | buf[5];
	if(*pGYRO_Z & 0x8000) *pGYRO_Z-=65536;
}
			   

void MPU9250_ReadAcc( int16_t *pACC_X , int16_t *pACC_Y , int16_t *pACC_Z )
{
    
	uint8_t buf[6];    		    
	mpu9250_register_read(MPU9250_ACC_OUT, buf, 6);
  *pACC_X = (buf[0] << 8) | buf[1];
	if(*pACC_X & 0x8000) *pACC_X-=65536;
		
	*pACC_Y= (buf[2] << 8) | buf[3];
  if(*pACC_Y & 0x8000) *pACC_Y-=65536;
	
  *pACC_Z = (buf[4] << 8) | buf[5];
	if(*pACC_Z & 0x8000) *pACC_Z-=65536;
}


void AK8963_ReadMag( int16_t *pMAG_X , int16_t *pMAG_Y , int16_t *pMAG_Z )
{
	uint8_t buf[6];    		    
	ak8963_register_read(AK8963_MAG_OUT, buf, 6);
  *pMAG_X=(buf[1] << 8)| buf[0];	
	*pMAG_Y=(buf[3] << 8)| buf[2];
  *pMAG_Z=(buf[5] << 8)| buf[4];
}


/*lint --flb "Leave library region" */ 
