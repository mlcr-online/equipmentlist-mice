#include "TrVMEDevice.hh"
#include "TrVLSBController.hh"

#include <iostream>

TrVLSBController::TrVLSBController() {}

void TrVLSBController::initialise(int baseAddress) {

	TrVMEDevice::initialise(baseAddress);
	id = (baseAddress & 0xF0000000) >> 28; // Take first 4 bits of address as board ID - will range 0-15
	vlsbParameters.insert( std::pair<std::string, int> ("numberOfBanks",4) );
	vlsbParameters.insert( std::pair<std::string,int> ("bankAddressOffset",0x100000) );
	vlsbParameters.insert( std::pair<std::string,int> ("bankLengthOffset",0x4));

	addresses.insert( std::pair<std::string, int> ("bank0",0x0) );
	addresses.insert( std::pair<std::string, int> ("bank1",0x4) );	
	addresses.insert( std::pair<std::string, int> ("bank2",0x8) );
	addresses.insert( std::pair<std::string, int> ("bank3",0xC) );
	addresses.insert( std::pair<std::string, int> ("control",0x10) );
	addresses.insert( std::pair<std::string, int> ("spillDuration",0x14) );	
	addresses.insert( std::pair<std::string, int> ("triggerSettings",0x18) );
	addresses.insert( std::pair<std::string, int> ("vetoSettings",0x1C) );	
	addresses.insert( std::pair<std::string, int> ("recordedTriggers",0x20) );
	addresses.insert( std::pair<std::string, int> ("lvdsControl",0x24) );	
	addresses.insert( std::pair<std::string, int> ("firmwareVersion",0x28) );
	addresses.insert( std::pair<std::string, int> ("fifoStatus",0x2C) );
	addresses.insert( std::pair<std::string, int> ("memoryStatus",0x30) );
	addresses.insert( std::pair<std::string, int> ("triggerTDC",0x34) );	
	addresses.insert( std::pair<std::string, int> ("bank0CRC",0x38) );
	addresses.insert( std::pair<std::string, int> ("bank1CRC",0x3C) );
	addresses.insert( std::pair<std::string, int> ("bank2CRC",0x40) );
	addresses.insert( std::pair<std::string, int> ("bank3CRC",0x44) );
	addresses.insert( std::pair<std::string, int> ("bankStart",0x1000000));	
	
	fifoStatus.insert( std::pair<int, std::string> (0xF, "FIFO empty") );
	fifoStatus.insert( std::pair<int, std::string> (0xF0, "FIFO full") );
	fifoStatus.insert( std::pair<int, std::string> (0xF00, "FIFO timeout") );

	triggerSetting.insert( std::pair<std::string, int> ("spillEnable",0x4) );
	triggerSetting.insert( std::pair<std::string, int> ("continuousSpill",0x10) );
	triggerSetting.insert( std::pair<std::string, int> ("external",0x20) );
	triggerSetting.insert( std::pair<std::string, int> ("noZeroSuppression",0x40) );
	triggerSetting.insert( std::pair<std::string, int> ("noVeto",0x80) );

	lvdsSetting.insert( std::pair<std::string, int> ("disableLVDS", 0x0) );
	lvdsSetting.insert( std::pair<std::string, int> ("enableLVDS", 0x40) );
	
	bankLengths.resize(vlsbParameters["numberOfBanks"],0);
}

void TrVLSBController::getBankLengths() {
	
	int address = 0x0;
	int vmeStatus = 0x0;
	int banks = vlsbParameters["numberOfBanks"];
	for (int bank=0; bank < banks; ++bank) {
		address = baseAddress + addresses["bank0"] + (bank*vlsbParameters["bankLengthOffset"]);
		try {
			vmeStatus |= CAENVME_ReadCycle( 0,address, &bankLengths[bank], cvA32_U_DATA, cvD32);
		}
		catch(...) {
			//std::cout << "VME Error" << std::endl;
		}
		//std::cout << "Bank length register " << std::hex << address << std::endl;
		//std::cout << bank << " " << bankLengths[bank] << std::endl;
	}
}

void TrVLSBController::setReadoutMode() {
	//int en = 0xF;
	//CAENVME_WriteCycle(0, baseAddress + addresses["control"], &en, addressModifier, dataWidth);
	CAENVME_WriteCycle(0, baseAddress + addresses["lvdsControl"], &lvdsSetting["disableLVDS"], addressModifier, dataWidth);
	std::cout << "Set readout mode " << (baseAddress+addresses["lvdsControl"]) << " " << lvdsSetting["disableLVDS"] << std::endl;
}

void TrVLSBController::setDataMode() {
	int en = 0x40;
	CAENVME_WriteCycle(0, baseAddress + addresses["control"], &en, addressModifier, dataWidth);
	CAENVME_WriteCycle(0, baseAddress + addresses["lvdsControl"], &lvdsSetting["enableLVDS"], addressModifier, dataWidth);
	std::cout << "Set lvds mode " << (baseAddress+addresses["lvdsControl"]) << " " << lvdsSetting["enableLVDS"] << std::endl;
}

int TrVLSBController::getTotalDataVolume() {
	int volume = 0;
	for (int bank=0; bank<4; ++bank) {
		volume += bankLengths[bank];
	}
	return volume;
}

int *TrVLSBController::readAllBanks() {
	int *dataPointer = (int*)calloc(getTotalDataVolume(), 4 );
	int *pointerStart = dataPointer;
	for (int bank = 0; bank < 4; ++bank) {
		std::cout << "Reading bank " << bank << std::endl;
		//dataPointer += readBank(bank, 0, dataPointer);
	}
	return pointerStart;
}

int TrVLSBController::readBank(int bank, int readoutMode, MDE_Pointer* dataPointer = NULL) {

	int bytesRead = 0;
	unsigned int address = baseAddress + addresses["bankStart"] + ((bank)*vlsbParameters["bankAddressOffset"]);
	unsigned int addressArray[bankLengths[bank]];
	unsigned int dataBuffer[bankLengths[bank]];
	int nCycles = bankLengths[bank];
	CVErrorCodes errors[bankLengths[bank]];
	CVAddressModifier addressModifiers[bankLengths[bank]];
	CVDataWidth dataWidths[bankLengths[bank]];	
	for (int word=0; word<bankLengths[bank]; ++word) {
		addressArray[word] = address;
		dataBuffer[word] = 0;
		addressModifiers[word] = cvA32_U_DATA;
		dataWidths[word] = cvD32; 	
		address += 4;
		//std::cout << << std::hex << address << std::endl;
	}
	try {	
		CAENVME_MultiRead(0,addressArray, dataBuffer, nCycles, addressModifiers, dataWidths, errors);
   	}
	catch (...) {
		//std::cout << "Failed to multiread VME" << std::endl;
	}
	//std::cout << "Printing bank contents" << std::endl;
	for (int i=0; i<bankLengths[bank]; ++i) {
	//	std::cout << std::hex << dataBuffer[i] << std::endl;
	}
	/* Move the data from the buffer to somewher else, as required */
	switch(readoutMode) {
		case 0:
			if (!dataPointer) break;
			for (int j=0; j<bankLengths[bank]; j++){ 
				//std::cout << "Data: " << std::hex << dataBuffer[j] << std::endl;
				//std::cout << "Address: " << std::hex << addressArray[j] << std::endl;
      				*dataPointer = dataBuffer[j];
				//std::cout << "Filled pointer as " << *dataPointer << std::endl;
				dataPointer++;
      				bytesRead += 1;
    			} 
			break;
		default:
			//do nothing
			break;
	}
	//std::cout << "words " << bytesRead << std::endl;
    	return bytesRead;
}

int TrVLSBController::verifyBankSpill(int* dataPointer, int dataVolume, int bankNumber) {
	int crcAddress = baseAddress + addresses["bank0CRC"];
	int afeCRC = 0;
	int status = CAENVME_ReadCycle( 0,crcAddress, &afeCRC, cvA32_U_DATA, cvD32); 
	int vlsbCRC = calculateCRC(dataPointer, dataVolume);
	int verification = (afeCRC & vlsbCRC);
	return verification;
}

int TrVLSBController::calculateCRC(int* dataPointer, int dataVolume) {
	return 0;
}
int TrVLSBController::getID() {return baseAddress;}

