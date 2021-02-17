#include "SIM8xx.h"

TOKEN(RDY);

SIM8xx::SIM8xx(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin)
{
	_resetPin = resetPin;
	_pwrKeyPin = pwrKeyPin;
	_statusPin = statusPin;

	pinMode(_resetPin, OUTPUT);
	if(_pwrKeyPin != SIM8xx_UNAVAILABLE_PIN) pinMode(_pwrKeyPin, OUTPUT);
	if (_statusPin != SIM8xx_UNAVAILABLE_PIN) pinMode(_statusPin, INPUT);
	
	if(_pwrKeyPin != SIM8xx_UNAVAILABLE_PIN) digitalWrite(_pwrKeyPin, HIGH);
	digitalWrite(_resetPin, HIGH);
}

SIM8xx::~SIM8xx() { }

#pragma region Public functions

void SIM8xx::init()
{
	SIM8xx_PRINT_SIMPLE_P("Init...");

	reset();
	waitForReady();
	delay(1500);

	setEcho(SIM8xxEcho::Off);
}

void SIM8xx::reset()
{
//comentadas todas
	digitalWrite(_resetPin, HIGH);
	delay(10);
	digitalWrite(_resetPin, LOW);
	delay(200);

	digitalWrite(_resetPin, HIGH);
	
	// añadido: https://github.com/blemasle/arduino-sim808/issues/10
	digitalWrite(_pwrKeyPin, HIGH); //pwr off
	delay(2500); //wait discharching capacitors
	digitalWrite(_pwrKeyPin, LOW); //pwr on
	delay(1000); //wait charching capacitors
}

void SIM8xx::waitForReady()
{
//comentado el while ya que en el bk-808 no recibimos nunca RDY
	do
	{
		SIM8xx_PRINT_SIMPLE_P("Waiting for echo...");
		sendAT(S_F(""));
	// Despite official documentation, we can get an "AT" back without a "RDY" first.
	} while (waitResponse(2000) != 0); // por defecto el tiempo de espera era -> TO_F(TOKEN_AT) <- muy corto , lo hemos aumentado tanto aquí como en SIMComAT.h -> constante SIMCOMAT_DEFAULT_TIMEOUT aumentada a 2000

	// we got AT, waiting for RDY
//	while (waitResponse(TO_F(TOKEN_RDY)) != 0);
}

bool SIM8xx::setEcho(SIM8xxEcho mode)
{
	sendAT(S_F("E"), (uint8_t)mode);

	return waitResponse() == 0;
}

size_t SIM8xx::sendCommand(const char *cmd, char *response, size_t responseSize)
{
	flushInput();
	sendAT(cmd);
	
	uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT;
	readNext(response, responseSize, &timeout);
}

#pragma endregion


