#include "SIM8xx.h"

TOKEN_TEXT(GPS_POWER, "+CGNSPWR");
TOKEN_TEXT(GPS_INFO, "+CGNSINF");

bool SIM8xx::powerOnOffGps(bool power)
{
	bool currentState;
	if(!getGpsPowerState(&currentState) || (currentState == power)) return false;

	sendAT(TO_F(TOKEN_GPS_POWER), TO_F(TOKEN_WRITE), (uint8_t)power);
	return waitResponse() == 0;
}

bool SIM8xx::getGpsPosition(char *response, size_t responseSize)
{
	sendAT(TO_F(TOKEN_GPS_INFO));

	if(waitResponse(TO_F(TOKEN_GPS_INFO)) != 0)
		return false;

	// GPSINF response might be too long for the reply buffer
	copyCurrentLine(response, responseSize, strlen_P(TOKEN_GPS_INFO) + 2);
}

void SIM8xx::getGpsField(const char* response, SIM8xxGpsField field, char** result) 
{
	char *pTmp = find(response, ',', (uint8_t)field);
	*result = pTmp;
}

bool SIM8xx::getGpsField(const char* response, SIM8xxGpsField field, uint16_t* result)
{
	if (field < SIM8xxGpsField::Speed) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

bool SIM8xx::getGpsField(const char* response, SIM8xxGpsField field, float* result)
{
	if (field != SIM8xxGpsField::Course && 
		field != SIM8xxGpsField::Latitude &&
		field != SIM8xxGpsField::Longitude &&
		field != SIM8xxGpsField::Altitude &&
		field != SIM8xxGpsField::Speed) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

SIM8xxGpsStatus SIM8xx::getGpsStatus(char * response, size_t responseSize, uint8_t minSatellitesForAccurateFix)
{	
	SIM8xxGpsStatus result = SIM8xxGpsStatus::NoFix;

	sendAT(TO_F(TOKEN_GPS_INFO));

	if(waitResponse(TO_F(TOKEN_GPS_INFO)) != 0)
		return SIM8xxGpsStatus::Fail;

	uint16_t shift = strlen_P(TOKEN_GPS_INFO) + 2;

	if(replyBuffer[shift] == '0') result = SIM8xxGpsStatus::Off;
	if(replyBuffer[shift + 2] == '1') // fix acquired
	{
		uint16_t satellitesUsed;
		getGpsField(replyBuffer, SIM8xxGpsField::GnssUsed, &satellitesUsed);

		result = satellitesUsed > minSatellitesForAccurateFix ?
			SIM8xxGpsStatus::AccurateFix :
			SIM8xxGpsStatus::Fix;

		copyCurrentLine(response, responseSize, shift);
	}

	if(waitResponse() != 0) return SIM8xxGpsStatus::Fail;

	return result;
}

bool SIM8xx::getGpsPowerState(bool *state)
{
	uint8_t result;

	sendAT(TO_F(TOKEN_GPS_POWER), TO_F(TOKEN_READ));

	if(waitResponse(10000L, TO_F(TOKEN_GPS_POWER)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}
