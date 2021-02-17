#include "SIM8xx.h"

TOKEN_TEXT(CBC, "+CBC");
TOKEN_TEXT(CFUN, "+CFUN");
TOKEN_TEXT(CCLK, "+CCLK?");

bool SIM8xx::powered()
{
	if(_statusPin == SIM8xx_UNAVAILABLE_PIN) {
		sendAT();
		return waitResponse(SIMCOMAT_DEFAULT_TIMEOUT) != -1;
	}
	
	return digitalRead(_statusPin) == HIGH;
}

bool SIM8xx::powerOnOff(bool power)
{
	if (_pwrKeyPin == SIM8xx_UNAVAILABLE_PIN) return false;

	bool currentlyPowered = powered();
	if (currentlyPowered == power) return false;
	
	SIM8xx_PRINT_P("powerOnOff: %t", power);

	digitalWrite(_pwrKeyPin, LOW);
	delay(2000);
	digitalWrite(_pwrKeyPin, HIGH);

	uint16_t timeout = 2000;
	do {
		delay(150);
		timeout -= 150;
		currentlyPowered = powered();
	} while(currentlyPowered != power && timeout > 0);

	return currentlyPowered == power;
}

void SIM8xx::getInternalClock(char *clock_output)	// tomamos como referencia getImei() o getChargingState()
{
	sendAT(TO_F(TOKEN_CCLK));
	waitResponse(2000,NULL);
	waitResponse(2000,NULL);
	copyCurrentLine(clock_output, 21, 8); //tamaño de cadena del reloj interno = 20 caracteres +1 , 8 de offset iniciales

}

SIM8xxChargingStatus SIM8xx::getChargingState()
{
	uint8_t state;
	uint8_t level;
	uint16_t voltage;

	sendAT(TO_F(TOKEN_CBC));

	if (waitResponse(TO_F(TOKEN_CBC)) == 0 &&
		parseReply(',', (uint8_t)SIM8xxBatteryChargeField::Bcs, &state) &&
		parseReply(',', (uint8_t)SIM8xxBatteryChargeField::Bcl, &level) &&
		parseReply(',', (uint16_t)SIM8xxBatteryChargeField::Voltage, &voltage) &&
		waitResponse() == 0)
		return { (SIM8xxChargingState)state, level, voltage };
			
	return { SIM8xxChargingState::Error, 0, 0 };
}

uint8_t SIM8xx::getBattStat()
{
	uint8_t state;
	uint8_t level;
	uint16_t voltage;

	sendAT(TO_F(TOKEN_CBC));

	if (waitResponse(TO_F(TOKEN_CBC)) == 0 &&
		parseReply(',', (uint8_t)SIM8xxBatteryChargeField::Bcs, &state) &&
		parseReply(',', (uint8_t)SIM8xxBatteryChargeField::Bcl, &level) &&
		parseReply(',', (uint16_t)SIM8xxBatteryChargeField::Voltage, &voltage) &&
		waitResponse() == 0)
		return level;
			
	return 0;
}

SIM8xxPhoneFunctionality SIM8xx::getPhoneFunctionality()
{
	uint8_t state;

	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_READ));

	if (waitResponse(10000L, TO_F(TOKEN_CFUN)) == 0 &&
		parseReply(',', 0, &state) &&
		waitResponse() == 0)
		return (SIM8xxPhoneFunctionality)state;
	
	return SIM8xxPhoneFunctionality::Fail;
}

bool SIM8xx::setPhoneFunctionality(SIM8xxPhoneFunctionality fun)
{
	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_WRITE), (uint8_t)fun);

	return waitResponse(10000L) == 0;
}

bool SIM8xx::setSlowClock(SIM8xxSlowClock mode)
{
	sendAT(S_F("+CSCLK"), TO_F(TOKEN_WRITE), (uint8_t)mode);

	return waitResponse() == 0;
}

