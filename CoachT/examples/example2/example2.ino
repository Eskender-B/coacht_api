#include <CoachT.h>

CoachT ccc(Serial);
int count = 0;

void setup()
{
	Serial.begin(9600);
	while(!Serial);

	ccc.begin(0, 16);
	
}

void loop()
{
	
	const int len = 261;

	float vectorX[len];
	float vectorY[len];
	float vectorZ[len];

	Serial.println("\n\nReading swing from sensor until swing is detected ...");
	
	if(ccc.readSwingFromSensor(vectorX, vectorY, vectorZ, len, 3.0f) > 0)
	{
		count++;
		Serial.print("Swing "); Serial.println(count);Serial.println();
		for(int i=0; i<len; i++)
		{
			Serial.print(i+1); Serial.print(". ");
			Serial.print(vectorX[i]);
			Serial.print(',');
			Serial.print(vectorY[i]);
			Serial.print(',');
			Serial.println(vectorZ[i]);
		}
	}
	else
		Serial.println("Error");
}