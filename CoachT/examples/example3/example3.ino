#include <CoachT.h>

CoachT ccc(Serial);

void setup()
{
	Serial.begin(9600);
	while(!Serial);

	float accuracy;
	
	ccc.begin(1, 16);

	int ret;
	if((ret = ccc.train("COACHT/DATA/TRAIN/", 395, 3.0f))>0)
	{
		Serial.print("Number of committed neurons: "); Serial.println(ret);
		ccc.validate("COACHT/DATA/TEST/", 3.0f, accuracy);
	}

	
	if((ret=ccc.saveKnowledge("COACHT/MODEL.DAT"))>0)
	{
		Serial.print("Knowledge saved in ");Serial.println("\"COACHT/MODEL.DAT\"");
	}
	return;
}

void loop()
{
	
}