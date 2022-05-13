#include "pcf8523.h"

/**
 *  \brief pcf8523 initialization function
 *  
 *  \return Return SUCCESS after the initialization
 *  
 *  \details This function initialize the pcf8523 RTC at the start-up.
 */
enum pcf8523_state pcf8523_init(struct pcf8523_dev *dev)
{
    enum pcf8523_state ret = PCF8523_KO;

    if(dev->init != 0x00000000)
    {
        ret = dev->init(PCF8523_ADD);
    }
    else
    {
        ret = PCF8523_OK; //no platform init, assume that chip is ok
    }
    
    if(ret == PCF8523_OK)
    {
        ret = pcf8523_reg_bit_set(dev, PCF8523_Tmr_CLKOUT_ctrl_REG,(7<<PCF8523_COF)); //CLKOUT disable
        if(PCF8523_KO == ret) return PCF8523_KO;
        
        ret = pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_3_REG,(7<<PCF8523_PM)); //auxiliary battery enable
    }
	return ret;
}

/**
 *  \brief RTC get current date function 
 *  
 *  \param [out] date Structure to receive the current date.
 *  \return Return SUCCESS after the operation.
 *  
 *  \details This function ask the RTC for the current date and store it in the variable date.
 */
enum pcf8523_state pcf8523_get_date(struct pcf8523_dev *dev, struct pcf8523_date *date)
{
    enum pcf8523_state ret = PCF8523_KO;
    uint8_t data[7];

    data[0] = PCF8523_SECONDS_REG;

    ret = dev->write(PCF8523_ADD, data, 1);
    if(ret == PCF8523_OK)
    {
        ret = dev->read(PCF8523_ADD, data, 7);
	
        date->second    = (data[0]&0x7F);
        date->minute	= (data[1]&0x7F);
        date->hour		= (data[2]&0x3F);
        date->day		= (data[3]&0x3F);
        date->weekday   = (data[4]&0x07);
        date->month		= (data[5]&0x1F);
        date->year		= data[6];
    }

	return ret;
}

/**
 *  \brief RTC set current date function 
 *  
 *  \param [in] date Structure of the current date to set.
 *  \return Return SUCCESS after setting the date.
 *  
 *  \details This function modify the current date in the RTC according to the one provided.
 */
enum pcf8523_state pcf8523_set_date(struct pcf8523_dev *dev, struct pcf8523_date *date)
{
    enum pcf8523_state ret=PCF8523_KO;
    uint8_t data[8];

    data[0] = PCF8523_SECONDS_REG;
    data[1] = date->second;
    data[2] = date->minute;
    data[3] = date->hour;
    data[4] = date->day;
    data[5] = date->weekday;
    data[6] = date->month;
    data[7] = date->year;

    ret = dev->write(PCF8523_ADD, data, 8);

	return ret;
}

/**
 *  \brief RTC set alarm function 
 *  
 *  \param [in] alarm Structure of the alarm to set.
 *  \return Return SUCCESS after setting the alarm.
 *  
 *  \details This function modify the current alarm in the RTC according to the one provided and activate the alarm.
 */
enum pcf8523_state pcf8523_set_alarm(struct pcf8523_dev *dev, struct pcf8523_date *alarm)
{
    enum pcf8523_state ret = PCF8523_KO;
    uint8_t data[4];

    data[0] = PCF8523_MINUTE_ALARM_REG;
    data[1] = alarm->minute;
    data[2] = alarm->hour;
    data[3] = alarm->day;

    ret = dev->write(PCF8523_ADD, data, 4);

	return ret;
}

/**
 *  \brief RTC get alarm function 
 *  
 *  \param [out] alarm Structure to receive the alarm.
 *  \return Return SUCCESS after the operation.
 *  
 *  \details This function ask the RTC for the current alarm and store it in the variable alarm.
 */
enum pcf8523_state pcf8523_get_alarm(struct pcf8523_dev *dev, struct pcf8523_date *alarm)
{
    enum pcf8523_state ret=PCF8523_KO;
    uint8_t data[3];

    data[0] = PCF8523_MINUTE_ALARM_REG;

    ret = dev->write(PCF8523_ADD, data, 1);

    if(ret == PCF8523_OK)
    {
        ret = dev->read(PCF8523_ADD, data, 3);
        alarm->minute	= data[0];
        alarm->hour		= data[1];
        alarm->day		= data[2];
    }

    return ret;
}

/**
 *  \brief pcf8523 set bit in a register function
 *  
 *  \param [in] reg register to modify
 *  \param [in] bit_msk bit mask to set
 *  \return Return SUCCESS after the operation.
 *  
 *  \details This function set the bit mask bit_msk in the register reg without modifying others bits.
 */
enum pcf8523_state pcf8523_reg_bit_set(struct pcf8523_dev *dev, uint8_t reg, uint8_t bit_msk)
{
    enum pcf8523_state ret=PCF8523_KO;
    uint8_t data[2];

    data[0] = reg;

    ret = dev->write(PCF8523_ADD, data, 1);

    if(ret == PCF8523_OK)
    {
        ret = dev->read(PCF8523_ADD, data+1, 1);

        data[1] |= bit_msk;

        ret |= dev->write(PCF8523_ADD, data, 2);
    }
	
	return ret;
}

/**
 *  \brief pcf8523 reset bit in a register function
 *  
 *  \param [in] reg register to modify
 *  \param [in] bit_msk bit mask to reset
 *  \return Return SUCCESS after the operation.
 *  
 *  \details This function reset the bit mask bit_msk in the register reg without modifying others bits.
 */
enum pcf8523_state pcf8523_reg_bit_reset(struct pcf8523_dev *dev, uint8_t reg, uint8_t bit_msk)
{
    enum pcf8523_state ret = PCF8523_KO;
    uint8_t data[2];

    data[0] = reg;

    ret = dev->write(PCF8523_ADD, data, 1);

    if(ret == PCF8523_OK)
    {
        ret = dev->read(PCF8523_ADD, data+1, 1);

        data[1] &= ~bit_msk;

        ret |= dev->write(PCF8523_ADD, data, 2);
    }
	
	return ret;
}

void pcf8523_handle_interrupt(struct pcf8523_dev *dev)
{
    char tab[20];
    pcf8523_list_register(dev, tab);
 
    if(tab[1]&(1<<PCF8523_WTAF))
    {
        //watchdog interrupt        
    }
    else if(tab[1]&(1<<PCF8523_CTAF))
    {
        //countdown A interrupt
        pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_2_REG, (1<<PCF8523_CTAF));
    }
    else if(tab[1]&(1<<PCF8523_CTBF))
    {
        //countdown B interrupt
        pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_2_REG, (1<<PCF8523_CTBF));
    }
    else if(tab[1]&(1<<PCF8523_SF))
    {
        //second interrupt
        pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_2_REG, (1<<PCF8523_SF));
    }
    else if(tab[1]&(1<<PCF8523_AF))
    {
        //alarm interrupt
        pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_2_REG, (1<<PCF8523_AF));
    }
    else if(tab[2]&(1<<PCF8523_BSF))
    {
        //battery switch interrupt
        pcf8523_reg_bit_reset(dev, PCF8523_CONTROL_3_REG, (1<<PCF8523_BSF));
    }
    else if(tab[2]&(1<<PCF8523_BLF))
    {
        //battery low interrupt
    }
}

enum pcf8523_state pcf8523_list_register(struct pcf8523_dev *dev, char *tab)
{
    enum pcf8523_state ret=PCF8523_KO;
    uint8_t data[2];

    data[0] = PCF8523_CONTROL_1_REG;

    ret = dev->write(PCF8523_ADD, data, 1);

    if(ret == PCF8523_OK)
    {
        ret = dev->read(PCF8523_ADD, tab, 21);
    }

    return ret;
}

enum pcf8523_state pcf8523_soft_reset(struct pcf8523_dev *dev)
{
    enum pcf8523_state ret=PCF8523_KO;
    uint8_t data[2];

    data[0] = PCF8523_CONTROL_1_REG;
    data[1] = 0x58;

    return dev->write(PCF8523_ADD, data, 2);
}