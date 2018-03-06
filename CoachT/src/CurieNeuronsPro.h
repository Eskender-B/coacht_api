/************************************************************************/
/*	CurieNeuronsPro.h	--	Driver for the Curie module
 *  revision 1.1, July 5, 2016 			    
 *	Copyright (c) 2016, General Vision Inc, All rights reserved	
 *
 * http://general-vision.com/documentation/TM_NeuroMem_Technology_Reference_Guide.pdf
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************/

 #ifndef CURIE_NEURONS_H
 #define CURIE_NEURONS_H
 
 #include <SD.h>

extern "C" {
  #include <stdint.h>
}

class CurieNeurons
{
	public:				
			
		CurieNeurons();
		int begin();
		void getNeuronsInfo(int* neuronSize, int* neuronsAvailable, int* neuronsCommitted);
		void forget();
		void forget(int Maxif);
		void clearNeurons();
		
		int learn(unsigned char vector[], int length, int category);
		int classify(unsigned char vector[], int length);
		int classify(unsigned char vector[], int length, int* distance, int* category, int* nid);
		int classify(unsigned char vector[], int length, int K, int distance[], int category[], int nid[]);

		void readNeuron(int nid, unsigned char model[], int* context, int* aif, int* category);
		void readNeuron(int nid, unsigned char neuron[]);
		int readNeurons(unsigned char neurons[]);
		int writeNeurons(unsigned char neurons[]);

		int saveKnowledge(const char* filePath);
		int loadKnowledge(const char* filePath);
				
		//-------------------------------------------
		// debugHelpers, send information
		//-------------------------------------------

		#define DEBUG_STR(txt)           if (_debugEnabled) { Serial.println(txt); }
		#define DEBUG_VAL(label, val)    if (_debugEnabled) { Serial.print(label);  Serial.print(':');  Serial.println(val); }
		#define DEBUG_VALX(label, val)    if (_debugEnabled) { Serial.print(label);  Serial.print(':');  Serial.println(val, HEX); }
		
		void debugEnable(bool enable); 
		void debugNeuronInfo();	
		void debugStatus();	
		
		//-------------------------------------------
		//Functions available in the Pro Library
		//-------------------------------------------

		void setContext(int context, int minif, int maxif);
		void getContext(int* context, int* minif, int* maxif);
		void setRBF();
		void setKNN();
		
		int NCOUNT();
		void NSR(int value);
		int NSR();
		void MINIF(int value);
		int MINIF();
		void MAXIF(int value);
		int MAXIF();
		void GCR(int value);
		int GCR();
		int DIST();
		void CAT(int value);
		int CAT();
		void NID(int value);
		int NID();
		void RSTCHAIN();	
		void AIF(int value);
		int AIF();
		void IDX(int value);									
		
	private:
		bool _debugEnabled=true;	
};

#endif // CURIE_NEURONS_H