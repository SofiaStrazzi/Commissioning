/// @author Roberto Preghenella
/// @since  2020-03-08

#include "Framework/Task.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/ConfigParamSpec.h"
#include "Framework/ConfigParamRegistry.h"
#include "Framework/ControlService.h"
#include "Framework/CallbackService.h"
#include "Framework/ConcreteDataMatcher.h"
#include "Framework/RawDeviceService.h"
#include "Framework/DeviceSpec.h"
#include <fairmq/FairMQDevice.h>

#include "Headers/RAWDataHeader.h"
#include "DataFormatsTOF/RawDataFormat.h"

using namespace o2::framework;

class FileProxyTask : public Task
{
 public:
  FileProxyTask() = default;
  ~FileProxyTask() override = default;

  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;
  
 private:

  long readFLP();
  long readCONET();

  bool mStatus = false;
  bool mCONET = false;
  bool mDumpData = false;
  std::ifstream mFile;
  char mBuffer[1048576];
};

void
FileProxyTask::init(InitContext& ic)
{
  auto infilename = ic.options().get<std::string>("infilename");
  mCONET = ic.options().get<bool>("CONET");
  mDumpData = ic.options().get<bool>("dump-data");

  /** open input file **/
  std::cout << " --- Opening input file: " << infilename << std::endl;
  mFile.open(infilename, std::fstream::in | std::fstream::binary);
  if (!mFile.is_open()) {
    std::cout << " --- File is not open " << std::endl;
    mStatus = true;
  }

  if (mCONET) {
    std::cout << " --- CONET mode " << std::endl; 
  }

};

long
FileProxyTask::readFLP()
{
  /** read input file **/
  char *pointer = mBuffer;
  if (!mFile.read(pointer, 64)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  long payload = 64;
  auto rdh = reinterpret_cast<o2::header::RAWDataHeader*>(pointer);
  while (!rdh->stop) {
    if (!mFile.read(pointer + rdh->headerSize, rdh->offsetToNext - rdh->headerSize)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
    payload += (rdh->offsetToNext - rdh->headerSize);
    pointer += rdh->offsetToNext;
    if (!mFile.read(pointer, 64)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
    payload += 64;
    rdh = reinterpret_cast<o2::header::RAWDataHeader*>(pointer);
  }
  
  return payload;
}

long
FileProxyTask::readCONET()
{

  char *pointer = mBuffer;
  const uint32_t *word;

  /** read TOF data header **/
  if (!mFile.read(pointer, 4)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  word = reinterpret_cast<const uint32_t *>(pointer);
  if (*word == 0x00080000) {
    printf(" --- unrecognised word: %08x \n ", *word);
    if (!mFile.read(pointer, 4)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
  }
  auto tofDataHeader = reinterpret_cast<const o2::tof::raw::TOFDataHeader_t*>(pointer);
  auto bytePayload = tofDataHeader->bytePayload;

  /** read payload **/
  if (!mFile.read(pointer + 4, bytePayload)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  return bytePayload + 4;
}

void
FileProxyTask::run(ProcessingContext& pc)
{

  /** check status **/
  if (mStatus) {
    mFile.close();
    pc.services().get<ControlService>().endOfStream();
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }

  /** read data **/
  int payload = 0;
  if (mCONET) payload = readCONET();
  else payload = readFLP();
  if (payload == 0) return;

  if (mDumpData) {
    std::cout << " --- dump data: " << payload << " bytes" << std::endl;
    uint32_t *word = reinterpret_cast<uint32_t *>(mBuffer);
    for (int i = 0; i < payload / 4; ++i) {
      printf(" 0x%08x \n", *word);
      word++;
    }
    std::cout << " --- end of dump data " << std::endl;
  }

  /** output **/
  auto device = pc.services().get<o2::framework::RawDeviceService>().device();
  auto outputRoutes = pc.services().get<o2::framework::RawDeviceService>().spec().outputs;
  auto fairMQChannel = outputRoutes.at(0).channel;  
  auto payloadMessage = device->NewMessage(payload);
  std::memcpy(payloadMessage->GetData(), mBuffer, payload);
  o2::header::DataHeader header("RAWDATA", "TOF", 0);
  header.payloadSize = payload;
  o2::framework::DataProcessingHeader dataProcessingHeader{0};
  o2::header::Stack headerStack{header, dataProcessingHeader};
  auto headerMessage = device->NewMessage(headerStack.size());
  std::memcpy(headerMessage->GetData(), headerStack.data(), headerStack.size());
  
  /** send **/
  FairMQParts parts;
  parts.AddPart(std::move(headerMessage));
  parts.AddPart(std::move(payloadMessage));
  device->Send(parts, fairMQChannel);

  /** check end of file **/
  if (mFile.eof()) {
    std::cout << " --- End of file " << std::endl;
    mStatus = true;
  }
  
};


// add workflow options, note that customization needs to be declared before
// including Framework/runDataProcessing
void customize(std::vector<ConfigParamSpec>& workflowOptions)
{}

#include "Framework/runDataProcessing.h" // the main driver

/// This function hooks up the the workflow specifications into the DPL driver.
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec {
    DataProcessorSpec {"file-proxy",
	Inputs{},
	Outputs{OutputSpec(ConcreteDataTypeMatcher{"TOF", "RAWDATA"})},
	AlgorithmSpec(adaptFromTask<FileProxyTask>()),
	Options{
	  {"infilename", VariantType::String, "", {"Input file name"}},
	  {"dump-data", VariantType::Bool, false, {"Dump data"}},
	  {"CONET", VariantType::Bool, false, {"CONET mode"}}}
    }
  };
}
