#include <CoachT.h>

CoachT ccc(Serial);


void setup()
{
	Serial.begin(9600);
	while(!Serial);

	ccc.begin(1, 16);

	const int len = 261;

	float vectorX[len];
	float vectorY[len];
	float vectorZ[len];

	int count = 0;

	int ret = 0;
	if(!ccc.openFile("COACHT/DATA/FOREHAS.CSV"))
		return;
	while((ret=ccc.readSwingFromFile(vectorX, vectorY, vectorZ, len, 3.0f))>0)
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
	Serial.print("Found ");Serial.print(count);Serial.println(" swing/s");
	Serial.println("Done reading swings from file");
	Serial.println(ret);
	count = 0;

}

void loop()
{

}