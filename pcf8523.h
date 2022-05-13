#ifndef PCF8523_H
#define PCF8523_H

#include <stdint.h>

/**
 *  \struct pcf8523_date
 *  
 *  \brief Structure used to store and set a date in the RTC.
 */
struct pcf8523_date {
	char year;   /*!< the last 2 digits of the year (i.e. 17 for 2017)*/
    char month;  /*!< between 1 and 12*/
    char weekday;/*!< between 0 and 6 */
    char day;    /*!< between 1 and 31*/
    char hour;   /*!< between 0 and 23*/
    char minute; /*!< between 0 and 59*/
    char second; /*!< between 0 and 59*/
	};

//REGISTER MAP
#define PCF8523_CONTROL_1_REG		0x00
#define PCF8523_CAP_SEL				7
#define PCF8523_T					6
#define PCF8523_STOP				5
#define PCF8523_SR					4
#define PCF8523__12_24				3
#define PCF8523_SIE					2
#define PCF8523_AIE					1
#define PCF8523_CIE					0

#define PCF8523_CONTROL_2_REG		0x01
#define PCF8523_WTAF				7
#define PCF8523_CTAF				6
#define PCF8523_CTBF				5
#define PCF8523_SF					4
#define PCF8523_AF					3
#define PCF8523_WTAIE				2
#define PCF8523_CTAIE				1
#define PCF8523_CTBIE				0

#define PCF8523_CONTROL_3_REG		0x02
#define PCF8523_PM					5
#define PCF8523_BSF					3
#define PCF8523_BLF					2
#define PCF8523_BSIE				1
#define PCF8523_BLIE				0

#define PCF8523_SECONDS_REG			0x03
#define PCF8523_OS					7

#define PCF8523_MINUTES_REG			0x04

#define PCF8523_HOURS_REG			0x05
#define PCF8523_AMPM				5

#define PCF8523_DAYS_REG			0x06

#define PCF8523_WEEKDAYS_REG		0x07

#define PCF8523_MONTHS_REG			0x08

#define PCF8523_YEARS_REG			0x09

#define PCF8523_MINUTE_ALARM_REG	0x0A
#define PCF8523_AEN_M				7

#define PCF8523_HOUR_ALARM_REG		0x0B
#define PCF8523_AEN_H				7

#define PCF8523_DAY_ALARM_REG		0x0C
#define PCF8523_AEN_D				7

#define PCF8523_WEEKDAY_ALARM		0x0D
#define PCF8523_AEN_W				7

#define PCF8523_OFFSET_REG			0x0E
#define PCF8523_MODE				7

#define PCF8523_Tmr_CLKOUT_ctrl_REG 0x0F
#define PCF8523_TAM					7
#define PCF8523_TBM					6
#define PCF8523_COF					3
#define PCF8523_TAC					1
#define PCF8523_TBC					0

#define PCF8523_Tmr_A_freq_ctrl_REG 0x10
#define PCF8523_TAQ					0

#define PCF8523_Tmr_A_REG			0x11

#define PCF8523_Tmr_B_freq_ctrl_REG 0x12
#define PCF8523_TBW					4
#define PCF8523_TBQ					0

#define PCF8523_Tmr_B_REG			0x13

enum pcf8523_state {
    PCF8523_OK,
    PCF8523_KO,
};

typedef enum pcf8523_state (*pcf8523_init_fptr)(uint8_t addr);
typedef enum pcf8523_state (*pcf8523_read_fptr)(uint8_t addr, uint8_t *data, uint8_t length);
typedef enum pcf8523_state (*pcf8523_write_fptr)(uint8_t addr, uint8_t *data, uint8_t length);

#define PCF8523_ADD (0x68<<1)

struct pcf8523_dev {
    pcf8523_init_fptr init;
    pcf8523_read_fptr read;
    pcf8523_write_fptr write;
};

enum pcf8523_state pcf8523_init(struct pcf8523_dev *dev);
enum pcf8523_state pcf8523_get_date(struct pcf8523_dev *dev, struct pcf8523_date *date);
enum pcf8523_state pcf8523_set_date(struct pcf8523_dev *dev, struct pcf8523_date *date);
enum pcf8523_state pcf8523_set_alarm(struct pcf8523_dev *dev, struct pcf8523_date *alarm);
enum pcf8523_state pcf8523_get_alarm(struct pcf8523_dev *dev, struct pcf8523_date *alarm);
enum pcf8523_state pcf8523_reg_bit_set(struct pcf8523_dev *dev, uint8_t reg, uint8_t bit_msk);
enum pcf8523_state pcf8523_reg_bit_reset(struct pcf8523_dev *dev, uint8_t reg, uint8_t bit_msk);
void pcf8523_handle_interrupt(struct pcf8523_dev *dev);
enum pcf8523_state pcf8523_list_register(struct pcf8523_dev *dev, char *tab);
enum pcf8523_state pcf8523_soft_reset(struct pcf8523_dev *dev);

#endif /* PCF8523_H */
