
#ifndef COACHT_H
#define COACHT_H

#include <Arduino.h>
#include <SD.h>
#include <CurieIMU.h>
#include <CurieNeuronsPro.h>

extern "C" {
  #include <stdint.h>
}


class CoachT
{
public:
	CoachT(Stream& s);
	//~CoachT();
	int begin(int useSDCard, int accRange);

	int saveKnowledge(const char* filePath);
	int loadKnowledge(const char* filePath);
	
	int train(String fileDir, int maxIF, float threshold);
	int validate(String fileDir, float threshold, float& accuracy);

	int readSwingFromSensor(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold);
	int readSwingFromFile(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold);

	int sampleSwing(float inputX[], float inputY[], float inputZ[], int len1, float outputX[], float outputY[], float outputZ[], int len2);
	int learnSwing(byte pattern[], int length, int category);
	int classifySwing(byte pattern[], int length, int K, int distance[], int category[], int nids[]);

	
	
	int readFromSensor(float vector[], int length);
	int readFromFile(float vector[], int length);


	int getSwingPatternFromSensor(byte pattern[], int length, float threshold);
	int getSwingPatternFromFile(byte pattern[], int length, float threshold);
	
	int openFile(String filePath);
	void closeFile();
	int buildPattern(float vectorX[], float vectorY[], float vectorZ[], int length1, byte pattern[], int length2);
	void setAccelerometerRange(int accRange);

private:
	int readSwing(float vectorX[], float vectorY[], float vectorZ[], int length, float threshold, int option);
	int read(float vector[], int length, int option);
	int getSwingPattern(byte pattern[], int length, float threshold, int option);
	int toFloat(String buff , float vector[], int length);



	CurieNeurons hNN;
	Stream& serial;
	File file;
	const static int _size = 500;
	int _start = 0;
	int _end = 0;
	int useSDCard = 0;
	int accRange;

	int maxNeuronSize = 128;
	int maxNumOfFiringNeurons = 1;
	int patternLength = 126;
	int fromMax = 130;
	float ratio = 0.95;
	float _bufferX[_size];
	float _bufferY[_size];
	float _bufferZ[_size];

public:
	const static int sampleSize = 42;
	const static int numOfCategory = 7;
	const char* category[numOfCategory] = {"", "FOREHAS", "FOREHAH", "FOREHAL", "BACKHAS", "BACKHAH", "BACKHAL"};
	const int sampleIndexs[sampleSize]={103, 106, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
								    120, 121, 122, 123, 124, 125, 126, 127, 128, 132, 133, 134, 135, 136, 
								    137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 150, 160};
	
};

#endif