#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    clawPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  driveEncL,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  driveEncR,      sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  armBump,        sensorDigitalIn)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           A4,            tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           A1,            tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port3,           Claw1,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           R1,            tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           L2YL3,         tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           R2YR3,         tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           L1,            tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           A2,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           A3,            tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          Claw2,         tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* DEFINE CONSTANTS */
int dumpHeight = 1000; //Defines the height of the arm where the claw is opened.
int armLimit = 1300; //Defines the top of the arms range of motion.
bool armBumpVal; //Defines if the arm is at the bottom of its range of motion.

/* GLOBAL INIT */
#define MAX_VOLTAGE 127 //Maximum voltage to be applied to a motor.
#define MIN_VOLTAGE (-127) //Minimum voltage to be applied to a motor.
#define ARM_CLOSE_VAL 2300 //Defines the potentiometer value of where the claw is concidered "closed."
#define ARM_OPEN_VAL 1920 //Defines the potentiometer value of where the claw is concidered "open."

/*GLOBAL FUNCTIONS */

//Moves the arm at the given voltage.
void setArm(int voltage){
	motor(A1) = motor(A2) = motor(A3) = motor(A4) = voltage;
}

//Moves the claw at the given voltage.
void setClaw(int voltage){
	motor(Claw1) = motor(Claw2) = voltage;
}

//Drives the left drive at the given voltage.
void leftDrive(int voltage){
	motor(L1) = motor(L2YL3) = voltage;
}

//Drives the right drive at the given voltage.
void rightDrive(int voltage){
	motor(R1) = motor(R2YR3) = voltage;
}


// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the cortex has been powered on and    */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks
	// running between Autonomous and Driver controlled modes. You will need to
	// manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// Set bDisplayCompetitionStatusOnLcd to false if you don't want the LCD
	// used by the competition include file, for example, you might want
	// to display your team name on the LCD in this function.
	// bDisplayCompetitionStatusOnLcd = false;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  								          Claw PID Control Task                            */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

// These could be constants but leaving
// as variables allows them to be modified in the debugger "live"
float  pid_Kp = 1.0;
float  pid_Kd = 0.5;
float  deltaD = 20;

static bool pidRunning = true;
float pidRequestedValue;
float clawPIDOutput;

task pidController()
{
	float  pidSensorCurrentValue;

	float  pidError;
	float  pidLastError;
	float  pidIntegral;
	float  pidDerivative;
	float  pidDrive;

	// Init the variables - thanks Glenn :)
	pidLastError  = 0;

	while(true)
	{

		// Read the sensor value and scale
		pidSensorCurrentValue = SensorValue[clawPot];

		// calculate error
		pidError = (pidSensorCurrentValue - pidRequestedValue)/10;

		// calculate the derivative
		pidDerivative = pidError - pidLastError;
		pidLastError  = pidError;

		// Is PID control active ?
		if(pidRunning)
		{
			// calculate drive
			pidDrive = (pid_Kp * pidError) + (pid_Kd * pidDerivative);

			// limit drive
			if(pidDrive > MAX_VOLTAGE)
				pidDrive = MAX_VOLTAGE;
			if(pidDrive < MIN_VOLTAGE)
				pidDrive = MIN_VOLTAGE;

			// send to motor
			motor(Claw1) = motor(Claw2) = pidDrive ;
		}

		// Run at 50Hz
		wait1Msec(25);
	}
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

task autonomous()
{
	// ..........................................................................
	// Insert user code here.
	// ..........................................................................

	// Remove this function call once you have "real" code.
	AutonomousCodePlaceholderForTesting();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

//ARCADE DRIVE SYSTEM
void arcadeDrive(){
	leftDrive(vexRT(Ch3) + vexRT(Ch1));
	rightDrive(vexRT(Ch3) - vexRT(Ch1));
}

//TANK DRIVE SYSTEM
void tankDrive(){
	leftDrive(vexRT(Ch3));
	rightDrive(vexRT(Ch2));
}

//CLAW CONTROL
void userClaw(){
	if(vexRT(Btn5U)){ //If arm close button is pushed close the arm at full voltage.
		setClaw(MIN_VOLTAGE);
		pidRunning = false;
	}
	else if(vexRT(Btn6U)){ //If arm open button is pushed open the arm at full voltage.
		setClaw(MAX_VOLTAGE);
		pidRunning = false;
	}
	else if(SensorValue(clawPot) >= ARM_CLOSE_VAL && !pidRunning){ //If arm is not controlled, but closed past close point give a gipping voltage.
		setClaw(-15);
	}
	else if(!pidRunning){ //Else, claw will not power.
		setClaw(0);
	}
}

//ARM CONTROL SYSTEM
void userArm(){
	if (vexRT(Btn6D) && SensorValue(I2C_1) < dumpHeight){ //Arm moving up
		setArm(MAX_VOLTAGE); //Full upward voltage.
	}
	else if(vexRT(Btn6D) && SensorValue(I2C_1) < armLimit){ //Arm moving up
		setArm(90); //Upward voltage.
		pidRunning = true;
		pidRequestedValue = ARM_OPEN_VAL; //Open claw
	}
	else if(SensorValue(armBump) == 0){ //Arm at bottom
		setArm(-10); //Hold voltage applied when claw is at the base.
		SensorValue(I2C_1) = 0; //Sets the encoder on the arm to 0.
	}
	else{ //Arm moving down
		setArm(-50); //Downward voltage.
	}
}


/* USER CONTROL CODE */
task usercontrol()
{
	//Init PID task:
	startTask(pidController);
	pidRunning = true;
	pidRequestedValue = ARM_OPEN_VAL;

	//Loop through control systems.
	while (true)
	{
		//Sets the value of the constant depending on the bump sensor's value.
		armBumpVal = SensorValue(armBump) == 0;

		//User control drive program:
		tankDrive();
		//arcadeDrive();

		//User control claw program:
		userClaw();

		//User control arm program:
		userArm();
	}
}
