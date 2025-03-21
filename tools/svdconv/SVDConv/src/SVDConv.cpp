/*
 * Copyright (c) 2010-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "SVDConv.h"
#include "SvdOptions.h"
#include "SvdTypes.h"
#include "XMLTreeSlim.h"
#include "ErrLog.h"
#include "ErrOutputterSaveToStdoutOrFile.h"
#include "SvdModel.h"
#include "SvdDevice.h"
#include "SvdGenerator.h"
#include "SvdPeripheral.h"
#include "SvdAddressBlock.h"
#include "SvdInterrupt.h"
#include "SvdRegister.h"
#include "SvdCluster.h"
#include "SvdDimension.h"
#include "SvdField.h"
#include "SvdEnum.h"
#include "RteFsUtils.h"
#include "CrossPlatformUtils.h"
#include "ProductInfo.h"
#include "ParseOptions.h"
#include "EnumStringTables.h"
#include <nlohmann/json.hpp>

#include <ostream>
#include <string>
#include <set>
#include <list>
#include <map>
#include <csignal>
#include <bitset>

using namespace std;
using json = nlohmann::json;

#define WHITESPACES 4

SvdModel* m_svdModel;

std::ostream& operator<<(std::ostream& os, const SvdTypes::Access& access) {
  switch (access) {
    case SvdTypes::Access::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::Access::READONLY:
      os << "READ_ONLY";
      break;
    case SvdTypes::Access::WRITEONLY:
      os << "WRITE_ONLY";
      break;
    case SvdTypes::Access::READWRITE:
      os << "READ_WRITE";
      break;
    case SvdTypes::Access::WRITEONCE:
      os << "WRITE_ONCE";
      break;
    case SvdTypes::Access::READWRITEONCE:
      os << "READ_WRITE_ONCE";
      break;
    case SvdTypes::Access::END:
      os << "END";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::ProtectionType& protection) {
  switch (protection) {
    case SvdTypes::ProtectionType::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::ProtectionType::NONSECURE:
      os << "NONSECURE";
      break;
    case SvdTypes::ProtectionType::SECURE:
      os << "SECURE";
      break;
    case SvdTypes::ProtectionType::PRIVILEGED:
      os << "PRIVILEGED";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::SauAccessType& access) {
  switch (access) {
    case SvdTypes::SauAccessType::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::SauAccessType::NONSECURE:
      os << "NONSECURE";
      break;
    case SvdTypes::SauAccessType::SECURE:
      os << "SECURE";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::Expression& expr) {
  switch (expr) {
    case SvdTypes::Expression::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::Expression::NONE:
      os << "NONE";
      break;
    case SvdTypes::Expression::EXTEND:
      os << "EXTEND";
      break;
    case SvdTypes::Expression::ARRAY:
      os << "ARRAY";
      break;
    case SvdTypes::Expression::INVALID:
      os << "INVALID";
      break;
    case SvdTypes::Expression::ARRAYINVALID:
      os << "ARRAYINVALID";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::AddrBlockUsage& usage) {
  switch (usage) {
    case SvdTypes::AddrBlockUsage::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::AddrBlockUsage::REGISTERS:
      os << "REGISTERS";
      break;
    case SvdTypes::AddrBlockUsage::BUFFER:
      os << "BUFFER";
      break;
    case SvdTypes::AddrBlockUsage::RESERVED:
      os << "RESERVED";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::Endian& endian) {
  switch (endian) {
    case SvdTypes::Endian::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::Endian::LITTLE:
      os << "LITTLE";
      break;
    case SvdTypes::Endian::BIG:
      os << "BIG";
      break;
    case SvdTypes::Endian::SELECTABLE:
      os << "SELECTABLE";
      break;
    case SvdTypes::Endian::OTHER:
      os << "OTHER";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::ReadAction& action) {
  switch (action) {
    case SvdTypes::ReadAction::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::ReadAction::CLEAR:
      os << "CLEAR";
      break;
    case SvdTypes::ReadAction::SET:
      os << "SET";
      break;
    case SvdTypes::ReadAction::MODIFY:
      os << "MODIFY";
      break;
    case SvdTypes::ReadAction::MODIFEXT:
      os << "MODIFEXT";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::EnumUsage& usage) {
  switch (usage) {
    case SvdTypes::EnumUsage::UNDEF:
      os << "UNDEF";
      break;
    case SvdTypes::EnumUsage::READ:
      os << "READ";
      break;
    case SvdTypes::EnumUsage::WRITE:
      os << "WRITE";
      break;
    case SvdTypes::EnumUsage::READWRITE:
      os << "READWRITE";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::SvdConvV2accType& accType) {
  switch (accType) {
    case SvdTypes::SvdConvV2accType::EMPTY:
      os << "EMPTY";
      break;
    case SvdTypes::SvdConvV2accType::READ:
      os << "READ";
      break;
    case SvdTypes::SvdConvV2accType::READONLY:
      os << "READONLY";
      break;
    case SvdTypes::SvdConvV2accType::WRITE:
      os << "WRITE";
      break;
    case SvdTypes::SvdConvV2accType::WRITEONLY:
      os << "WRITEONLY";
      break;
    case SvdTypes::SvdConvV2accType::READWRITE:
      os << "READWRITE";
      break;
    case SvdTypes::SvdConvV2accType::UNDEF:
      os << "UNDEF";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::CpuIrqNum& irqNum) {
  switch (irqNum) {
    case SvdTypes::CpuIrqNum::IRQ0:
      os << "IRQ0";
      break;
    case SvdTypes::CpuIrqNum::IRQ1:
      os << "IRQ1";
      break;
    case SvdTypes::CpuIrqNum::IRQ2:
      os << "IRQ2";
      break;
    case SvdTypes::CpuIrqNum::IRQ3:
      os << "IRQ3";
      break;
    case SvdTypes::CpuIrqNum::IRQ4:
      os << "IRQ4";
      break;
    case SvdTypes::CpuIrqNum::IRQ5:
      os << "IRQ5";
      break;
    case SvdTypes::CpuIrqNum::IRQ6:
      os << "IRQ6";
      break;
    case SvdTypes::CpuIrqNum::IRQ7:
      os << "IRQ7";
      break;
    case SvdTypes::CpuIrqNum::IRQ8:
      os << "IRQ8";
      break;
    case SvdTypes::CpuIrqNum::IRQ9:
      os << "IRQ9";
      break;
    case SvdTypes::CpuIrqNum::IRQ10:
      os << "IRQ10";
      break;
    case SvdTypes::CpuIrqNum::IRQ11:
      os << "IRQ11";
      break;
    case SvdTypes::CpuIrqNum::IRQ12:
      os << "IRQ12";
      break;
    case SvdTypes::CpuIrqNum::IRQ13:
      os << "IRQ13";
      break;
    case SvdTypes::CpuIrqNum::IRQ14:
      os << "IRQ14";
      break;
    case SvdTypes::CpuIrqNum::IRQ15:
      os << "IRQ15";
      break;
    case SvdTypes::CpuIrqNum::IRQ_END:
      os << "IRQ_END";
      break;
    case SvdTypes::CpuIrqNum::IRQ_RESERVED:
      os << "IRQ_RESERVED";
      break;
    case SvdTypes::CpuIrqNum::IRQ_UNDEF:
      os << "IRQ_UNDEF";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const SvdTypes::ModifiedWriteValue& modWriteVal) {
  switch (modWriteVal) {
    case SvdTypes::ModifiedWriteValue::UNDEF:
      os << "undefined";
      break;
    case SvdTypes::ModifiedWriteValue::ONETOCLEAR:
      os << "oneToClear";
      break;
    case SvdTypes::ModifiedWriteValue::ONETOSET:
      os << "oneToSet";
      break;
    case SvdTypes::ModifiedWriteValue::ONETOTOGGLE:
      os << "oneToToggle";
      break;
    case SvdTypes::ModifiedWriteValue::ZEROTOCLEAR:
      os << "zeroToClear";
      break;
    case SvdTypes::ModifiedWriteValue::ZEROTOSET:
      os << "zeroToSet";
      break;
    case SvdTypes::ModifiedWriteValue::ZEROTOTOGGLE:
      os << "zeroToToggle";
      break;
    case SvdTypes::ModifiedWriteValue::CLEAR:
      os << "clear";
      break;
    case SvdTypes::ModifiedWriteValue::SET:
      os << "set";
      break;
    case SvdTypes::ModifiedWriteValue::MODIFY:
      os << "modify";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  return os;
}

template <typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

/**
 * @brief exception handler for other than C++/STL exceptions
*/
void Sighandler(int signum)
{
  string criticalErrMsg = "Exception or Segmentation fault occurred!\n  ";
  criticalErrMsg += to_string(signum);
  criticalErrMsg += " : ";

  switch(signum) {
    case SIGINT:
      criticalErrMsg += "interrupt";
      break;
    case SIGILL:
      criticalErrMsg += "illegal instruction - invalid function image";
      break;
    case SIGFPE:
      criticalErrMsg += "floating point exception";
      break;
    case SIGSEGV:
      criticalErrMsg += "segment violation";
      break;
    case SIGTERM:
      criticalErrMsg += "Software termination signal from kill";
      break;
    //case SIGBREAK:
    //  criticalErrMsg += "Ctrl-Break sequence";
    //  break;
    case SIGABRT:
    //case SIGABRT_COMPAT:
      criticalErrMsg += "abnormal termination triggered by abort call";
      break;
    default:
      criticalErrMsg += "unknown exception";
      break;
  }

  cout << criticalErrMsg << endl;
  LogMsg("M104", MSG(criticalErrMsg));
  ErrLog::Get()->Save();
  exit(2);
}


/**
 * @brief class SvdConv constructor
*/
SvdConv::SvdConv()
{
  ErrLog::Get()->SetOutputter(new ErrOutputterSaveToStdoutOrFile());
  InitMessageTable();
}

/**
* @brief class SvdConv destructor
*/
SvdConv::~SvdConv()
{
}

/**
 * @brief initialize the ErrLog Messages table for error reporting
 * @return passed / failed
*/
bool SvdConv::InitMessageTable()
{
  PdscMsg::AddMessages(msgTable);
  PdscMsg::AddMessagesStrict(msgStrictTable);

  return true;
}

/**
 * @brief SvdConv wrapper main entry point. Parses arguments and executes the tests
 * @param argc command line argument
 * @param argv command line argument
 * @param envp command line argument (not used)
 * @return passed / failed
*/
int SvdConv::Check(int argc, const char* argv[], const char* envp[])
{
  for (auto s : { SIGSEGV, SIGINT, SIGILL, SIGFPE, SIGSEGV, SIGTERM, /*SIGBREAK,*/ SIGABRT, /*SIGABRT_COMPAT*/ } ) {
    signal(s, Sighandler);  // catch fault
  }

  try {
#if 0   // Exception Test Code
    int *testPtr = (int *) 0x12345678;
    *testPtr = 0;
    string testStr = "a";
    testStr.compare(testStr.length() - 2, 2, "ab");
#endif

    ParseOptions parseOptions(m_svdOptions);
    const string header = m_svdOptions.GetHeader();

    ParseOptions::Result result = parseOptions.Parse(argc, argv);

    if(!m_svdOptions.GetLogPath().empty()) {
      cout << header << endl;
    }

    switch(result) {
      case ParseOptions::Result::Ok:
        break;
      case ParseOptions::Result::ExitNoError:
        return 0;
      case ParseOptions::Result::Error:
        return 1;
    }

    // Add date and time to log file
    if(!m_svdOptions.GetLogPath().empty()) {
      string dateTime = m_svdOptions.GetCurrentDateTime();
      LogMsg("M002", TXT("Log created on "), TXT2(dateTime));
    }

    parseOptions.PrintCommandLine();
    ErrLog::Get()->CheckSuppressMessages();
    LogMsg("M061");  // Checking Package Description

    CheckSvdFile();
  }
  catch(std::exception& e) {
    string criticalErrMsg = "STL exception occurred: ";
    criticalErrMsg += e.what();
    cout << criticalErrMsg << endl;
    LogMsg("M104", MSG(criticalErrMsg));
    ErrLog::Get()->Save();
    return 2;
  }
  catch(...) {
    string criticalErrMsg = "Unknown exception occurred!";
    cout << criticalErrMsg << endl;
    LogMsg("M104", MSG(criticalErrMsg));
    ErrLog::Get()->Save();
    return 2;
  }

  int errCnt  = ErrLog::Get()->GetErrCnt();
  int warnCnt = ErrLog::Get()->GetWarnCnt();

  LogMsg("M016");
  LogMsg("M022", ERR(errCnt), WARN(warnCnt));

  if(!m_svdOptions.GetLogPath().empty()) {
    cout << "Found " << errCnt << " Error(s) and " << warnCnt << " Warning(s)." << endl;
  }

  if(errCnt) {
    return 2;
  }
  else if(warnCnt) {
    return 1;
  }

  return 0;
}

SVD_ERR SvdConv::CheckSvdFile()
{
  uint32_t tAll = CrossPlatformUtils::ClockInMsec();

  SVD_ERR svdRes = SVD_ERR_SUCCESS;
  XMLTreeSlim* xmlTree;
  const string& path = m_svdOptions.GetSvdFullpath();

  const string version = VERSION_STRING;
  const string descr = PRODUCT_NAME;
  const string copyright = COPYRIGHT_NOTICE;

  LogMsg("M051", PATH(path));
  if(!RteFsUtils::Exists(path)) {
    LogMsg("M123", PATH(path));
    svdRes = SVD_ERR_NOT_FOUND;
    return svdRes;
  }

  xmlTree = new XMLTreeSlim();
  xmlTree->AddFileName(path);

  // ----------------------  Read XML  ----------------------
  uint32_t t1 = CrossPlatformUtils::ClockInMsec();
	bool success = xmlTree->ParseAll();
  uint32_t t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Reading SVD File"), TIME(t2)); }
	else        { LogMsg("M111", NAME("Reading SVD File"));           }

  // ----------------------  Construct Model  ----------------------
  if (m_svdOptions.IsUnderTest()) {
    string inFile = m_svdOptions.GetSvdFileName();
    try {
      const fs::path inPath = inFile;
      const auto inFilename = inPath.filename();
      const auto fnString = inFilename.string();
     ErrLog::Get()->SetFileName(fnString);
    }
    catch (const fs::filesystem_error&) {
     ErrLog::Get()->SetFileName(inFile);
    }
  }
  else if (m_svdOptions.IsSuppressPath()) {
    string inFile = m_svdOptions.GetSvdFileName();
   ErrLog::Get()->SetFileName(inFile);
  }
  else {
   ErrLog::Get()->SetFileName(path);
  }

  t1 = CrossPlatformUtils::ClockInMsec();
  m_svdModel = new SvdModel(0);
  m_svdModel->SetInputFileName(path);
  m_svdModel->SetShowMissingEnums();
  success = m_svdModel->Construct(xmlTree);
  t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Constructing Model"), TIME(t2)); }
	else        { LogMsg("M111", NAME("Constructing Model"));           }

  // ----------------------  Delete XML Tree  ----------------------
  t1 = CrossPlatformUtils::ClockInMsec();
  delete xmlTree;
  t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Deleting XML Tree"), TIME(t2));  }
	else        { LogMsg("M111", NAME("Deleting XML Tree"));            }

  // ----------------------  Calculate Model  ----------------------
  t1 = CrossPlatformUtils::ClockInMsec();
  success = m_svdModel->CalculateModel();
  t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Calculating Model"), TIME(t2));  }
	else        { LogMsg("M111", NAME("Calculating Model"));            }
  // ----------------------  Validate Model  ----------------------
  t1 = CrossPlatformUtils::ClockInMsec();
	success = m_svdModel->Validate();
  t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Validating Model"), TIME(t2)); }
	else        { LogMsg("M111", NAME("Validating Model"));           }

  // ----------------------  GetModel: device  ----------------------
  SvdDevice  *device = m_svdModel->GetDevice();

  if(device && m_svdOptions.IsCreateFields() && !m_svdOptions.IsCreateFieldsAnsiC()) {     // if fields are generated, we have annon unions
    device->SetHasAnnonUnions();
  }

  // ----------------------  Create Generator  ----------------------
  SvdGenerator *generator = new SvdGenerator(m_svdOptions);
  string outDir = m_svdOptions.GetOutputDirectory();

  // ----------------------  Generate Listings  ----------------------
  if(m_svdOptions.IsGenerateMap()) {
    t1 = CrossPlatformUtils::ClockInMsec();

    if(device) {
      generator->SetSvdFileName(path);
      generator->SetProgramInfo(version, descr, copyright);

      if(m_svdOptions.IsGenerateMapPeripheral()) {
        success = generator->PeripheralListing  (device, outDir);
      }
      if(m_svdOptions.IsGenerateMapRegister()) {
        success = generator->RegisterListing    (device, outDir);
      }
      if(m_svdOptions.IsGenerateMapField()) {
        success = generator->FieldListing       (device, outDir);
      }
    }
    t2 = CrossPlatformUtils::ClockInMsec() - t1;

    if(success) { LogMsg("M040", NAME("Generate Listing File"), TIME(t2));}
	  else        { LogMsg("M111", NAME("Generate Listing File"));          }
  }

  // ----------------------  Generate CMSIS Headerfile  ----------------------
  if(m_svdOptions.IsGenerateHeader()) {
    t1 = CrossPlatformUtils::ClockInMsec();
    if(device) {
      generator->SetSvdFileName(path);
      generator->SetProgramInfo(version, descr, copyright);
      success = generator->CmsisHeaderFile(device, outDir);
    }
    t2 = CrossPlatformUtils::ClockInMsec() - t1;

    if(success) { LogMsg("M040", NAME("Generate CMSIS Headerfile"), TIME(t2)); }
	  else        { LogMsg("M111", NAME("Generate CMSIS Headerfile"));           }
  }

  // ----------------------  Generate CMSIS Partitionfile  ----------------------
  if(m_svdOptions.IsGeneratePartition()) {
    t1 = CrossPlatformUtils::ClockInMsec();
    if(device) {
      generator->SetSvdFileName(path);
      generator->SetProgramInfo(version, descr, copyright);
      success = generator->CmsisPartitionFile(device, outDir);
    }
    t2 = CrossPlatformUtils::ClockInMsec() - t1;

    if(success) { LogMsg("M040", NAME("Generate CMSIS Partitionfile"), TIME(t2)); }
	  else        { LogMsg("M111", NAME("Generate CMSIS Partitionfile"));           }
  }

  // ----------------------  Generate SFD File  ----------------------
  if(m_svdOptions.IsGenerateSfd()) {
    t1 = CrossPlatformUtils::ClockInMsec();
    if(device) {
      generator->SetSvdFileName(path);
      generator->SetProgramInfo(version, descr, copyright);
      success = generator->SfdFile(device, outDir);
    }
    t2 = CrossPlatformUtils::ClockInMsec() - t1;

    if(success) { LogMsg("M040", NAME("Generate System Viewer SFD File"), TIME(t2)); }
	  else        { LogMsg("M111", NAME("Generate System Viewer SFD File"));           }
  }

  // ----------------------  Generate SFR File  ----------------------
  if(m_svdOptions.IsGenerateSfr()) {
    t1 = CrossPlatformUtils::ClockInMsec();
    if(device) {
      generator->SetSvdFileName(path);
      generator->SetProgramInfo(version, descr, copyright);
      success = generator->SfrFile(device, outDir);
    }
    t2 = CrossPlatformUtils::ClockInMsec() - t1;

    if(success) { LogMsg("M040", NAME("Generate System Viewer SFR File"), TIME(t2)); }
	  else        { LogMsg("M111", NAME("Generate System Viewer SFR File"));           }
  }


  if(m_svdOptions.IsDebugOutputModeText()) {
    PrintDevice(device);
  }

  if(m_svdOptions.IsDebugOutputModeJson()) {
    PrintDeviceJson(device);
  }

  // ----------------------  Delete Generator  ----------------------
  delete generator;

  // ----------------------  Delete Model  ----------------------
  t1 = CrossPlatformUtils::ClockInMsec();
  delete m_svdModel;
  t2 = CrossPlatformUtils::ClockInMsec() - t1;

  if(success) { LogMsg("M040", NAME("Deleting Model"), TIME(t2)); }
	else        { LogMsg("M111", NAME("Deleting Model"));           }

  t2 = CrossPlatformUtils::ClockInMsec() - tAll;
  LogMsg("M041", TIME(t2));

  return svdRes;
}

void SvdConv::PrintPeripheral(SvdPeripheral *const peri, const std::__cxx11::list<SvdInterrupt *> &interrupts)
{
  cout << "=== Peripheral " << peri->GetNameCalculated() << " ===" << endl;
  cout << "version: " << peri->GetVersion() << endl;
  // cout << "description: " << peri->GetDescription() << endl;
  cout << "alternatePeripheral: " << peri->GetAlternate() << endl;
  cout << "groupName: " << peri->GetGroupName() << endl;
  cout << "prependToName: " << peri->GetPrependToName() << endl;
  cout << "appendToName: " << peri->GetAppendToName() << endl;
  cout << "headerStructName: " << peri->GetHeaderStructName() << endl;
  
  if (peri->GetDisableCondition())
    cout << "disableCondition: " << peri->GetDisableCondition()->GetExpressionString() << endl;
  else 
    cout << "disableCondition: " << "NULL" << endl;
  
  cout << "baseAddress: 0x" << hex << peri->GetAddress() << dec << endl;
  cout << "size: " << peri->GetBitWidth() << endl;
  cout << "size effective: " << peri->GetEffectiveBitWidth() << endl;
  cout << "access: " << peri->GetEffectiveAccess() << endl;
  cout << "protection: " << peri->GetEffectiveProtection() << endl;
  cout << "resetValue: 0x" << hex << peri->GetEffectiveResetValue() << dec << endl;
  cout << "resetMask: 0x" << hex << peri->GetEffectiveResetMask() << dec << endl;
  cout << "\n";
  cout << "Absolute Address: 0x" << hex << peri->GetAbsoluteAddress() << dec << endl;
  cout << "Hierarchical Name: " << peri->GetHierarchicalNameResulting() << endl;
  cout << "\n";
  
  PrintAddressBlocks(peri);
  PrintInterrupts(interrupts);
  PrintRegistersContainer(peri);
  cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n\n";
}

void SvdConv::PrintAddressBlocks(SvdPeripheral *const peri)
{
  const auto &addrBlocks = peri->GetAddressBlocks();
  for (const auto &addrBlock : addrBlocks)
  {
    cout << "Address Block:" << endl;
    cout << "  Offset: 0x" << hex << addrBlock->GetOffset() << dec << endl;
    cout << "  Size: " << addrBlock->GetSize() << endl;
    cout << "  Usage: " << addrBlock->GetUsage() << endl;
    cout << "  Protection: " << addrBlock->GetProtection() << endl;
  }
  cout << "\n";
}

void SvdConv::PrintInterrupts(const std::__cxx11::list<SvdInterrupt *> &interrupts)
{
  for (const auto &interrupt : interrupts)
  {
    cout << "Interrupt:" << endl;
    cout << "  Name: " << interrupt->GetNameCalculated() << endl;
    // cout << "  Description: " << interrupt->GetDescription() << endl;
    cout << "  Value: " << interrupt->GetValue() << endl;
  }
  cout << "\n";
}

void SvdConv::PrintCluster(SvdCluster *cluster, int numSpaces)
{
  std::string space(numSpaces, ' ');

  cout << space << "=== Cluster " << cluster->GetNameCalculated() << " ===" << endl;
  // cout << space << "description: " << cluster->GetDescription() << endl;
  cout << space << "alternateCluster: " << cluster->GetAlternate() << endl;
  cout << space << "headerStructName: " << cluster->GetHeaderStructName() << endl;
  cout << space << "addressOffset: 0x" << hex << cluster->GetOffset() << dec << endl;
  cout << space << "size: " << cluster->GetBitWidth() << endl;
  cout << space << "size effective: " << cluster->GetEffectiveBitWidth() << endl;
  cout << space << "access: " << cluster->GetEffectiveAccess() << endl;
  cout << space << "protection: " << cluster->GetEffectiveProtection() << endl;
  cout << space << "resetValue: 0x" << hex << cluster->GetEffectiveResetValue() << dec << endl;
  cout << space << "resetMask: 0x" << hex << cluster->GetEffectiveResetMask() << dec << endl;
  cout << space << "\n";
  cout << space << "Absolute Address: 0x" << hex << cluster->GetAbsoluteAddress() << dec << endl;
  cout << space << "Hierarchical Name: " << cluster->GetHierarchicalNameResulting() << endl;
  cout << space << "\n";
}

void SvdConv::PrintRegister(SvdRegister *reg, int numSpaces)
{
  std::string space(numSpaces, ' ');

  cout << space << "=== Register " << reg->GetNameCalculated() << " (with prepend & append: " << reg->GetHeaderFileName() << ")" " ===" << endl;
  cout << space << "displayName: " << reg->GetDisplayName() << endl;
  // cout << space << "description: " << reg->GetDescription() << endl;
  cout << space << "alternateGroup: " << reg->GetAlternateGroup() << endl;
  cout << space << "alternateRegister: " << reg->GetAlternate() << endl;
  cout << space << "addressOffset: 0x" << hex << reg->GetOffset() << dec << endl;
  cout << space << "size: " << reg->GetBitWidth() << endl;
  cout << space << "size effective: " << reg->GetEffectiveBitWidth() << endl;
  cout << space << "access: " << reg->GetEffectiveAccess() << endl;
  cout << space << "protection: " << reg->GetEffectiveProtection() << endl;
  cout << space << "resetValue: 0x" << hex << reg->GetEffectiveResetValue() << dec << endl;
  cout << space << "resetMask: 0x" << hex << reg->GetEffectiveResetMask() << dec << endl;
  cout << space << "dataType: " << reg->GetDataType() << endl;
  cout << space << "modifiedWriteValues: " << reg->GetModifiedWriteValue() << endl;
  cout << space << "writeConstraint: " << (reg->HasWriteConstraint() ? "yes" : "no") << endl;
  cout << space << "readAction: " << reg->GetReadAction() << endl;
  cout << space << "\n";
  cout << space << "Absolute Address: 0x" << hex << reg->GetAbsoluteAddress() << dec << endl;
  cout << space << "Hierarchical Name: " << reg->GetHierarchicalNameResulting() << endl;
  cout << space << "\n";
}

void SvdConv::PrintField(SvdField *field, int numSpaces)
{
  std::string space(numSpaces, ' ');

  cout << space << "=== Field " << field->GetNameCalculated() << " ===" << endl; 
  // cout << space << "description: " << field->GetDescription() << endl;
  cout << space << "bitOffset: " << field->GetOffset() << endl;
  cout << space << "bitWidth: " << field->GetBitWidth() << endl;
  cout << space << "access: " << field->GetEffectiveAccess() << endl;
  cout << space << "modifiedWriteValues: " << field->GetModifiedWriteValue() << endl;
  cout << space << "writeConstraint: " << (field->HasWriteConstraint() ? "yes" : "no") << endl;
  cout << space << "readAction: " << field->GetReadAction() << endl;
  cout << space << "\n";
  cout << space << "Hierarchical Name: " << field->GetHierarchicalNameResulting() << endl;
  cout << space << "\n";
}

void SvdConv::PrintEnumContainer(SvdEnumContainer *const enumContainer, int numSpaces)
{
  std::string space(numSpaces, ' ');
  cout << space << "=== Enum Container: " << enumContainer->GetName() << " ===" << endl;
  cout << space << "headerEnumName: " << enumContainer->GetHeaderEnumName() << endl;
  cout << space << "usage: " << enumContainer->GetUsage() << endl;
  cout << space << "Hierarchical Name: " << enumContainer->GetHierarchicalNameResulting() << endl;
}

void SvdConv::PrintEnum(SvdEnum *const &enum_, int numSpaces)
{
  std::string space(numSpaces, ' ');

  cout << space << "Enum:" << endl;
  cout << space << "  name: " << enum_->GetNameCalculated() << endl;
  // cout << space << "  description: " << enum_->GetDescription() << endl;
  cout << space << "  value: 0b" << bitset<32>(enum_->GetValue().u32) << endl;
  cout << space << "  isDefault: " << boolalpha << enum_->IsDefault() << noboolalpha << endl;
  cout << space << "  Hierarchical Name: " << enum_->GetHierarchicalNameResulting() << endl;
  cout << space << "\n";
}

void SvdConv::PrintDevice(SvdDevice *device)
{
  const auto periCont = device->GetPeripheralContainer();
  const auto &periContChilds = periCont->GetChildren();
  for (const auto periSvdItem : periContChilds) {
    if(!periSvdItem || !periSvdItem->IsValid()) {
      continue;
    }
    
    const auto peri = dynamic_cast<SvdPeripheral *>(periSvdItem);
    const auto dim = periSvdItem->GetDimension();
    if(dim) {
      const auto &dimChild = dim->GetChildren();
      for (const auto child : dimChild) {
        if(!child || !child->IsValid()) {
          continue;
        }

        const auto peri_ = dynamic_cast<SvdPeripheral *>(child);
        PrintPeripheral(peri_, peri->GetInterrupt());
      }
    } else {
      PrintPeripheral(peri, peri->GetInterrupt());
    }
  }
}

void SvdConv::PrintRegistersContainer(SvdPeripheral *const peri)
{
  const auto regCont = peri->GetRegisterContainer();
  const auto &childs = regCont->GetChildren();
  PrintRegistersClusters(childs, WHITESPACES);
}

void SvdConv::PrintRegistersClusters(const std::__cxx11::list<SvdItem *> &registersClusters, int numSpaces)
{
  for (const auto registerCluster : registersClusters) {
    if (!registerCluster || !registerCluster->IsValid()) {
      continue;
    }

    if (auto reg = dynamic_cast<SvdRegister *>(registerCluster))
    {
      const auto dim = reg->GetDimension();
      if(dim) {
        const auto &dimChilden = dim->GetChildren();
        for (const auto child : dimChilden) {
          if(!child || !child->IsValid()) {
            continue;
          }

          const auto reg_ = dynamic_cast<SvdRegister *>(child);
          PrintRegisterWrapper(reg_, numSpaces);
        }
      } else {
        PrintRegisterWrapper(reg, numSpaces);
      }
    }
    else if (auto cluster = dynamic_cast<SvdCluster *>(registerCluster))
    {
      const auto dim = cluster->GetDimension();
      if(dim) {
        const auto &dimChilden = dim->GetChildren();
        for (const auto child : dimChilden) {
          if(!child || !child->IsValid()) {
            continue;
          }

          const auto cluster_ = dynamic_cast<SvdCluster *>(child);
          PrintClusterWrapper(cluster_, numSpaces);
        }
      } else {
        PrintClusterWrapper(cluster, numSpaces);
      }
    }
  }
}

void SvdConv::PrintRegisterWrapper(SvdRegister *reg, int numSpaces)
{
  PrintRegister(reg, numSpaces);

  const auto fieldCont = reg->GetFieldContainer();
  if (!fieldCont || !fieldCont->IsValid()) {
    return;
  }

  const auto &regContChilds = fieldCont->GetChildren();
  for (const auto regContChild : regContChilds)
  {
    if(!regContChild || !regContChild->IsValid()) {
      continue;
    }

    const auto dim = regContChild->GetDimension();
    if(dim) {
      const auto &dimChilds = dim->GetChildren();
      for (const auto dimChild : dimChilds) {
        if(!dimChild || !dimChild->IsValid()) {
          continue;
        }

        const auto field = dynamic_cast<SvdField *>(dimChild);
        PrintFieldWrapper(field, numSpaces);
      }
    } else {
      const auto field = dynamic_cast<SvdField *>(regContChild);
      PrintFieldWrapper(field, numSpaces + WHITESPACES);
    }

    cout << "\n\n";
  }
}

void SvdConv::PrintFieldWrapper(SvdField *const field, int numSpaces)
{
  PrintField(field, numSpaces + WHITESPACES);
  
  if (field && !field->GetChildren().empty()) {
    for(const auto &child : field->GetChildren()) {
      if(!child || !child->IsValid()) {
        continue;
      }
      
      const auto enumContainer = dynamic_cast<SvdEnumContainer *>(child);
      if(enumContainer) {
        PrintEnumContainerWrapper(enumContainer, numSpaces + WHITESPACES);
      }
    }
  }
}

void SvdConv::PrintEnumContainerWrapper(SvdEnumContainer *const enumContainer, int numSpaces)
{
  PrintEnumContainer(enumContainer, numSpaces + WHITESPACES);
  const auto &enums = enumContainer->GetChildren();
  for (const auto &child : enums)
  {
    if (!child || !child->IsValid())
    {
      continue;
    }
    const auto enum_ = dynamic_cast<SvdEnum *>(child);
    PrintEnum(enum_, numSpaces + 2 * WHITESPACES);
  }
}

void SvdConv::PrintClusterWrapper(SvdCluster *cluster, int numSpaces)
{
  PrintCluster(cluster, numSpaces);

  if (cluster && !cluster->GetChildren().empty())
  {
    PrintRegistersClusters(cluster->GetChildren(), numSpaces + WHITESPACES);
  }
}

json SvdConv::PeripheralToJson(SvdPeripheral *const peri, const std::list<SvdInterrupt *> &interrupts)
{
    json j;
    j["name"]                = peri->GetNameCalculated();
    j["version"]             = peri->GetVersion();
    // j["description"]      = peri->GetDescription(); // if needed
    j["alternatePeripheral"] = peri->GetAlternate();
    j["groupName"]           = peri->GetGroupName();
    j["prependToName"]       = peri->GetPrependToName();
    j["appendToName"]        = peri->GetAppendToName();
    j["headerStructName"]    = peri->GetHeaderStructName();
    
    if (peri->GetDisableCondition())
        j["disableCondition"] = peri->GetDisableCondition()->GetExpressionString();
    else 
        j["disableCondition"] = "NULL";
    
    j["baseAddress"]         = peri->GetAddress();
    j["size"]                = peri->GetBitWidth();
    j["sizeEffective"]       = peri->GetEffectiveBitWidth();
    j["access"]              = toString(peri->GetEffectiveAccess());
    j["protection"]          = toString(peri->GetEffectiveProtection());
    j["resetValue"]          = peri->GetEffectiveResetValue();
    j["resetMask"]           = peri->GetEffectiveResetMask();
    j["absoluteAddress"]     = peri->GetAbsoluteAddress();

    j["addressBlocks"]  = AddressBlocksToJson(peri);
    j["interrupts"]     = InterruptsToJson(interrupts);
    j["registersClusters"] = RegistersContainerToJson(peri);

    return j;
}

json SvdConv::AddressBlocksToJson(SvdPeripheral *const peri)
{
    json arr = json::array();
    const auto &addrBlocks = peri->GetAddressBlocks();
    for (const auto &addrBlock : addrBlocks)
    {
        json jBlock;
        jBlock["offset"]     = addrBlock->GetOffset();
        jBlock["size"]       = addrBlock->GetSize();
        jBlock["usage"]      = toString(addrBlock->GetUsage());
        jBlock["protection"] = toString(addrBlock->GetProtection());
        arr.push_back(jBlock);
    }
    return arr;
}

json SvdConv::InterruptsToJson(const std::list<SvdInterrupt *> &interrupts)
{
    json arr = json::array();
    for (const auto &intr : interrupts)
    {
        json jIntr;
        jIntr["name"]  = intr->GetNameCalculated();
        // jIntr["description"] = intr->GetDescription(); // if needed
        jIntr["value"] = intr->GetValue();
        arr.push_back(jIntr);
    }
    return arr;
}

json SvdConv::RegistersContainerToJson(SvdPeripheral *const peri)
{
  const auto regCont = peri->GetRegisterContainer();
  const auto &childs = regCont->GetChildren();
  return RegistersClustersToJson(childs);
}

json SvdConv::RegistersClustersToJson(const std::__cxx11::list<SvdItem *> &registersClusters)
{
  json registers_clusters_arr = json::array();
  for (const auto registerCluster : registersClusters) {
    if (!registerCluster || !registerCluster->IsValid()) {
      continue;
    }

    if (auto reg = dynamic_cast<SvdRegister *>(registerCluster))
    {
      const auto dim = reg->GetDimension();
      if(dim) {
        const auto &dimChilden = dim->GetChildren();
        for (const auto child : dimChilden) {
          if(!child || !child->IsValid()) {
            continue;
          }

          const auto reg_ = dynamic_cast<SvdRegister *>(child);
          registers_clusters_arr.push_back(RegisterToJson(reg_));
        }
      } else {
        registers_clusters_arr.push_back(RegisterToJson(reg));
      }
    }
    else if (auto cluster = dynamic_cast<SvdCluster *>(registerCluster))
    {
      const auto dim = cluster->GetDimension();
      if(dim) {
        const auto &dimChilden = dim->GetChildren();
        for (const auto child : dimChilden) {
          if(!child || !child->IsValid()) {
            continue;
          }

          const auto cluster_ = dynamic_cast<SvdCluster *>(child);
          registers_clusters_arr.push_back(ClusterToJson(cluster_));
        }
      } else {
        registers_clusters_arr.push_back(ClusterToJson(cluster));
      }
    }
  }

  return registers_clusters_arr;
}

json SvdConv::RegisterToJson(SvdRegister *reg)
{
    json j;
    j["type"]              = "register";
    j["name"]              = reg->SvdItem::GetNameCalculated();
    j["displayName"]       = reg->GetDisplayName();
    j["alternateGroup"]    = reg->GetAlternateGroup();
    j["alternateRegister"] = reg->GetAlternate();
    j["addressOffset"]     = reg->GetOffset();
    j["size"]              = reg->GetBitWidth();
    j["sizeEffective"]     = reg->GetEffectiveBitWidth();
    j["access"]            = toString(reg->GetEffectiveAccess());
    j["protection"]        = toString(reg->GetEffectiveProtection());
    j["resetValue"]        = reg->GetEffectiveResetValue();
    j["resetMask"]         = reg->GetEffectiveResetMask();
    j["dataType"]          = reg->GetDataType();
    j["modifiedWriteValues"]= toString(reg->GetModifiedWriteValue());
    j["writeConstraint"]   = (reg->HasWriteConstraint() ? "yes" : "no");
    j["readAction"]        = toString(reg->GetReadAction());
    j["absoluteAddress"]   = reg->GetAbsoluteAddress();

    // If the register has fields, add them
    const auto fieldCont = reg->GetFieldContainer();
    if (fieldCont && fieldCont->IsValid() && !fieldCont->GetChildren().empty())
    {
        json fieldsArr = json::array();
        for (const auto child : fieldCont->GetChildren())
        {
            if (!child || !child->IsValid())
                continue;

            const auto dim = child->GetDimension();
            if(dim) {
              const auto &dimChilds = dim->GetChildren();
              for (const auto dimChild : dimChilds) {
                if(!dimChild || !dimChild->IsValid()) {
                  continue;
                }
        
                const auto field = dynamic_cast<SvdField *>(dimChild);
                fieldsArr.push_back(FieldToJson(field));
              }
            } else {
              const auto field = dynamic_cast<SvdField *>(child);
              fieldsArr.push_back(FieldToJson(field));
            }
        }
        j["fields"] = fieldsArr;
    } else {
        j["fields"] = json::array();
    }
    return j;
}

json SvdConv::ClusterToJson(SvdCluster *cluster)
{
    json j;
    j["type"]              = "cluster";
    j["name"]              = cluster->GetNameCalculated();
    j["alternateCluster"]  = cluster->GetAlternate();
    j["headerStructName"]  = cluster->GetHeaderStructName();
    j["addressOffset"]     = cluster->GetOffset();
    j["size"]              = cluster->GetBitWidth();
    j["sizeEffective"]     = cluster->GetEffectiveBitWidth();
    j["access"]            = toString(cluster->GetEffectiveAccess());
    j["protection"]        = toString(cluster->GetEffectiveProtection());
    j["resetValue"]        = cluster->GetEffectiveResetValue();
    j["resetMask"]         = cluster->GetEffectiveResetMask();
    j["absoluteAddress"]   = cluster->GetAbsoluteAddress();

    // Add nested registers/clusters if any
    if (cluster && !cluster->GetChildren().empty())
    {
        j["registersClusters"] = RegistersClustersToJson(cluster->GetChildren());
    } else {
        j["registersClusters"] = json::array();
    }
    return j;
}

json SvdConv::FieldToJson(SvdField *field)
{
    json j;
    j["name"]         = field->GetNameCalculated();
    j["bitOffset"]    = field->GetOffset();
    j["bitWidth"]     = field->GetBitWidth();
    j["access"]       = toString(field->GetEffectiveAccess());
    j["modifiedWriteValues"] = toString(field->GetModifiedWriteValue());
    j["writeConstraint"]     = (field->HasWriteConstraint() ? "yes" : "no");
    j["readAction"]   = toString(field->GetReadAction());

    // If the field has enum containers, add them as an array.
    if (!field->GetChildren().empty())
    {
        json enumsArr = json::array();
        for (const auto &child : field->GetChildren())
        {
            if (!child || !child->IsValid())
                continue;
            if (const auto enumContainer = dynamic_cast<SvdEnumContainer *>(child))
            {
                enumsArr.push_back(EnumContainerToJson(enumContainer));
            }
        }
        j["enumContainers"] = enumsArr;
    } else {
        j["enumContainers"] = json::array();
    }
    return j;
}

json SvdConv::EnumContainerToJson(SvdEnumContainer *const enumContainer)
{
    json j;
    j["name"]            = enumContainer->GetName();
    j["headerEnumName"]  = enumContainer->GetHeaderEnumName();
    j["usage"]           = toString(enumContainer->GetUsage());

    // Add enums if available
    if (!enumContainer->GetChildren().empty())
    {
        json enumsArr = json::array();
        for (const auto &child : enumContainer->GetChildren())
        {
            if (!child || !child->IsValid())
                continue;
            if (const auto enum_ = dynamic_cast<SvdEnum *>(child))
            {
                enumsArr.push_back(EnumToJson(enum_));
            }
        }
        j["enumeratedValues"] = enumsArr;
    } else {
        j["enumeratedValues"] = json::array();
    }
    return j;
}

json SvdConv::EnumToJson(SvdEnum *const &enum_)
{
    json j;
    j["name"]  = enum_->GetNameCalculated();
    // Store the bitset as a string; you could convert it to an integer if that fits your needs.
    j["value"] = std::string("0b") + std::bitset<32>(enum_->GetValue().u32).to_string();
    j["isDefault"] = enum_->IsDefault();
    return j;
}

void SvdConv::PrintDeviceJson(SvdDevice *device)
{
    json jDevice = json::array();
    const auto periCont = device->GetPeripheralContainer();
    const auto &periContChilds = periCont->GetChildren();
    for (const auto periSvdItem : periContChilds) {
        if (!periSvdItem || !periSvdItem->IsValid())
            continue;
        if (auto peri = dynamic_cast<SvdPeripheral *>(periSvdItem)) {
            const auto dim = periSvdItem->GetDimension();
            if (dim) {
                for (const auto child : dim->GetChildren()) {
                    if (!child || !child->IsValid()) continue;
                    auto peri_ = dynamic_cast<SvdPeripheral *>(child);
                    jDevice.push_back(PeripheralToJson(peri_, peri->GetInterrupt()));
                }
            }
            else {
                jDevice.push_back(PeripheralToJson(peri, peri->GetInterrupt()));
            }
        }
    }
    std::cout << jDevice.dump(4) << std::endl;
}