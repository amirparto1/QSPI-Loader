#include "stm32F7xx_hal.h"
#include "main.h"

#define MEMORY_FLASH_SIZE               0x02000000 /* 256 MBits*/
#define MEMORY_SECTOR_SIZE              0x10000   /* 64kBytes */
#define MEMORY_PAGE_SIZE                0x100     /* 256 bytes */

#define W25Q256JW_FLASH_SIZE                  MEMORY_FLASH_SIZE /* 128 MBits => 32MBytes */
#define W25Q256JW_SECTOR_SIZE                 MEMORY_SECTOR_SIZE   /* 256 sectors of 64KBytes */
#define W25Q256JW_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256JW_PAGE_SIZE                   MEMORY_PAGE_SIZE     /* 65536 pages of 256 bytes */

#define W25Q256JW_DUMMY_CYCLES_READ           4
#define W25Q256JW_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JW_BULK_ERASE_MAX_TIME         250000
#define W25Q256JW_SECTOR_ERASE_MAX_TIME       3000
#define W25Q256JW_SUBSECTOR_ERASE_MAX_TIME    1000

/** 
 * @brief  W25Q256JW Commands
 */
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_OUT_FAST_READ_CMD_4Byte_Address 0x6C
#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define Quad_Fast_Read_INOUT_4Byte_Address   0xEC

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
#define QUAD_INPUT_PAGE_PROG_4Byte_Address_CMD             0x34
#define Address_4Byte_Mode_CMD               0xB7

/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define BLOCK_ERASE_CMD                      0xDC
#define CHIP_ERASE_CMD                       0xC7
#define Block_ERASE_4ByteAdd_CMD             0xDC

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* Flag Status Register */
#define W25Q256JW_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q256JW_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q256JW_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */

/** @addtogroup STM32746G_DISCOVERY_QSPI
 * @{
 */

/* Exported constants --------------------------------------------------------*/
/** @defgroup STM32746G_DISCOVERY_QSPI_Exported_Constants STM32746G_DISCOVERY_QSPI Exported Constants
 * @{
 */
/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)

/* W25Q256JW Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            23     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/**
 * @}
 */

/* Exported types ------------------------------------------------------------*/
/** @defgroup STM32746G_DISCOVERY_QSPI_Exported_Types STM32746G_DISCOVERY_QSPI Exported Types
 * @{
 */
/* QSPI Info */
typedef struct
{
	uint32_t FlashSize; /*!< Size of the flash */
	uint32_t EraseSectorSize; /*!< Size of sectors for the erase operation */
	uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
	uint32_t ProgPageSize; /*!< Size of pages for the program operation */
	uint32_t ProgPagesNumber; /*!< Number of pages for the program operation */
} QSPI_Info;

/**
 * @}
 */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup STM32746G_DISCOVERY_QSPI_Exported_Functions
 * @{
 */
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Read(uint8_t *pData , uint32_t ReadAddr , uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t *pData , uint32_t WriteAddr , uint32_t Size);
uint8_t BSP_QSPI_Erase_Sector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t BSP_QSPI_Erase_Chip(void);
uint8_t BSP_QSPI_GetStatus(void);
uint8_t BSP_QSPI_GetInfo(QSPI_Info *pInfo);
uint8_t BSP_QSPI_MemoryMappedMode(void);
uint8_t BSP_QSPI_Enter4ByteAddrMode(void);
