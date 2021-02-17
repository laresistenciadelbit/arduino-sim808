#include "SIM8xx.h"

AT_COMMAND(SEND_SMS, "+CMGS=\"%s\"");

TOKEN_TEXT(CPIN, "+CPIN");
TOKEN_TEXT(CSQ, "+CSQ");
TOKEN_TEXT(CMGS, "+CMGS");

bool SIM8xx::simUnlock(const char* pin)
{
	sendAT(TO_F(TOKEN_CPIN), TO_F(TOKEN_WRITE), pin);

	return waitResponse(5000L) == 0;
}

size_t SIM8xx::getSimState(char *state, size_t stateSize)
{
	sendAT(TO_F(TOKEN_CPIN), TO_F(TOKEN_READ));
	if(waitResponse(5000L, TO_F(TOKEN_CPIN)) != 0) return 0;

	copyCurrentLine(state, stateSize, strlen_P(TOKEN_CPIN) + 2);

	return waitResponse() == 0 ?
		strlen(state) :
		0;
}
/*
size_t SIM8xx::getImei(char *imei, size_t imeiSize)
{
	//AT+GSN does not have a response prefix, so we need to flush input
	//before sending the command
	flushInput();

	sendAT(S_F("+GSN"));	
	waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, NULL); //consuming an extra line before the response. Undocumented

	if(waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, NULL) != 0) return 0;
	copyCurrentLine(imei, imeiSize);

	return waitResponse() == 0?
		strlen(imei) :
		0;
}

SIM8xxSignalQualityReport SIM8xx::getSignalQuality()
{
	uint8_t quality;
	uint8_t errorRate;

	SIM8xxSignalQualityReport report = {99, 99, 1};

	sendAT(TO_F(TOKEN_CSQ));
	if(waitResponse(TO_F(TOKEN_CSQ)) != 0 ||
		!parseReply(',', (uint8_t)SIM8xxSignalQualityResponse::SignalStrength, &quality) ||
		!parseReply(',', (uint8_t)SIM8xxSignalQualityResponse::BitErrorrate, &errorRate) ||
		waitResponse())
		return report;

	report.rssi = quality;
	report.ber = errorRate;

	if (quality == 0) report.attenuation = -115;
	else if (quality == 1) report.attenuation = -111;
	else if (quality == 31) report.attenuation = -52;
	else if (quality > 31) report.attenuation = 1;
	else report.attenuation = map(quality, 2, 30, -110, -54);

	return report;
}

bool SIM8xx::setSmsMessageFormat(SIM8xxSmsMessageFormat format)
{
	sendAT(S_F("+CMGF="), (uint8_t)format);
	return waitResponse() == 0;
}

bool SIM8xx::sendSms(const char *addr, const char *msg)
{
	if (!setSmsMessageFormat(SIM8xxSmsMessageFormat::Text)) return false;
	sendFormatAT(TO_F(AT_COMMAND_SEND_SMS), addr);

	if (!waitResponse(S_F(">")) == 0) return false;

	SENDARROW;
	print(msg);
	print((char)0x1A);

	return waitResponse(60000L, TO_F(TOKEN_CMGS)) == 0 &&
		waitResponse() == 0;
}
*/