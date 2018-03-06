#include <CoachT.h>

CoachT::CoachT(Stream& s):serial(s)
{	
	_start = 0;
	_end = 0;
	_bufferX[_start] = 0;
	_bufferY[_start] = 0;
	_bufferZ[_start] = 0;
}

int CoachT::begin(int useSDCard=0, int accRange=16)
{	

	this->useSDCard = useSDCard;
	this->accRange = accRange;

	this->hNN.begin();
	this->hNN.forget();

	CurieIMU.begin();
	CurieIMU.autoCalibrateXAccelOffset(0);
	CurieIMU.autoCalibrateYAccelOffset(0);
	CurieIMU.autoCalibrateZAccelOffset(1);
  	CurieIMU.setAccelOffsetEnabled(true);

	CurieIMU.setAccelerometerRange(accRange);

	if(useSDCard)
	{
		serial.print("Initializing SD card...");
		if (!SD.begin(4)) 
		{
			serial.println("initialization failed!");
			return -1;
		}
		serial.println("initialization done.");

	}

			
	return 1;
}

int CoachT::saveKnowledge(const char* filePath)
{
	return this->hNN.saveKnowledge(filePath);
}

int CoachT::loadKnowledge(const char* filePath)
{
	return this->hNN.loadKnowledge(filePath);
}

int CoachT::train(String fileDir, int maxIF, float threshold)
{
	this->hNN.forget(maxIF);

	int neuronsAvailable, neuronsCommitted, neuronSize;

	for(int i=1; i<numOfCategory;i++)
	{
		//char nam[50];
		//nam = fileDir.c_str();
		//strcat(nam, category[i]);
		//strcat(nam, ".CSV");
		if(this->openFile(fileDir+String(category[i])+".CSV")<0)
			return -1;

		serial.print("\n\n");serial.println(this->file.name());serial.print("\n\n");

		byte pattern[this->patternLength];

		int ret;
		int count = 0;
		while((ret=this->getSwingPatternFromFile(pattern, this->patternLength, threshold))>0)
		{
			serial.print("Swing "); serial.println(++count);
			this->learnSwing(pattern, this->patternLength, i);
		}	
		if(ret == 0)
		{
			this->file.close();
			continue;
		}
		else
			return -1;

	}


	hNN.getNeuronsInfo( &neuronSize, &neuronsAvailable, &neuronsCommitted);
	serial.print("Neuron size ="); serial.println(neuronSize);
	serial.print("Neurons available ="); serial.println(neuronsAvailable);
	serial.print("Neurons committed ="); serial.println(neuronsCommitted);

	return neuronsCommitted;
}

int CoachT::validate(String fileDir, float threshold, float& accuracy)
{
	int dists[this->maxNumOfFiringNeurons], cats[this->maxNumOfFiringNeurons], nids[this->maxNumOfFiringNeurons];
	
	int total = 0;
	int correct = 0;

	for(int i=1; i<numOfCategory;i++)
	{
		//char nam[50];
		//nam = fileDir.c_str();
		//strcat(nam, category[i]);
		//strcat(nam, ".CSV");
		if(this->openFile(fileDir+String(category[i])+".CSV")<0)
			return -1;

		serial.print("\n\n");serial.println(this->file.name());serial.println();

		byte pattern[this->patternLength];

		int ret;
		int count = 0;
		while((ret=this->getSwingPatternFromFile(pattern, this->patternLength, threshold))>0)
		{
			serial.print("\nSwing "); serial.print(++count); serial.print(", CAT: ");serial.print(i);
			serial.print('(');serial.print(category[i]);serial.println(')'); 
			int responseNbr= this->classifySwing(pattern, this->patternLength, this->maxNumOfFiringNeurons, dists, cats, nids);

			for (int k=0; k<responseNbr; k++)
			{
				serial.print("Firing neuron "); serial.print(nids[k]);
				serial.print(", Category="); serial.print(cats[k]);
				if(cats[k]>0 && cats[k]<numOfCategory)
					serial.print('(');serial.print(category[cats[k]]);serial.print(')'); 
				serial.print(", at Distance="); serial.println(dists[k]);
			} 
			total++;
			if(cats[0]==i)
				correct++;
       
		}	
		if(ret == 0)
		{
			this->file.close();
			continue;
		}
		else
			return -1;

	}
	serial.println("\n\nDone prediction.");
	serial.print("Correctly predicted "); Serial.print(correct); Serial.print("/");
	serial.println(total);
	serial.print("ACCURACY = "); Serial.println(float(correct)/total);
	accuracy = float(correct)/total;

	return 1;
}

int CoachT::readSwing(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold, int option=0)
{	
	if(length < 2*this->fromMax+1)
		return -1;

	int k = 3;
	float vector[k];

	bool thresh = false;
	float maxx = 0.0;
	int indx = -1;
	float abslt = 0;
	int ret = 0;
	int right = 0;
	int left = 0;
	int padLeft = 0;
	int padRight = 2*fromMax;

	while((ret=read(vector, k, option))>0)
	{
		_end = ++_end % _size;
		if(_end == _start)
			_start = ++_start % _size;

		_bufferX[_end] = vector[0];
		_bufferY[_end] = vector[1];
		_bufferZ[_end] = vector[2];

		abslt = abs(_bufferX[_end]);
		if(abslt > threshold)
			thresh = true;
		if(thresh && abslt > maxx)
		{
			maxx = abslt;
			indx = _end;
		}
		else if(maxx > threshold && maxx-abslt > this->ratio*maxx)
		{

			left = (indx-fromMax+_size)%_size;
			right = (indx+fromMax)%_size;

			//serial.print("Left: "); serial.println(left);
			//serial.print("Right: "); serial.println(right);

			int x;
			if((x=(indx-_start+_size)%_size)<fromMax)
			{
				left = _start;
				padLeft = fromMax - x;
				for(int i=0; i<padLeft; i++)
				{
					vectorX[i] = _bufferX[_start];
					vectorY[i] = _bufferY[_start];
					vectorZ[i] = _bufferZ[_start];
				}
			}
			if((x=(_end-indx+_size)%_size)<fromMax)
			{
				right = _end;
				padRight = fromMax - x;
				padRight = 2*fromMax - padRight;
				for(int i=2*fromMax; i>padRight; i--)
				{
					vectorX[i] = _bufferX[_end];
					vectorY[i] = _bufferY[_end];
					vectorZ[i] = _bufferZ[_end];
				}
			}
			for(int i=0; i<=padRight-padLeft; i++)
			{
				vectorX[padLeft+i] = _bufferX[(left+i)%_size];
				vectorY[padLeft+i] = _bufferY[(left+i)%_size];
				vectorZ[padLeft+i] = _bufferZ[(left+i)%_size];

			}

			/*
			serial.print("Max: "); serial.println(maxx);
			serial.print("Index: ");serial.println(indx);
			serial.print("End: ");serial.println(end);
			serial.print("Left: "); serial.println(left);
			serial.print("Right: "); serial.println(right);
			serial.print("padRight: ");serial.println(padRight);
			serial.print("padLeft: "); serial.println(padLeft);
			serial.print("pr-pl: "); serial.println(padRight-padLeft);
			serial.print("end-start:  ");serial.println((end-start+size)%size);
			*/
			break;

		}
	
	}
	if(ret==0)
		this->file.close();

	_start = _end;
	return ret;

}

int CoachT::readSwingFromSensor(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold)
{
	return this->readSwing(vectorX, vectorY, vectorZ, length, threshold, 0);
}

int CoachT::readSwingFromFile(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold)
{
	return this->readSwing(vectorX, vectorY, vectorZ, length, threshold, 1);
}

int CoachT::sampleSwing(float inputX[], float inputY[], float inputZ[], int length1, float outputX[], float outputY[], float outputZ[], int length2)
{
	if(length2 > sampleSize)
		return -1;

	for (int i = 0; i < sampleSize; ++i)
	 {
	 	if(sampleIndexs[i]<0 || sampleIndexs[i]>=length1)
	 		return -1;
	 	outputX[i] = inputX[sampleIndexs[i]];
	 	outputY[i] = inputY[sampleIndexs[i]];
	 	outputZ[i] = inputZ[sampleIndexs[i]];
	 } 
	return 1;
}

int CoachT::learnSwing(byte pattern[], int length, int category)
{

	this->hNN.setRBF();
	return this->hNN.learn(pattern, length, category);
}



int CoachT::classifySwing(byte pattern[], int length, int K, int distance[], int category[], int nids[])
{	
	this->hNN.setKNN();

	return this->hNN.classify(pattern, length, K, distance, category, nids);
}

int CoachT::read(float vector[], int length, int option=0)
{
	if(option==0)
		return this->readFromSensor(vector, length);
	else
		return this->readFromFile(vector, length);
}

int CoachT::readFromSensor(float vector[], int length)
{
	if (length!=3)
		return -1;

	static uint64_t timer = millis();

	while(millis() - timer <= 10)
		;

	timer = millis();

	CurieIMU.readAccelerometerScaled(vector[0], vector[1], vector[2]);
	return 1;
}

int CoachT::readFromFile(float vector[], int length)
{
	if(!file)
		return -1;
	if(this->file.available())
	{
		return this->toFloat(this->file.readStringUntil('\n'), vector, length);
	}
	else
		return 0;
}

int CoachT::getSwingPattern(byte pattern[], int length, float threshold, int option)
{
	int len = 2*fromMax+1;
	float vectorX[len];
	float vectorY[len];
	float vectorZ[len];

	float outputX[sampleSize];
	float outputY[sampleSize];
	float outputZ[sampleSize];

	int ret;
	if((ret=this->readSwing(vectorX, vectorY, vectorZ, len, threshold, option))>0)
	{
		if(this->sampleSwing(vectorX, vectorY, vectorZ, len, outputX, outputY, outputZ, sampleSize)<0)
			return -1;

		if(this->buildPattern(outputX, outputY, outputZ, sampleSize, pattern, length)<0)
			return -1;
	}
	return ret;
}


int CoachT::getSwingPatternFromSensor(byte pattern[], int length, float threshold)
{
	return this->getSwingPattern(pattern, length, threshold, 0);
}
int CoachT::getSwingPatternFromFile(byte pattern[], int length, float threshold)
{
	return this->getSwingPattern(pattern, length, threshold, 1);
}


int CoachT::openFile(String filePath)
{
	if(!useSDCard)
		return -1;
	
	this->closeFile();

	if(this->file = SD.open(filePath))
		return 1;
	else
		return -1;
}

void CoachT::closeFile()
{
	if(useSDCard && this->file)
		file.close();
}


int CoachT::buildPattern(float vectorX[], float vectorY[], float vectorZ[], int length1, byte pattern[], int length2)
{
	if(3*length1>this->maxNeuronSize)
		return -1;
	if(length2>this->maxNeuronSize)
		return -1;

	float minx = -1.0f*this->accRange;
	float maxx = 1.0f*this->accRange;
	float range = maxx - minx;
	float temp;

	for(int i=0; i<length1; i++)
	{
		temp = ((vectorX[i]-minx)/range)*255.0;
		pattern[i*3] = (int)temp & 0xFF;

		if(temp-(int)temp >= 0.5)
			pattern[i*3]+=1;

		temp = ((vectorY[i]-minx)/range)*255.0;
		pattern[i*3+1] = (int)temp & 0xFF;

		if(temp-(int)temp >= 0.5)
			pattern[i*3+1]+=1;

		temp = ((vectorZ[i]-minx)/range)*255.0;
		pattern[i*3+2] = (int)temp & 0xFF;

		if(temp-(int)temp >= 0.5)
			pattern[i*3+2]+=1;
	}

	return 1;
}


int CoachT::toFloat(String buff, float vector[], int length)
{
	if(length!=3)
		return -1;

	int firstComma;
	int secondComma;

	if((firstComma= buff.indexOf(','))<0)
		return -1;

	if((secondComma = buff.indexOf(',', firstComma+1))<0)
		return -1;

	vector[0]=buff.substring(0, firstComma).toFloat();
	vector[1]=buff.substring(firstComma+1, secondComma).toFloat();
	vector[2]=buff.substring(secondComma+1).toFloat(); 

	return 1;
}


void CoachT::setAccelerometerRange(int accRange)
{
	CurieIMU.setAccelerometerRange(accRange);
	this->accRange = accRange;	
}