/******************************************************************************/
/*	CurieNeuronsPro.h	--	Driver for the Curie module
 *  revision 1.2, Feb 10, 2017
 *      begin not only uncommit the neurons but clear their memory 
 *      readNeuron starts with index 0 and the ncr has become the 2nd output
 *  revision 1.1, July 5, 2016 
 *	Copyright (c) 2016, General Vision Inc, All rights reserved	
 *
 * http://general-vision.com/documentation/TM_NeuroMem_Technology_Reference_Guide.pdf
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
 */
/******************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <CurieNeuronsPro.h>

using namespace std;
extern "C" {
  #include <stdint.h>
}

/* PVP */
#define PVP_REGISTER_BASE   (0xB0600000)
#define NM_NCR       		*(volatile int*)(PVP_REGISTER_BASE + 0x00)
#define NM_COMP             *(volatile int*)(PVP_REGISTER_BASE + 0x04)
#define NM_LCOMP            *(volatile int*)(PVP_REGISTER_BASE + 0x08)
#define NM_DIST         	*(volatile int*)(PVP_REGISTER_BASE + 0x0C)
#define NM_IDX        		*(volatile int*)(PVP_REGISTER_BASE + 0x0C)
#define NM_CAT              *(volatile int*)(PVP_REGISTER_BASE + 0x10)
#define NM_AIF              *(volatile int*)(PVP_REGISTER_BASE + 0x14)
#define NM_MINIF            *(volatile int*)(PVP_REGISTER_BASE + 0x18)
#define NM_MAXIF            *(volatile int*)(PVP_REGISTER_BASE + 0x1C)
#define NM_TESTCOMP         *(volatile int*)(PVP_REGISTER_BASE + 0x20)
#define NM_TESTCAT          *(volatile int*)(PVP_REGISTER_BASE + 0x24)
#define NM_NID              *(volatile int*)(PVP_REGISTER_BASE + 0x28)
#define NM_GCR        		*(volatile int*)(PVP_REGISTER_BASE + 0x2C)
#define NM_RSTCHAIN       	*(volatile int*)(PVP_REGISTER_BASE + 0x30)
#define NM_NSR              *(volatile int*)(PVP_REGISTER_BASE + 0x34)
#define NM_FORGET  			*(volatile int*)(PVP_REGISTER_BASE + 0x3C)
#define NM_NCOUNT    		*(volatile int*)(PVP_REGISTER_BASE + 0x3C)


#define NEURONSIZE 	128 //memory capacity of each neuron in byte
#define MAXNEURONS 	128 //number of neurons per chip

// ------------------------------------------------------------ //
//    Constructor to the class CurieNeurons
// ------------------------------------------------------------ 
CurieNeurons::CurieNeurons(){	
}
// --------------------------------------------------------------
//  Initialize the neural network clearing any previous knowledge.
// --------------------------------------------------------------
int CurieNeurons::begin()
{
	clearNeurons();
	if (NM_MINIF==2) return(0); else return(1);
}
// ------------------------------------------------------------ 
// Report the neural network status and capacity
// ------------------------------------------------------------ 
void CurieNeurons::getNeuronsInfo(int* neuronSize, int* neuronsAvailable, int* neuronsCommitted)
{
	*neuronSize=NEURONSIZE;
	*neuronsAvailable=MAXNEURONS;
	*neuronsCommitted=NM_NCOUNT;
}
// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn
// Reset the Maximum Influence Field to default value=0x4000
// ------------------------------------------------------------ 
void CurieNeurons::forget()
{
	NM_FORGET=0;
}
// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// ------------------------------------------------------------ 
void CurieNeurons::forget(int Maxif)
{
	NM_FORGET=0;
	NM_MAXIF=Maxif;
}
// --------------------------------------------------------------
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// Clear the memory of the neurons
// --------------------------------------------------------------
void CurieNeurons::clearNeurons()
{
	int TempNSR=NM_NSR;
	NM_FORGET=0;
	NM_NSR= 16;
	for (int i=0; i< NEURONSIZE; i++) NM_TESTCOMP=0;
	NM_NSR=TempNSR;
}
//-----------------------------------------------
// Learn a vector as belonging to category
// and return the number of committed neurons
//----------------------------------------------
int CurieNeurons::learn(unsigned char vector[], int length, int category)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	NM_CAT=category;
	return(NM_NCOUNT);
}
// ---------------------------------------------------------
// Classify a vector and return its classification status
// NSR=0, unknown
// NSR=8, identified
// NSR=4, uncertain
// ---------------------------------------------------------
int CurieNeurons::classify(unsigned char vector[], int length)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	return(NM_NSR);
}
//----------------------------------------------
// Recognize a vector and return the best match, or the 
// category, distance and identifier of the top firing neuron
//----------------------------------------------
int CurieNeurons::classify(unsigned char vector[], int length, int* distance, int* category, int* nid)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	*distance = NM_DIST;
	*category= NM_CAT; // & 0x7FFF; // optional masking of Degenerated bit15
	*nid =NM_NID;
	return(NM_NSR);
}
//----------------------------------------------
// Recognize a vector and return the response  of up to K top firing neurons
// The response includes the distance, category and identifier of the neuron
// The Degenerated flag of the category is masked
// Return the number of firing neurons or K whichever is smaller
//----------------------------------------------
int CurieNeurons::classify(unsigned char vector[], int length, int K, int distance[], int category[], int nid[])
{
	int responseNbr=0;
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	for (int i=0; i<K; i++)
	{
		distance[i] = NM_DIST;
		if (distance[i]==0xFFFF)
		{ 
			category[i]=0xFFFF;
			nid[i]=0xFFFF;
		}
		else
		{
			responseNbr++;
			category[i]= NM_CAT; // & 0x7FFF; // optional masking of Degenerated bit15
			nid[i] = NM_NID;
		}
	}
return(responseNbr);
}
//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting index is 0
//-------------------------------------------------------------
void CurieNeurons::readNeuron(int nid, unsigned char model[], int* context,  int* aif, int* category)
{
	int TempNSR=NM_NSR;
	NM_NSR=0x0010;
	NM_RSTCHAIN= 0;
	if (nid>0)
	{
		 // move to index in the chain of neurons
		for (int i=0; i< nid; i++) NM_CAT;
	}
	*context=NM_NCR;
	for (int j=0; j<NEURONSIZE; j++) model[j]=NM_COMP;
	*aif=NM_AIF;
	*category=NM_CAT;
	NM_NSR=TempNSR; // set the NN back to its calling status
}
//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting index is 0
// Returns an array of NEURONSIZE+8 bytes with the following format
// 2-bytes NCR, NEURONSIZE-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//-------------------------------------------------------------
void CurieNeurons::readNeuron(int nid, unsigned char neuron[])
{
	int TempNSR=NM_NSR; // save value to restore NN status upon exit of the function
	NM_NSR=0x0010;
	NM_RSTCHAIN= 0;
	if (nid>0)
	{
		 // move to index in the chain of neurons
		for (int i=0; i< nid; i++) NM_CAT;
	}
	int Temp=NM_NCR;
	neuron[0]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[1]=(unsigned char)(Temp & 0x00FF);
	for (int j=0; j<NEURONSIZE; j++) { Temp=NM_COMP; neuron[j+2]= (unsigned char)Temp;}
	Temp=NM_AIF;
	neuron[NEURONSIZE+2]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+3]=(unsigned char)(Temp & 0x00FF);
	Temp=NM_MINIF;
	neuron[NEURONSIZE+4]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+5]=(unsigned char)(Temp & 0x00FF);
	Temp=NM_CAT;
	neuron[NEURONSIZE+6]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+7]=(unsigned char)(Temp & 0x00FF);
	NM_NSR=TempNSR; // set the NN back to its calling status
}
//----------------------------------------------------------------------------
// Read the contents of the committed neurons and output in a generic format compatible with
// all NeuroMem chips regardless of their neuron size and capacity
// The neurons array should be sized to HEADERSIZE + MAXNEURON * (NEURONSIZE + 8)
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//----------------------------------------------------------------------------
int CurieNeurons::readNeurons(unsigned char neurons[])
{
	int ncount= NM_NCOUNT;
	int TempNSR= NM_NSR; // save value to restore NN status upon exit of the function
	NM_NSR= 0x0010;
	NM_RSTCHAIN= 0;
	int offset=8;
	int MaxLength=NEURONSIZE; //can be less than NEURONSIZE
	int recLen=MaxLength+8; // memory plus 8 bytes of register data
	neurons[0]= offset; // default header size of 8 bytes
	neurons[1]= 0; // reserved to encode a future format identifier
	neurons[2]= (MaxLength & 0xFF00)>>8;
	neurons[3]= (MaxLength & 0x00FF);
	neurons[4]= (ncount & 0xFF000000)>>24;
	neurons[5]= (ncount & 0x00FF0000)>>16;
	neurons[6]= (ncount & 0x0000FF00)>>8;
	neurons[7]= (ncount & 0x000000FF);
	int Temp;	
	for (int i=0; i< ncount; i++)
	{
		offset+=(i*recLen);
		Temp=NM_NCR;
		neurons[ offset]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + 1]=(unsigned char)(Temp & 0x00FF);
		for (int j=0; j< MaxLength; j++) 
		{
			Temp=NM_COMP;
			neurons[ offset + 2 + j]=(unsigned char)(Temp & 0x00FF);
		}
		Temp=NM_AIF;
		neurons[ offset + MaxLength + 2]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 3]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_MINIF;
		neurons[ offset + MaxLength + 4]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 5]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_CAT;
		neurons[ offset + MaxLength + 6]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 7]=(unsigned char)(Temp & 0x00FF);
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	return(ncount);
}
//---------------------------------------------------------------------
// Clear the committed neurons and restore a new content for the neurons
// from an input array formatted as follows
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//---------------------------------------------------------------------
int CurieNeurons::writeNeurons(unsigned char neurons[])
{
	int offset= neurons[0];
	int MaxLength= (neurons[2]*256) + neurons[3];
	int ncount= (neurons[4]<<24) + (neurons[5]<<16) + (neurons[6]<<8) + neurons[7];
	if (ncount>MAXNEURONS) ncount=MAXNEURONS;
	int recLen=MaxLength+8;	
	int TempNSR=NM_NSR; // save value to restore NN status upon exit of the function
	int TempGCR=NM_GCR;
	NM_FORGET= 0;
	NM_NSR= 0x0010;
	NM_RSTCHAIN=0 ;
	for (int i=0; i< ncount; i++)
	{
		offset +=(i*recLen);
		NM_NCR= neurons[offset+1];
		for (int j=0; j<MaxLength; j++) NM_COMP=neurons[offset+2+j];
		NM_AIF= (neurons[offset + MaxLength +2]<<8)+neurons[offset + MaxLength+3];
		NM_MINIF= (neurons[offset + MaxLength+4]<<8)+neurons[offset + MaxLength+5];	
		NM_CAT= (neurons[offset + MaxLength+6]<<8)+neurons[offset + MaxLength+7];
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	NM_GCR=TempGCR;
	return(ncount);
}

int CurieNeurons::saveKnowledge(const char* filePath)
{

	if(SD.exists(filePath))
		SD.remove(filePath);
	File file = SD.open(filePath, FILE_WRITE);
	if(!file)
		return -1;

	int ncount= NM_NCOUNT;
	int TempNSR= NM_NSR; // save value to restore NN status upon exit of the function
	NM_NSR= 0x0010;
	NM_RSTCHAIN= 0;
	int offset=8;
	int MaxLength=NEURONSIZE; //can be less than NEURONSIZE
	unsigned int recLen=MaxLength+8; // memory plus 8 bytes of register data


	byte buffer[recLen];

	buffer[0]= offset; // default header size of 8 bytes
	buffer[1]= 0; // reserved to encode a future format identifier
	buffer[2]= (MaxLength & 0xFF00)>>8;
	buffer[3]= (MaxLength & 0x00FF);
	buffer[4]= (ncount & 0xFF000000)>>24;
	buffer[5]= (ncount & 0x00FF0000)>>16;
	buffer[6]= (ncount & 0x0000FF00)>>8;
	buffer[7]= (ncount & 0x000000FF);

	if(file.write(buffer, 8)!=8)
		return -1;

	int Temp;	
	for (int i=0; i< ncount; i++)
	{
		Temp=NM_NCR;
		buffer[0]=(unsigned char)((Temp & 0xFF00)>>8);
		buffer[1]=(unsigned char)(Temp & 0x00FF);
		for (int j=0; j< MaxLength; j++) 
		{
			Temp=NM_COMP;
			buffer[2 + j]=(unsigned char)(Temp & 0x00FF);
		}
		Temp=NM_AIF;
		buffer[MaxLength + 2]=(unsigned char)((Temp & 0xFF00)>>8);
		buffer[MaxLength + 3]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_MINIF;
		buffer[MaxLength + 4]=(unsigned char)((Temp & 0xFF00)>>8);
		buffer[MaxLength + 5]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_CAT;
		buffer[MaxLength + 6]=(unsigned char)((Temp & 0xFF00)>>8);
		buffer[MaxLength + 7]=(unsigned char)(Temp & 0x00FF);

		if(file.write(buffer, recLen)!=recLen)
			return -1;
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	file.close();
	return(ncount);
}

int CurieNeurons::loadKnowledge(const char* filePath)
{
	File file = SD.open(filePath, FILE_READ);
	if(!file)
		return -1;
	int len = NEURONSIZE+8;
	byte buffer[len];

	if(file.read(buffer, 8)!=8)
		return -1;

	int offset= buffer[0]-8;
	int MaxLength= (buffer[2]*256) + buffer[3];
	int ncount= (buffer[4]<<24) + (buffer[5]<<16) + (buffer[6]<<8) + buffer[7];
	if (ncount>MAXNEURONS) ncount=MAXNEURONS;
	int recLen=(int)(MaxLength+8);	
	int TempNSR=NM_NSR; // save value to restore NN status upon exit of the function
	int TempGCR=NM_GCR;
	NM_FORGET= 0;
	NM_NSR= 0x0010;
	NM_RSTCHAIN=0 ;

	if(file.read(buffer, offset)!=offset)
		return -1;

	for (int i=0; i< ncount; i++)
	{
		if(file.read(buffer, recLen)!=recLen)
			return -1;


		NM_NCR= buffer[1];
		for (int j=0; j<MaxLength; j++) NM_COMP=buffer[2+j];
		NM_AIF= (buffer[MaxLength +2]<<8)+buffer[MaxLength+3];
		NM_MINIF= (buffer[MaxLength+4]<<8)+buffer[MaxLength+5];	
		NM_CAT= (buffer[MaxLength+6]<<8)+buffer[MaxLength+7];
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	NM_GCR=TempGCR;
	file.close();
	return(ncount);

}
				
//-------------------------------------------
// debugHelpers, send information
//-------------------------------------------

// enable debugging information on Serial port.
void CurieNeurons::debugEnable(bool enable) {
  _debugEnabled = enable;
} // debugEnable()

void CurieNeurons::debugNeuronInfo() {
	// tdb.
} // debugNeuronInfo()

void CurieNeurons::debugStatus() {
  // no output.
} // debugStatus

/**********************************************************************/
/* 
 * The following functions are supplied in the Advanced library
 */
/**********************************************************************/

// ------------------------------------------------------------ 
// Set a context and associated norm, minimum and maximum influence fields
// ------------------------------------------------------------ 
void CurieNeurons::setContext(int context, int minif, int maxif)
{
	// context[15-8]= unused
	// context[7]= Norm (0 for L1; 1 for LSup)
	// context[6-0]= Active context value
	NM_GCR=context;
	NM_MINIF=minif;
	NM_MAXIF=maxif;
}
// ------------------------------------------------------------ 
// Get a context and associated norm, minimum and maximum influence fields
// ------------------------------------------------------------ 
void CurieNeurons::getContext(int* context, int* minif, int* maxif)
{
	// context[15-8]= unused
	// context[7]= Norm (0 for L1; 1 for LSup)
	// context[6-0]= Active context value
	*context = NM_GCR;
	*minif= NM_MINIF; 
	*maxif =NM_MAXIF;
}
// --------------------------------------------------------
// Set the neurons in Radial Basis Function mode (default)
//---------------------------------------------------------
void CurieNeurons::setRBF()
{
	int tempNSR=NM_NSR;
	NM_NSR=tempNSR & 0xDF;
}
// --------------------------------------------------------
// Set the neurons in K-Nearest Neighbor mode
//---------------------------------------------------------
void CurieNeurons::setKNN()
{
	int tempNSR=NM_NSR;
	NM_NSR=tempNSR | 0x20;
}
// --------------------------------------------------------
// Read the number of committed neurons
//---------------------------------------------------------
int CurieNeurons::NCOUNT()
{
	return(NM_NCOUNT);
}
// --------------------------------------------------------
// Get/Set the Network Status register
// bit 2 = UNC (read only)
// bit 3 = ID (read only)
// bit 4 = SR mode
// bit 5= KNN mode
//---------------------------------------------------------
void CurieNeurons::NSR(int value)
{
	NM_NSR=value;
}
int CurieNeurons::NSR()
{
	return(NM_NSR);
}
// --------------------------------------------------------
// Get/Set the Minimum Influence Field register
//---------------------------------------------------------
void CurieNeurons::MINIF(int value)
{
	NM_MINIF=value;
}
int CurieNeurons::MINIF()
{
	return(NM_MINIF);
}
// --------------------------------------------------------
// Get/Set the Maximum Influence Field register
//---------------------------------------------------------
void CurieNeurons::MAXIF(int value)
{
	NM_MAXIF=value;
}
int CurieNeurons::MAXIF()
{
	return(NM_MAXIF);
}
// --------------------------------------------------------
// Get/Set the Global Context register
//---------------------------------------------------------
void CurieNeurons::GCR(int value)
{
	// GCR[15-8]= unused
	// GCR[7]= Norm (0 for L1; 1 for LSup)
	// GCR[6-0]= Active context value
	NM_GCR=value;
}
int CurieNeurons::GCR()
{
	return(NM_GCR);
}
// --------------------------------------------------------
// Get the Distance register
//---------------------------------------------------------
int CurieNeurons::DIST()
{
	return(NM_DIST);
}
// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void CurieNeurons::CAT(int value)
{
	NM_CAT=value;
}
int CurieNeurons::CAT()
{
	return(NM_CAT);
}
// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void CurieNeurons::NID(int value)
{
	NM_NID=value;
}
int CurieNeurons::NID()
{
	return(NM_NID);
}
// --------------------------------------------------------
// Reset the neuron chain to fisrt
//---------------------------------------------------------
void CurieNeurons::RSTCHAIN(void)
{
	NM_RSTCHAIN=0;
}
// --------------------------------------------------------
// Get/Set the Active Influence Field register
//---------------------------------------------------------
void CurieNeurons::AIF(int value)
{
	NM_AIF=value;
}
int CurieNeurons::AIF()
{
	return(NM_AIF);
}
// --------------------------------------------------------
// Set the Component Index register
//---------------------------------------------------------
void CurieNeurons::IDX(int value)
{
	NM_IDX=value;
}



