#include "TrVMEDevice.hh"

TrVMEDevice::TrVMEDevice() {
}

void TrVMEDevice::initialise(int ba) {

	baseAddress = ba;
	addressModifier = cvA32_U_DATA;
	dataWidth = cvD32;
}

void TrVMEDevice::setVMEController(std::string controllerType) {
	vmeController = controllerType;
}

void TrVMEDevice::setDLink(int dLinkIn) {
	dLink = dLinkIn;
}

int TrVMEDevice::getBaseAddress() const {
	return baseAddress;
}
