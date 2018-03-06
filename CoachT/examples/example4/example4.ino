#include <CoachT.h>

CoachT ccc(Serial);

#define K 1
int dists[K], cats[K], nids[K];
int count = 0;

void setup()
{
	Serial.begin(9600);
	while(!Serial);

	float accuracy;

	ccc.begin(1, 16);

	if(ccc.loadKnowledge("COACHT/MODEL.DAT")>0)
	{
		Serial.println("Loaded knowledge from ");Serial.println("\"COACHT/MODEL.DAT\"");
		ccc.validate("COACHT/DATA/TEST/",3.0f, accuracy);
	}
	Serial.print("\n Accuracy = ");Serial.println(accuracy); 
	return;
}

void loop()
{	

}