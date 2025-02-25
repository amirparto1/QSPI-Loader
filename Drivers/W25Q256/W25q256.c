/**
 ******************************************************************************
 * @file    stm32746g_discovery_qspi.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    25-June-2015
 * @brief   This file includes a standard driver for the N25Q128A QSPI
 *          memory mounted on STM32746G-Discovery board.
 @verbatim
 ==============================================================================
 ##### How to use this driver #####
 ==============================================================================
 [..]
 (#) This driver is used to drive the N25Q128A QSPI external
 memory mounted on STM32746G-Discovery board.

 (#) This driver need a specific component driver (N25Q128A) to be included with.
 (#) Initialization steps:
 (++) Initialize the QPSI external memory using the BSP_QSPI_Init() function. This
 function includes the MSP layer hardware resources initialization and the
 QSPI interface with the external memory.

 (#) QSPI memory operations
 (++) QSPI memory can be accessed with read/write operations once it is
 initialized.
 Read/write operation can be performed with AHB access using the functions
 BSP_QSPI_Read()/BSP_QSPI_Write().
 (++) The function BSP_QSPI_GetInfo() returns the configuration of the QSPI memory.
 (see the QSPI memory data sheet)
 (++) Perform erase block operation using the function BSP_QSPI_Erase_Block() and by
 specifying the block address. You can perform an erase operation of the whole
 chip by calling the function BSP_QSPI_Erase_Chip().
 (++) The function BSP_QSPI_GetStatus() returns the current status of the QSPI memory.
 (see the QSPI memory data sheet)
 @endverbatim
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "w25q256.h"
/** @addtogroup BSP
 * @{
 */

/** @addtogroup STM32746G_DISCOVERY
 * @{
 */

/** @defgroup STM32746G_DISCOVERY_QSPI STM32746G-Discovery QSPI
 * @{
 */

/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32746G_DISCOVERY_QSPI_Private_Variables STM32746G_DISCOVERY QSPI Private Variables
 * @{
 */
//QSPI_HandleTypeDef QSPIHandle;
#define QSPIHandle hqspi
/**
 * @}
 */

/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32746G_DISCOVERY_QSPI_Private_Functions STM32746G_DISCOVERY QSPI Private Functions
 * @{
 */
static uint8_t QSPI_ResetMemory(void);
static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout);
extern QSPI_HandleTypeDef QSPIHandle;
/**
 * @}
 */

/** @defgroup STM32746G_DISCOVERY_QSPI_Exported_Functions STM32746G_DISCOVERY QSPI Exported Functions
 * @{
 */

uint8_t BSP_QSPI_QE(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t value = 0;

//	Get status register for Quad Enable,the Quad IO2 and IO3 pins are enable
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = READ_STATUS_REG2_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//			 Configure the command
	if(HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
//			 Receive the data
	if(HAL_QSPI_Receive(&hqspi, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

//			 automatic polling mode to wait for memory ready
	if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	value |= 0b00000010;
//	value &= 0b11101101;

	/* Enable write operations */
	if(QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

//		 Set status register for Quad Enable,the Quad IO2 and IO3 pins are enable
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = WRITE_STATUS_REG2_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//		 Configure the command
	if(HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
//		 Transmit the data
	if(HAL_QSPI_Transmit(&hqspi, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

//		 automatic polling mode to wait for memory ready
	if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief  Initializes the QSPI interface.
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_Init(void)
{

	/* QSPI memory reset */
	if(QSPI_ResetMemory() != QSPI_OK)
	{
		return QSPI_NOT_SUPPORTED;
	}

/*	if(BSP_QSPI_QE() != QSPI_OK)
	{
		return QSPI_ERROR;
	}*/
	if(BSP_QSPI_Enter4ByteAddrMode() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief  Reads an amount of data from the QSPI memory.
 * @param  pData: Pointer to data to be read
 * @param  ReadAddr: Read start address
 * @param  Size: Size of data to read
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_Read(uint8_t *pData , uint32_t ReadAddr , uint32_t Size)
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the read command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = READ_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_24_BITS;
	s_command.Address = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = Size;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Reception of the data */
	if(HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief  Writes an amount of data to the QSPI memory.
 * @param  pData: Pointer to data to be written
 * @param  WriteAddr: Write start address
 * @param  Size: Size of data to write
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_Write(uint8_t *pData , uint32_t WriteAddr , uint32_t Size)
{
	QSPI_CommandTypeDef s_command;
	uint32_t end_addr, current_size, current_addr;

	/* Calculation of the size between the write address and the end of the page */
	current_addr = 0;

	while(current_addr <= WriteAddr)
	{
		current_addr += W25Q256JW_PAGE_SIZE;
	}
	current_size = current_addr - WriteAddr;

	/* Check if the size of the data is less than the remaining place in the page */
	if(current_size > Size)
	{
		current_size = Size;
	}

	/* Initialize the adress variables */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/* Initialize the program command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = QUAD_INPUT_PAGE_PROG_4Byte_Address_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Perform the write page by page */
	do
	{
		s_command.Address = current_addr;
		s_command.NbData = current_size;

		/* Enable write operations */
		if(QSPI_WriteEnable() != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* Configure the command */
		if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
				!= HAL_OK)
		{
			return QSPI_ERROR;
		}

		/* Transmission of the data */
		if(HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
				!= HAL_OK)
		{
			return QSPI_ERROR;
		}

		/* Configure automatic polling mode to wait for end of program */
		if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* Update the address and size variables for next page programming */
		current_addr += current_size;
		pData += current_size;
		current_size =
				((current_addr + W25Q256JW_PAGE_SIZE) > end_addr) ?
						(end_addr - current_addr) : W25Q256JW_PAGE_SIZE;
	}
	while(current_addr < end_addr);

	return QSPI_OK;
}

/**
 * @brief  Erases the specified Sector of the QSPI memory.
 * @param  BlockAddress: Sector address to erase
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_Erase_Sector(uint32_t EraseStartAddress , uint32_t EraseEndAddress)
{
	QSPI_CommandTypeDef s_command;
	EraseStartAddress = EraseStartAddress - EraseStartAddress % MEMORY_SECTOR_SIZE;
	/* Initialize the erase command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = BLOCK_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	while(EraseEndAddress >= EraseStartAddress)
	{
		s_command.Address = (EraseStartAddress & 0x0FFFFFFF);
		/* Enable write operations */
		if(QSPI_WriteEnable() != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* Send the command */
		if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
				!= HAL_OK)
		{
			return QSPI_ERROR;
		}
		EraseStartAddress += MEMORY_SECTOR_SIZE;

		/* Configure automatic polling mode to wait for end of erase */
		if(QSPI_AutoPollingMemReady(W25Q256JW_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
		{
			return QSPI_ERROR;
		}
	}
	return QSPI_OK;
}

/**
 * @brief  Erases the entire QSPI memory.
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_Erase_Chip(void)
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the erase command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = CHIP_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Enable write operations */
	if(QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* Send the command */
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Configure automatic polling mode to wait for end of erase */
	if(QSPI_AutoPollingMemReady(W25Q256JW_BULK_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief  Reads current status of the QSPI memory.
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_GetStatus(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t reg;

	/* Initialize the read flag status register command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = READ_STATUS_REG1_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Reception of the data */
	if(HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Check the value of the register */
	if((reg & W25Q256JW_FSR_BUSY) != 0)
	{
		return QSPI_BUSY;
	}
	else
	{
		return QSPI_OK;
	}
}

/**
 * @brief  Return the configuration of the QSPI memory.
 * @param  pInfo: pointer on the configuration structure
 * @retval QSPI memory status
 */
uint8_t BSP_QSPI_GetInfo(QSPI_Info *pInfo)
{
	/* Configure the structure with the memory configuration */
	pInfo->FlashSize = W25Q256JW_FLASH_SIZE;
	pInfo->EraseSectorSize = W25Q256JW_SUBSECTOR_SIZE;
	pInfo->EraseSectorsNumber = (W25Q256JW_FLASH_SIZE / W25Q256JW_SUBSECTOR_SIZE);
	pInfo->ProgPageSize = W25Q256JW_PAGE_SIZE;
	pInfo->ProgPagesNumber = (W25Q256JW_FLASH_SIZE / W25Q256JW_PAGE_SIZE);

	return QSPI_OK;
}

/**
 * @brief  This function reset the QSPI memory.
 * @param  hqspi: QSPI handle
 * @retval None
 */
static uint8_t QSPI_ResetMemory()
{
	QSPI_CommandTypeDef s_command;
	if(QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* Initialize the reset enable command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = RESET_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	/* Send the command */
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* Send the reset memory command */
	s_command.Instruction = RESET_MEMORY_CMD;
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Configure automatic polling mode to wait the memory is ready */
	if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief  This function send a Write Enable and wait it is effective.
 * @param  hqspi: QSPI handle
 * @retval None
 */
static uint8_t QSPI_WriteEnable()
{
	QSPI_CommandTypeDef s_command;
	QSPI_AutoPollingTypeDef s_config;

	/* Enable write operations */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = WRITE_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Configure automatic polling mode to wait for write enabling */
	s_config.Match = W25Q256JW_FSR_WREN;
	s_config.Mask = W25Q256JW_FSR_WREN;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction = READ_STATUS_REG1_CMD;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.NbData = 1;

	if(HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief  This function read the SR of the memory and wait the EOP.
 * @param  hqspi: QSPI handle
 * @param  Timeout
 * @retval None
 */
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout)
{
	QSPI_CommandTypeDef s_command;
	QSPI_AutoPollingTypeDef s_config;

	/* Configure automatic polling mode to wait for memory ready */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = READ_STATUS_REG1_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	s_config.Match = 0x00;
	s_config.Mask = W25Q256JW_FSR_BUSY;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	if(HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, Timeout) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

uint8_t BSP_QSPI_MemoryMappedMode(void)
{
	QSPI_CommandTypeDef s_command;
	QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

	/* Configure the command for the read instruction */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = QUAD_OUT_FAST_READ_CMD_4Byte_Address;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 8;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

	/* Configure the memory mapped mode */
	s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	s_mem_mapped_cfg.TimeOutPeriod = 0;

	if(HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

uint8_t BSP_QSPI_Enter4ByteAddrMode(void)
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the read flag status register command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = Address_4Byte_Mode_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;
	s_command.NbData = 0;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
	{
		return QSPI_ERROR;
	}

	if(QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
