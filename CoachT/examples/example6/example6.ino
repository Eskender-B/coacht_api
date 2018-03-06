#include <CoachT.h>

CoachT ccc(Serial);

#define K 1
int dists[K], cats[K], nids[K];
int count = 0;
bool flag = true;

void setup()
{
	Serial.begin(9600);
	while(!Serial);

	float accuracy;

	ccc.begin(1, 16);

	if(ccc.loadKnowledge("COACHT/MODEL.DAT")<0)
	{
		Serial.println("Failed to load knowledge");
		flag = false;
	}
	Serial.println("Loaded knowledge from ");Serial.println("\"COACHT/MODEL.DAT\"");

	const int len = 126;

	byte pattern[len];

	Serial.println("\n\nReading swing from file until swing is detected ...");
	
	if(!ccc.openFile("COACHT/DATA/FOREHAS.CSV"))
	{
		Serial.println("Error openning file");
		return;
	}

	int ret;
	while(ret = ccc.getSwingPatternFromFile(pattern, len, 3.0f) > 0 && flag)
	{	
		count++;
		Serial.print("Swing "); Serial.print(count);Serial.println(":");
		int responseNbr = ccc.classifySwing(pattern, len, K, dists, cats, nids);
		for (int k=0; k<responseNbr; k++)
		{
			Serial.print("Firing neuron "); Serial.print(nids[k]);
			Serial.print(", Category="); Serial.print(cats[k]);
			if(cats[k]>0 && cats[k]<ccc.numOfCategory)
				Serial.print('(');Serial.print(ccc.category[cats[k]]);Serial.print(')'); 
			Serial.print(", at Distance="); Serial.println(dists[k]);
		} 
	
	}
	if(ret ==0)
		Serial.println("Done reading from file");
	else
		Serial.println("Error");

}

void loop()
{	
}