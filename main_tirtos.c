
/*
 * 16 bits TDK IMU MPU-6050
 * ADO pin ='0' i2c address 0x68
 *
 * */

#include <stdint.h>
#include <math.h>

// XDC module Header Files
#include <xdc/std.h>                                                            // XDC "base types" - must be included FIRST
#include <xdc/runtime/Types.h>                                                  // XDC constants/types used in xdc.runtime pkg
#include <xdc/cfg/global.h>                                                     // For all BIOS instances created statically in RTOS .cfg file
#include <xdc/runtime/Error.h>                                                  // For error handling (e.g. Error block)
#include <xdc/runtime/System.h>                                                 // XDC System functions (e.g. System_abort(), System_printf())
#include <xdc/runtime/Log.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Board.h>
#include <ti/drivers/Board.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>                                                // Task APIs
#include <ti/sysbios/knl/Semaphore.h>                                           // Semaphore APIs

/* Driver configuration */
#include "ti_drivers_config.h"

#include <ti/display/Display.h>

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

/* I2C slave addresses */
#define I2C_ADD_MPU6050                 0x68                                       //7 bit address
#define I2C_NUMB_MSG                    14                                         //Max number of bytes in a transaction

/* Registers */
#define SELF_TEST_X_REG                     0x0D
#define SELF_TEST_Y_REG                     0x0E
#define SELF_TEST_Z_REG                     0x0F
#define SELF_TEST_A_REG                     0x10

#define SMPRT_DIV_REG                       0x19                                //Sample Rate Divider Reg address
#define CONFIG_REG                          0x1A                                //2-0 bits Digital Low pass filter config
#define GYRO_CONFIG_REG                     0x1B
#define ACCEL_CONFIG_REG                    0x1C
#define FIFO_EN_REG                         0x23
#define I2C_MST_CTRL_REG                    0x24
#define INT_PIN_CFG_REG                     0x37
#define INT_EN_REG                          0x38
#define INT_STATUS_REG                      0x3A
#define ACCEL_XOUT_H_REG                    0x3B
#define ACCEL_YOUT_H_REG                    0x3D
#define ACCEL_ZOUT_H_REG                    0x3F
#define TEMP_OUT_H_REG                      0x41
#define GYRO_XOUT_H_REG                     0x43
#define GYRO_YOUT_H_REG                     0x45
#define GYRO_ZOUT_H_REG                     0x47
#define SIGNAL_PATH_RESET_REG               0x68
#define USER_CTRL_REG                       0x6A
#define PWR_MGMT_1_REG                      0x6B
#define PWR_MGMT_2_REG                      0x6C
#define FIFO_COUNT_H_REG                    0x72
#define FIFO_R_W                            0x74
#define WHO_AM_I                            0x75

/* Configuration Register values */
#define SMPLRT_DIV_VALUE                   0x00
#define CONFIG_VALUE                       0x06                      //Fs = 1kHz. LPF BW = 5Hz
#define GYRO_CONFIG_VALUE                  0x00                      //+/- 250 degrees
#define ACCEL_CONFIG_VALUE                 0x00                      //+/- 2G
#define FIFO_EN_VALUE                      0x08                      //accelerometer data in FIFO
#define USER_CTRL_FIFO_RST                 0X04
#define PWR_MGMT_1_DEVICE_RST              0X80
#define PWR_MGMT_1_DEVICE_SLEEP            0X40


#define PI                            3.14159265
#define ANGULAR_VEL_250_SCALE         32768

static Display_Handle display;
I2C_Handle      i2c;
I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;
Timer_Handle    timer0_h;
Timer_Params    tparams;

Semaphore_Handle semGetData;

int8_t txBuffer[I2C_NUMB_MSG];
int8_t rxBuffer[I2C_NUMB_MSG];

/* Prototypes */
void *mainThread(void *arg0);
void myTimer_init(void);
void taskGetDataFxn(void);
void timerFxnCB(void);
double getangleAcc (int8_t* numb);
double getangleGyro (int8_t* numb);

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Handle mainTh_h, getData_h;;
    Task_Params taskParams;

    Board_init();

    Task_Params_init(&taskParams);
    taskParams.priority = 3;


    mainTh_h = Task_create((Task_FuncPtr)mainThread, &taskParams, Error_IGNORE);
    if (mainTh_h == NULL) {
       System_abort("Task Get Data create failed");
    }

    taskParams.priority = 2;
    getData_h = Task_create((Task_FuncPtr)taskGetDataFxn, &taskParams, Error_IGNORE);
    if (getData_h == NULL) {
       System_abort("Task Get Data create failed");
    }

    semGetData = Semaphore_create(0, NULL, Error_IGNORE);
    if (semGetData == NULL) {
      System_abort("Semaphore Get Data create failed");
    }

    BIOS_start();

    return (0);
}

void *mainThread(void *arg0)
{
    /* Call driver init functions */
    Display_init();
    GPIO_init();
    I2C_init();
    myTimer_init();

    /* Open the UART display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        while (1);
    }

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(I2C_ACCEL, &i2cParams);
    if (i2c == NULL) {
        Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "I2C Initialized!\n");
    }

    /* Accel Config */
    txBuffer[0] = SMPRT_DIV_REG;
    txBuffer[1] = SMPLRT_DIV_VALUE;
    txBuffer[2] = CONFIG_VALUE;
    txBuffer[3] = GYRO_CONFIG_VALUE;
    txBuffer[4] = ACCEL_CONFIG_VALUE;

    i2cTransaction.slaveAddress = I2C_ADD_MPU6050;
    i2cTransaction.writeBuf   = txBuffer;
    i2cTransaction.writeCount = 5;
    i2cTransaction.readBuf    = rxBuffer;
    i2cTransaction.readCount  = 0;
    I2C_transfer(i2c, &i2cTransaction);

    /*Taking IMU out of Sleep mode*/
    txBuffer[0] = PWR_MGMT_1_REG;
    txBuffer[1] = 0;
    i2cTransaction.writeCount = 2;
    I2C_transfer(i2c, &i2cTransaction);
    return (NULL);
}

void myTimer_init(void){
    Timer_Params_init(&tparams);
    tparams.periodUnits = Timer_PERIOD_HZ;
    tparams.period = 1;
    tparams.timerMode  = Timer_CONTINUOUS_CALLBACK;
    tparams.timerCallback = timerFxnCB;
    timer0_h = Timer_open(CONFIG_TIMER_0, &tparams);
    Timer_start(timer0_h);
}

void timerFxnCB(void){
    GPIO_toggle(CONFIG_GPIO_LED_0);
    Semaphore_post(semGetData);
}


void taskGetDataFxn(void){
    double angleA, angleG;
    while(1){
        Semaphore_pend(semGetData, BIOS_WAIT_FOREVER);
        /*Accel data*/
        txBuffer[0]= ACCEL_XOUT_H_REG;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readCount  = I2C_NUMB_MSG;
        I2C_transfer(i2c, &i2cTransaction);
        angleA = getangleAcc(rxBuffer);
        angleG = getangleGyro(rxBuffer);
        Display_printf(display, 0, 0, "Angle in the Z axle using Accel is %f and using Gyro is %f",angleA, angleG);
    }
}

double getangleAcc (int8_t* numb){
    double ivaluex = (numb[0] << 8) | (0xFF & numb[1]);
    double ivaluey = (numb[2] << 8) | (0xFF & numb[3]);
    double ivaluez = (numb[4] << 8) | (0xFF & numb[5]);
    double accelt = sqrt(ivaluex*ivaluex + ivaluey*ivaluey + ivaluez*ivaluez);
    return (acos(ivaluez/accelt)*180/PI);                                                       //Calculate angle in radians and convert to degrees
}

double getangleGyro (int8_t* numb){
    static double gangle;
    double ivaluex = (double)((numb[8] << 8) | (0xFF & numb[9]))/ANGULAR_VEL_250_SCALE*250;        //Convertion to degrees/sec in the 250 degrees/seconds scale
    double ivaluey = (double)((numb[10] << 8) | (0xFF & numb[11]))/ANGULAR_VEL_250_SCALE*250;
    double ivaluez = (double)((numb[12] << 8) | (0xFF & numb[13]))/ANGULAR_VEL_250_SCALE*250;
    gangle+= ivaluex;                                                                              //Integration. This is just a test. Too much drift present.
    return (gangle);
}

