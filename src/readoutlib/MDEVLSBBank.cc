#include "MDEVLSBBank.hh"
#include "MiceDAQMessanger.hh"
#include "VLSBDef.hh"

// For readout timing:
#include <time.h>


IMPLEMENT_FUNCTIONS(VLSBBankParams, MDEVLSBBank)

MDEVLSBBank::MDEVLSBBank()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(VLSBBankParams)
  mde_name_ = "VLSBBank";
}

// Arm the detectors..
bool MDEVLSBBank::Arm()
{
  
  // Arm Message:
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  // Set the base address & other parameters:
  mde_base_address_ = (*this)["BaseAddress"];

  // Perform Setup tasks:
  bool Status (true);

  if ( Status )
    {
      mde_messanger_->sendMessage(this,"Arming successful.", MDE_INFO);
      return true;
    } 
  else 
    {
      mde_messanger_->sendMessage(this, "Arming failed.", MDE_FATAL);
      return false;
    }
}

int MDEVLSBBank::ReadEvent() {

	/* FIFO seems to read bad values (0x000f), which is empty..
	 * This is normal, since the empty bits are set when the data is cleared
	 * on the VLSB.
	 *
	 * TODO: Test input and output
	 * */

	// Load the FIFO Status:
	int FIFOStatus = ReadFIFOStatus();

	// Check against possible errors:
	if (FIFOStatus & VLSB_FIFOFull)
	{
		mde_messanger_->sendMessage(this, "FIFO Bank Full", MDE_WARNING);
	}

	if ((FIFOStatus & VLSB_FIFOEmpty) != VLSB_FIFOEmpty) // Note, this bit is normally set to 1.
	{
		mde_messanger_->sendMessage(this, "FIFO was not properly cleared.", MDE_WARNING);
	}

	if (FIFOStatus & VLSB_DataTimeout)
	{
		mde_messanger_->sendMessage(this, "FIFO Data Timeout", MDE_WARNING);
	}

	if (FIFOStatus & VLSB_CRCError)
	{
			mde_messanger_->sendMessage(this, "FIFO CRC Error in data from AFE to VLSB", MDE_WARNING);
	}

	if (FIFOStatus & VLSB_ClockPhaseError)
	{
		mde_messanger_->sendMessage(this, "FIFO Clock phase error in recovered data from VLSB", MDE_WARNING);
	}

	uint32_t BankLength = ReadBankLength();
	if (BankLength == 0)
	{
		mde_messanger_->sendMessage(this, "No Data in Bank", MDE_INFO);
	}

	// Read bank data into MDE Data pointer:
	bool Status = ReadBankData(mde_dataPtr_, BankLength);
	if (not Status)
	{
		mde_messanger_->sendMessage(this, "Error in readout.", MDE_INFO);
	}

	mde_dataPtr_ += BankLength;

	// Bank length is 32 bits of data... The number of bytes is
	// four times this value:
	return BankLength;
}

// 
int  MDEVLSBBank::ReadFIFOStatus()
{
  // the bank ID (0-3) which is to be readout:
  int BankID =  (*this)["VLSBBank"];
  
  // Read and validate the FIFO Status:
  mde_current_address_ = mde_base_address_ + VLSB_FIFO_FLAGS;
  mde_data_32_ = 0;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

  if ( mde_vmeStatus_ != cvSuccess ) {
      mde_messanger_->sendMessage(this, "Unable to read VLSB Bank FIFO statuses", MDE_WARNING);
      return false;
    }

  // There are 4 banks, and there are four flags in order so..
  // [0:3] fifo empty, [4:7] fifo full, and so on
  // Bit shift by bank ID to get to something eaisily comparable..
  uint32_t aligned = (mde_data_32_ & 0xFFFFF) >> BankID;

  //MESSAGESTREAM << "Read: " << std::hex <<  mde_data_32_ << "    shifted: " <<   aligned << "  by: " << std::dec << BankID;
  //mde_messanger_->sendMessage(MDE_INFO);

  return aligned;
}

bool  MDEVLSBBank::FIFOOK()
{
	return ReadFIFOStatus() == VLSB_FIFOEmpty;
}

uint32_t MDEVLSBBank::ReadBankLength()
{
	// Determine the bank register to read:
	switch((*this)["VLSBBank"])
	{
		case 0:
			mde_current_address_ =  mde_base_address_ + VLSB_BANK0_LENGTH;
			break;
		case 1:
			mde_current_address_ = mde_base_address_ +  VLSB_BANK1_LENGTH;
			break;
		case 2:
			mde_current_address_ = mde_base_address_ + VLSB_BANK2_LENGTH;
			break;
		case 3:
			mde_current_address_ = mde_base_address_ + VLSB_BANK3_LENGTH;
			break;
		default:
			mde_messanger_->sendMessage(this, "Invalid VLSBBank", MDE_FATAL);
			return 0;
	}

	// Do the VME read..
	uint32_t BankLength = 0;
	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &BankLength);

	//MESSAGESTREAM << "Bank Length Read " << std::hex << BankLength
	//			<< " from " << mde_current_address_;

	//mde_messanger_->sendMessage(MDE_INFO);

	if ( mde_vmeStatus_ != cvSuccess )
	{
		mde_messanger_->sendMessage(this, "Unable to read VLSB Bank Length", MDE_FATAL);
		return 0;
	}

	return BankLength;
}

bool MDEVLSBBank::ReadBankData(MDE_Pointer *dataPtr, uint32_t BankLength)
{

	// Determine the bank register to read:
	switch((*this)["VLSBBank"])
	{
		case 0:
			mde_current_address_ =  mde_base_address_ + VLSB_BANK0_DATA;
			break;
		case 1:
			mde_current_address_ = mde_base_address_ +  VLSB_BANK1_DATA;
			break;
		case 2:
			mde_current_address_ = mde_base_address_ + VLSB_BANK2_DATA;
			break;
		case 3:
			mde_current_address_ = mde_base_address_ + VLSB_BANK3_DATA;
			break;
		default:
			mde_messanger_->sendMessage(this, "Invalid VLSBBank", MDE_FATAL);
			return 0;
	}

	std::vector<uint32_t> Addresses (BankLength, 0);

	for (uint i=0; i<BankLength; i++) Addresses[i] = mde_current_address_+ 4*i;

	timespec time_start, time_end;
	clock_gettime(CLOCK_MONOTONIC, &time_start);

	// Test of normall read:
	VME_MULTIREAD_32(&(Addresses[0]), dataPtr, BankLength);
	//for (uint32_t i(0); i<BankLength; i++) VME_READ_32(Addresses[i], dataPtr+i );


	clock_gettime(CLOCK_MONOTONIC, &time_end);

	// Time Difference:
	long diff_ns = time_end.tv_nsec - time_start.tv_nsec;
	if (diff_ns < 0) diff_ns += 1000000000;

	/*
	MESSAGESTREAM << "ReadVME words:" << BankLength
				  << "  time:" << (double)diff_ns/1000.0
				  << "  speed:" << ((double)BankLength*1000.)/diff_ns
				  << " M-words/Sec";
	mde_messanger_->sendMessage(MDE_INFO);
	*/


	if ( mde_vmeStatus_ != cvSuccess ) {
			mde_messanger_->sendMessage(this, "Unable to read VLSB Bank Data", MDE_FATAL);
			return false;
	}

	return true;
}

/* // Useless, BLT is not in firmware.
bool MDEVLSBBank::ReadBankDataBLT(MDE_Pointer *dataPtr, uint32_t BankLength)
{

	// Determine the bank register to read:
	switch((*this)["VLSBBank"])
	{
		case 0:
			mde_current_address_ =  mde_base_address_ + VLSB_BANK0_DATA;
			break;
		case 1:
			mde_current_address_ = mde_base_address_ +  VLSB_BANK1_DATA;
			break;
		case 2:
			mde_current_address_ = mde_base_address_ + VLSB_BANK2_DATA;
			break;
		case 3:
			mde_current_address_ = mde_base_address_ + VLSB_BANK3_DATA;
			break;
		default:
			mde_messanger_->sendMessage(this, "Invalid VLSBBank", MDE_FATAL);
			return 0;
	}
	//std::cout << "Reading... " << dataPtr << " for " << BankLength << std::endl;
	// Perform readout of bank:


	//[in]  Address	: The VME bus address.
	//[out] Buffer	: The data read from the VME bus.
	//[in]  Size      : The size of the transfer in bytes.
	//[in]  AM		: The address modifier (see CVAddressModifier enum).
	//[in]  DW		: The data width.(see CVDataWidth enum).
	//[out] count		: The number of bytes transferred.


	// Note that the block transfer is limited to a maximum number of bytes:
	const uint32_t MaxBLTWords(256);
	uint32_t nTransferredWords(0);
	while (true)
	{
		int nbRead(0);

		// Logic to decide on transfer length and exit if needed
		uint32_t thisTransfer = BankLength - nTransferredWords;
		if (thisTransfer == 0) break;
		if (thisTransfer>MaxBLTWords) thisTransfer = MaxBLTWords;

		// Perform Transfer:
		VME_READ_BLT_32(mde_current_address_ + nTransferredWords,
				dataPtr+nTransferredWords,thisTransfer,&nbRead );

		// Check output:
		nTransferredWords + nbRead/4;

		if ( mde_vmeStatus_ != cvSuccess ) {
			mde_messanger_->sendMessage(this, "Unable to read VLSB Bank Data", MDE_FATAL);
			return false;
		}

		//MESSAGESTREAM << "Number of Words Read: " << nTransferredWords << std::endl
		 //             << "The VME status code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
		    //mde_messanger_->sendMessage(MDE_WARNING);
	}

	return true;
}
*/
